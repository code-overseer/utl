// Original Copyright (c) 2010-2011 Dmitry Vyukov. All rights reserved.
// Modifications and redesign Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_declval.h"
#include "utl/intrusive/utl_nodes.h"
#include "utl/intrusive/utl_node_ptr.h"
#include "utl/intrusive/utl_forward_list.h"
#include "utl/thread/utl_atomic_impl.h"
#include "utl/utility/utl_exchange.h"
#include "utl/utility/utl_forward.h"

#ifdef UTL_CXX20
UTL_NAMESPACE_BEGIN

namespace intrusive {

/**
 * Lock-free thread-safe stack that could have multiple threads adding to it
 *
 * The implementation does no provide an API for popping a node off the list, the consumer thread must
 * consume the whole list, which constructs a forward_list from which individual nodes can be popped.
 *
 * @tparam node_type - Node type that the linked list will use, node_type should expose the `next_node` pointer via one of the following:
 * * Declare a public non-static `node_type*` data member named `next_node`
 * * Declare public non-static function members with the following signatures `node_type* Getnext_node() const` & `node_type* Setnext_node(node_type*, node_type*)`
 * * Otherwise, the expressions `Getnext_node(node)` & `Setnext_node(node, value_to_set)` must be valid through argument-dependent-lookup, where
 *   `node` and `value_to_set` are `node_type*`
 * @see https://en.cppreference.com/w/cpp/language/adl
 *
 * @tparam node_policy - Node operation policy; policies should implement the following API:
 *  **
 *  * Retrieves/creates a pointer to a node from any set of arguments that will be passed from `multi_producer_stack::PushFront`
 *  *
 * node_type* MakeNode(Args...);
 *
 *  **
 *  * Releases ownership of the given node
 *  *
 * void release(node_type*);
 */
template <unidirectional Node, release_invocable<node_type> Policy>
class multi_producer_stack : private Policy
{
public:
    using node_type = Node;
    using node_policy = Policy;
    using node_pointer = node_ptr<node_type, node_policy>;
    using node_deleter = typename node_pointer::deleter_type;
    using consume_list = forward_list<node_type, node_policy>;

    multi_producer_stack(const multi_producer_stack&) = delete;
    multi_producer_stack& operator=(const multi_producer_stack&) = delete;

    /**
     * Constructor
     *
     * @param Args... - The policy class constructor arugments
     */
    template <typename... Args>
    requires is_constructible_v<node_policy, Args...>
    multi_producer_stack(Args&&... args)
        : node_policy(forward<Args>(args)...), head_(nullptr) {
    }

    /**
     * Creates/retrieves a node from the provided argument(s) to be added to the head of the linked list
     *
     * @param Args... - Arguments to retrieve/create the node pointer from
     */
    template <typename... Args>
    requires acquire_invocable<node_policy, node_type, Args...>
    void push_front(Args&&... Args) noexcept(
        noexcept(push_front_impl(node_policy::acquire(declval<Args>()...)))
    ) {
        push_front_impl(node_policy::acquire(forward<Args>(args)...));
    }

    node_pointer pop_front() {
        node_type* output = head_.load(std::memory_order_relaxed);
        if (output == nullptr) {
            return node_pointer(nullptr, static_cast<const node_policy&>(*this));
        }

        UTL_ATOMIC_THREAD_FENCE(std::memory_order_acquire);
        node_type* new_head = nullptr;
        do {
            new_head = next(output);
        } while (!head_.compare_exchange_weak(output, new_head, std::memory_order_acquire, std::memory_order_release));
        
        return node_pointer(new_head, static_cast<const node_policy&>(*this));
    }

    template <typename... Args>
    requires std::is_constructible_v<node_policy, Args...>
    node_pointer pop_front(Args&&... args) {
        node_type* output = head_.load(std::memory_order_relaxed);
        if (output == nullptr) {
            return node_pointer(nullptr, forward<Args>(args)...);
        }

        std::atomic_thread_fence(std::memory_order_acquire);
        node_type* new_head = nullptr;
        do {
            new_head = next(output);
        } while (!head_.compare_exchange_weak(output, new_head, std::memory_order_acquire, std::memory_order_release));
        
        return node_pointer(new_head, forward<Args>(args)...);
    }

    /**
     * Creates a consumer list which atomically takes ownership of the whole list and clears the current stack instance
     *
     * Callable only if the policy class is copy constructible
     *
     * @return A consumer list with all the node ownership transferred to it
     */
    [[nodiscard]] consume_list consume() {
        return consume_list(head_, static_cast<const node_policy&>(*this));
    }

    /**
     * Creates a consumer list which atomically takes ownership of the whole list and clears the current stack instance
     *
     * @param Args... - constructor arguments for the `node_policy` type
     *
     * @return A consumer list with all the node ownership transferred to it
     */
    template <typename... Args>
    requires std::is_constructible_v<node_policy, Args...>
    [[nodiscard]] consume_list consume(Args&&... args) {
        node_type* head = UTL_ATOMIC_EXCHANGE(addressof(head_), nullptr, memory_order_acquire);
        return consume_list(head, forward<Args>(args)...);
    }

    /**
     * Atomically clears the list, and subsequently (non-atomically) releasing ownership of every node
     */
    void clear() noexcept(
        noexcept(next((node_type*)0), next((node_type*)0, (node_type*)0), node_policy::release((node_type*)0))
    ) {
        node_type* current_node = UTL_ATOMIC_EXCHANGE(addressof(head_), nullptr, memory_order_acquire);
        while (current_node != nullptr)
        {
            node_type* next_node = get_next(current_node);
            set_next(current_node, nullptr);
            node_policy::release(current_node);
            current_node = next_node;
        }
    }

    /**
     * Checks if the list is empty
     *
     * @returns true if the list if empty, false otherwise
     */
    [[nodiscard]] bool empty() const noexcept { return UTL_ATOMIC_LOAD(addressof(head_), memory_order_relaxed) == nullptr; }

    ~multi_producer_stack() { clear(); }

private:
    void push_front_impl(node_type* node) {
        UTL_ASSERT(node != nullptr);
        // This must be `memory_order_acquire` because when the consumer thread reads the whole list
        // The memory operations on every node must be visible; if this was relaxed, only the memory operation on the head node may be
        // visible on the consumer thread, which may result in invalid data access when the consumer thread accesses nodes below `head_`
        node_type* current_head = UTL_ATOMIC_LOAD(addressof(head_), memory_order_acquire);
        do
        {
            set_next(node, current_head);
        } while (!UTL_ATOMIC_CAS(addressof(head_), node, memory_order_release, memory_order_acquire));
    }

    node_type* head_;
};
}   // namespace intrusive
UTL_NAMESPACE_END
#endif