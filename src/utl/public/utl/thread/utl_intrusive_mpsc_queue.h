// Original Copyright (c) 2010-2011 Dmitry Vyukov. All rights reserved.
// Modifications and redesign Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/intrusive/utl_intrusive_nodes.h"
#include "utl/intrusive/utl_node_ptr.h"
#include "utl/thread/utl_atomic_impl.h"
#include "utl/thread/utl_hardware_interference.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_declval.h"

#ifdef UTL_CXX20
UTL_NAMESPACE_BEGIN
namespace chrono {
template <typename R, typename P>
class duration;
};
template <typename...>
class tuple;

template <typename T>
struct is_duration : false_type {};
template <typename R, typename P>
struct is_duration<chrono::duration<R, P>> : true_type {};

namespace intrusive {
namespace details {
template <typename Node>
class mpsc_queue_base {
protected:
    static constexpr struct non_block_t {
    } non_block = {};

private:
    using node_type = Node;
    using pointer = node_type*;
    using const_pointer = node_type const*;
    using reference = node_type&;
    using const_reference = node_type const&;

    struct sentinel_t {
        pointer next;
    };

    template <typename T>
    struct is_dequeue_param : disjunction<is_same<T, non_block_t>, is_duration<T>> {};

    static constexpr UTL_ATTRIBUTES(NODISCARD) pointer* next_ptr(
        reference node UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
        return addressof(get_next_ref(node));
    }
    static constexpr UTL_ATTRIBUTES(CONST, NODISCARD) pointer* next_ptr(
        sentinel_t& sentinel UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
        return addressof(get_next_ref(sentinel));
    }
    static constexpr UTL_ATTRIBUTES(NODISCARD) pointer* next_ptr(
        const_reference node UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
        return next_ptr(const_cast<reference>(node));
    }
    static constexpr UTL_ATTRIBUTES(CONST, NODISCARD) pointer* next_ptr(
        sentinel_t const& sentinel UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
        return next_ptr(const_cast<sentinel_t&>(sentinel));
    }

protected:
    mpsc_queue_base() noexcept : sentinel_(), head_(&sentinel_), tail_(&sentinel_) {}
    mpsc_queue_base(mpsc_queue_base const&) noexcept = delete;
    mpsc_queue_base& operator=(mpsc_queue_base const&) noexcept = delete;
    mpsc_queue_base(mpsc_queue_base&&) noexcept = delete;
    mpsc_queue_base& operator=(mpsc_queue_base&&) noexcept = delete;
    ~mpsc_queue_base() = default;

    void enqueue_impl(pointer new_node) noexcept {
        UTL_ASSERT(new_node != nullptr);
        enqueue_impl(*new_node);
    }

    template <typename T>
    UTL_ATTRIBUTES(NODISCARD)
    enable_if<is_dequeue_param<T>::value, pointer> dequeue_impl(T parameter) noexcept {
        pointer output;
        pointer next_head;

        if (is_sentinel(head_)) {
            // sentinel is the head, so move one node forward if possible
            pointer next_node = load_next_from_sentinel(parameter);
            if (next_node == nullptr) {
                // empty queue
                return nullptr;
            }

            output = next_node;
            next_head = UTL_ATOMIC_LOAD(next_ptr(*next_node), memory_order_acquire);
        } else {
            output = head_;
            next_head = UTL_ATOMIC_LOAD(next_ptr(*head_), memory_order_acquire);
        }

        if (next_head == nullptr) {
            // next_head is nullptr, so we have just arrived at tail_; i.e. output == tail_ when we
            // loaded next_head
            if ((void*)output != tail_.load(std::memory_order_relaxed)) {
                // In this branch, someone is in the process of enqueuing,
                // the tail_ exchange has occured, but potentially the next pointer of output hasn't
                // been set Just exit, and let the caller call again
                return nullptr;
            }

            enqueue_sentinel();
            next_head = UTL_ATOMIC_LOAD(next_ptr(*output), memory_order_acquire);
            UTL_ASSERT(next_head != nullptr);
        }

        head_ = next_head;
        UTL_ATOMIC_STORE(next_ptr(*output), nullptr, memory_order_relaxed);
        return output;
    }

    void notify_one() noexcept(futex::signal((sentinel_t**)0, 1)) {
        // TODO: APPLE has no public futex implementation;
        // we need to conditionally add a futex emulator using semaphores for app store dev
        futex::notify(next_ptr(sentinel_), 1);
    }

    void notify_all() noexcept(futex::signal((sentinel_t**)0, npos_type::value)) {
        futex::notify(next_ptr(sentinel_), npos_type::value);
    }

private:
    UTL_ATTRIBUTES(NODISCARD)
    bool is_sentinel(const_pointer node) const noexcept {
        return (void const*)node == (void const*)addressof(sentinel_);
    }

    template <bool notify>
    void enqueue_impl(reference new_node) noexcept {
        set_next(new_node, nullptr);
        pointer ptr = (void*)addressof(new_node);
        auto previous = (pointer)UTL_ATOMIC_EXCHANGE(addressof(tail_), ptr, memory_order_acq_rel);
        if (is_sentinel(previous)) {
            UTL_ATOMIC_STORE(next_ptr(sentinel_), ptr, memory_order_release);
            if UTL_CONSTEXPR_CXX17 (notify) {
                notify_one();
            }
        } else {
            UTL_ATOMIC_STORE(next_ptr(*previous), ptr, memory_order_release);
        }
    }

    void enqueue_sentinel() noexcept {
        set_next(sentinel_, nullptr);
        void* ptr = (void*)addressof(sentinel_);
        auto previous = (pointer)UTL_ATOMIC_EXCHANGE(addressof(tail_), ptr, memory_order_acq_rel);
        UTL_ATOMIC_STORE(next_ptr(*previous), ptr, memory_order_release);
    }

    node_type* load_next_from_sentinel(non_block_t) const noexcept {
        return UTL_ATOMIC_LOAD(next_ptr(sentinel_), memory_order_acquire);
    }

    template <typename R, typename P>
    node_type* load_next_from_sentinel(chrono::duration<R, P> duration) const noexcept {
        futex::wait(next_ptr(sentinel_), nullptr, duration);
        return UTL_ATOMIC_LOAD(next_ptr(sentinel_), memory_order_relaxed);
    }

    sentinel_t sentinel_;
    alignas(hardware_destructive_interference_size) void* head_;
    alignas(hardware_destructive_interference_size) void* tail_;
};
} // namespace details

/**
 * Intrusive multiproducer, single consumer queue
 * Implementation is based on
 * https://www.1024cores.net/home/lock-free-algorithms/queues/intrusive-mpsc-node-based-queue
 * @tparam NodeType - Node type that the queue will use
 * * Declare a public non-static `std::atomic<NodeType*>` data member named `next_node`
 * * Declare a public non-static function member with the following signatures
 * `std::atomic<NodeType*>& Getnext_node()`
 * * Otherwise, the expression `Getnext_node(node)` returning `std::atomic<NodeType*>&` must be
 * valid through argument-dependent-lookup, where `node` is of type `NodeType*`
 * @see https://en.cppreference.com/w/cpp/language/adl
 *
 * @tparam Policy - Node operation policy; policies should implement the following API:
 *
 *  **
 *  * Retrieves/creates a pointer to a node from any set of arguments that will be passed from
 * `mspc_queue::enqueue`
 *  *
 * NodeType* acquire(Args...);
 *
 *  **
 *  * Releases ownership of the given node
 *  *
 * void release(NodeType*);
 */
template <atomic_unidirectional NodeType, release_invocable<NodeType> Policy>
class mspc_queue : private details::mpsc_queue_base<NodeType>, private Policy {
public:
    using node_type = NodeType;
    using node_policy = Policy;
    using node_pointer = node_ptr<node_type, node_policy>;
    using node_deleter = typename node_pointer::deleter_type;

private:
    using base_type = details::mpsc_queue_base<node_type>;
    using base_type::dequeue_impl;
    using base_type::enqueue_impl;
    using base_type::non_block;

    template <typename R, typename P>
    static chrono::duration<R, P>&& decl_duration() noexcept;
    static node_deleter&& decl_deleter() noexcept;

public:
    /**
     * Constructor
     *
     * @param args... - The policy class constructor arugments
     */
    template <typename... Args>
    requires is_constructible_v<node_policy, Args...>
    explicit mspc_queue(Args&&... args)
        : node_policy(forward<Args>(args)...)
        , tail_(&sentinel_)
        , head_(&sentinel_) {}

    /**
     * Enqueues a node without notifying any thread
     */
    template <typename... Args>
    requires acquire_invocable<node_policy, node_type, Args...>
    void enqueue(Args&&... args) noexcept(
        noexcept(enqueue_impl(node_policy::acquire(declval<Args>()...)))) {
        static constexpr bool notify = false;
        enqueue_impl<notify>(node_policy::acquire(forward<Args>(args)...));
    }

    /**
     * Dequeues an element off the head of the queue
     *
     * Does not block the calling thread
     *
     * As the enqueue operation is not atomic, if a dequeue of the last node in the queue races with
     * the enqueue operation, a safety mechanism will force a nullptr to be returned instead
     *
     * @return
     * A valid node_pointer will be returned if the queue is non-empty and there was no enqueue
     * race-condition with the final node; otherwise, nullptr will be returned
     */
    node_pointer dequeue() noexcept(
        is_nothrow_constructible_v<node_deleter, node_policy const&>&& noexcept(
            node_pointer(dequeue_impl(non_block), decl_deleter()))) {
        return node_pointer(dequeue_impl(non_block), static_cast<node_policy const&>(*this));
    }

    /**
     * Enqueues a node
     *
     * This operation will notify 1 thread waiting on the queue if the queue was previously empty
     */
    template <typename... Args>
    requires acquire_invocable<node_policy, node_type, Args...>
    void enqueue_notify_one(Args&&... args) noexcept(
        noexcept(enqueue_impl(node_policy::acquire(declval<Args>()...)))) {
        static constexpr bool notify = true;
        enqueue_impl<notify>(node_policy::acquire(forward<Args>(args)...));
    }

    /**
     * Dequeues an element off the head of the queue
     *
     * This function blocks the calling thread if the queue is empty until the timeout occurs,
     * thread is notified, or a new element is added to the queue.
     *
     * The returned node pointer uses a copy of the node policy implementation of the current queue
     * instance
     *
     * @return
     * If the calling thread was notified, a valid node_pointer will be returned if the queue is
     * non-empty and there was no enqueue race-condition with the final node; otherwise, nullptr
     * will be returned
     */
    template <typename R, typename P>
    node_pointer dequeue_wait_for(chrono::duration<R, P> const& timeout) noexcept(
        is_nothrow_constructible_v<node_deleter, node_policy const&>&& noexcept(
            node_pointer(dequeue_impl(decl_duration<R, P>()), decl_deleter()))) {
        return node_pointer(
            dequeue_impl(timeout), node_deleter(static_cast<node_policy const&>(*this)));
    }

    /**
     * Clears the queue and release ownership of all nodes
     *
     * Not thread safe, call this only on the consumer thread or after joining the consumer thread
     */
    void clear() noexcept(
        noexcept(node_policy::release((node_type*)0)) && noexcept(dequeue_impl())) {
        while (node_type* node = dequeue_impl()) {
            node_policy::release(node);
        }
    }

    using mpsc_queue_base::notify_all;
    using mpsc_queue_base::notify_one;

    ~mspc_queue() { clear(); }
};

} // namespace intrusive
UTL_NAMESPACE_END
#endif