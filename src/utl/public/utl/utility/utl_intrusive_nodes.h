// Original Copyright (c) 2010-2011 Dmitry Vyukov. All rights reserved.
// Modifications and redesign Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/memory/utl_addressof.h"
#include "utl/thread/utl_hardware_interference.h"
#include "utl/utility/utl_intrusive_nodes.h"

#include <atomic>

#ifdef UTL_CXX14
namespace utl {

using std::atomic;
using std::atomic_ref;

namespace intrusive {

/**
 * Simple unidirectional node type for an intrusive singly-linked list
 *
 * @note This is meant to be inherited
 *
 * @tparam Derive - derived type that will be a node for a singly-linked list
 */
template <typename Derive>
class unidirectional_node {
public:
    constexpr bool linked() const noexcept { return next_ != nullptr; }

protected:
    constexpr unidirectional_node() noexcept = default;

private:
    using this_type = unidirectional_node;
    using value_type = Derive;
    using pointer_type = value_type*;
    using reference_type = value_type&;
    using const_reference_type = const value_type&;
    using const_pointer_type = value_type const*;
    using atomic_pointer = atomic_ref<pointer_type>;

    alignas(atomic_pointer::required_alignment)
    pointer_type next_ = enable_if_t<std::is_base_of_v<this_type, Derive>, decltype(nullptr)>{};

    friend constexpr pointer_type next(const_reference_type node) noexcept { return node->next_; }
    friend constexpr atomic_pointer atomic_next(reference_type node) noexcept { return atomic_pointer{ node.next_ }; }
    friend constexpr void next(value_type& node, pointer_type value) noexcept { node.next_ = value; }
    friend constexpr pointer_type& next_ref(reference_type node) noexcept { return node->next_; }
};

/**
 * Simple bidirectional node type for an intrusive singly-linked list
 *
 * @note This is meant to be inherited
 *
 * @tparam Derive - derived type that will be a node for a singly-linked list
 */
template <typename Derive>
class bidirectional_node {
public:
    constexpr bool linked() const noexcept { return next_ != nullptr && prev_ != nullptr; }

protected:
    constexpr bidirectional_node() noexcept = default;

private:
    using this_type = unidirectional_node;
    using value_type = Derive;
    using pointer_type = value_type*;
    using reference_type = value_type&;
    using const_reference_type = const value_type&;
    using const_pointer_type = value_type const*;
    using atomic_pointer = atomic_ref<pointer_type>;

    alignas(atomic_pointer::required_alignment)
    pointer_type next_ = enable_if_t<std::is_base_of_v<this_type, Derive>, decltype(nullptr)>{};
    alignas(atomic_pointer::required_alignment)
    pointer_type prev_ = nullptr;

    friend constexpr pointer_type next(const_reference_type node) noexcept { return node->next_; }
    friend constexpr atomic_pointer atomic_next(reference_type node) noexcept { return atomic_pointer{ node.next_ }; }
    friend constexpr void next(value_type& node, pointer_type value) noexcept { node.next_ = value; }
    friend constexpr pointer_type& next_ref(reference_type node) noexcept { return node->next_; }

    friend constexpr pointer_type prev(const_reference_type node) noexcept { return node->prev_; }
    friend constexpr atomic_pointer atomic_prev(reference_type node) noexcept { return atomic_pointer{ node.prev_ }; }
    friend constexpr void prev(value_type& node, pointer_type value) noexcept { node.prev_ = value; }
    friend constexpr pointer_type& prev_ref(reference_type node) noexcept { return node->next_; }
};

#ifdef UTL_CXX20
/**
 * A concept that is satisfied iff `NodeType` has a public non-static data member with name `next` of type `NodeType*`
 */
template <typename NodeType>
concept next_node_container = same_as<decltype(&NodeType::next), NodeType*(NodeType::*)>;

/**
 * A concept that is satisfied iff `NodeType` has a public non-static data member with name `next` of type `atomic<NodeType*>`
 */
template <typename NodeType>
concept atomic_next_node_container = same_as<decltype(&NodeType::next), atomic<NodeType*>(NodeType::*)>;

/**
 * A concept that is satisfied iff `NodeType` has a public non-static function member of signature `NodeType* next() const`
 */
template <typename NodeType>
concept next_node_gettable = requires(const NodeType* ptr) {
    { ptr->next() } -> same_as<NodeType*>;
} && !requires { NodeType::next(); };

/**
 * A concept that is satisfied iff `NodeType` has a public non-static function member of signature `NodeType* next(NodeType*)`
 */
template <typename NodeType>
concept next_node_settable = requires(NodeType& node, NodeType* value) {
    node.next(value);
} && !requires(NodeType* value) { NodeType::next(value); };

/**
 * A concept that is satisfied iff `NodeType` either has a public non-static function member of signature `NodeType*& next_ref()`
 * or satisfies next_node_container
 */
template <typename NodeType>
concept next_node_referenceable = next_node_container<NodeType> || 
(requires(NodeType* ptr) {
    { ptr->next_ref() } -> same_as<NodeType*&>;
} && !requires { NodeType::next_ref(); });

template <typename NodeType>
concept atomic_next_accessible =
    next_node_container<NodeType> || 
    atomic_next_node_container<NodeType> || ((
        requires(NodeType* node) {
            { node->atomic_next() } -> same_as<atomic<NodeType*>&>;
        } || requires(NodeType* node) {
            { node->atomic_next() } -> same_as<atomic_ref<NodeType*>>;
        }) && !requires { NodeType::atomic_next(); }
    );

/**
 * A concept that is satisfied iff `NodeType` has a public non-static data member with name `prev` of type `NodeType*`
 */
template <typename NodeType>
concept prev_node_container = same_as<decltype(&NodeType::prev), NodeType*(NodeType::*)>;

/**
 * A concept that is satisfied iff `NodeType` has a public non-static data member with name `prev` of type `atomic<NodeType*>`
 */
template <typename NodeType>
concept atomic_prev_node_container = same_as<decltype(&NodeType::prev), atomic<NodeType*>(NodeType::*)>;

/**
 * A concept that is satisfied iff `NodeType` has a public non-static function member of signature `NodeType* prev() const`
 */
template <typename NodeType>
concept prev_node_gettable = requires(const NodeType* ptr) {
    { ptr->prev() } -> same_as<NodeType*>;
} && !requires { NodeType::prev(); };

/**
 * A concept that is satisfied iff `NodeType` has a public non-static function member of signature `NodeType* prev(NodeType*)`
 */
template <typename NodeType>
concept prev_node_settable = requires(NodeType& node, NodeType* value) {
    node.prev(value);
} && !requires(NodeType* value) { NodeType::prev(value); };

/**
 * A concept that is satisfied iff `NodeType` either has a public non-static function member of signature `NodeType*& prev_ref()`
 * or satisfies prev_node_container
 */
template <typename NodeType>
concept prev_node_referenceable = prev_node_container<NodeType> || 
(requires(NodeType* ptr) {
    { ptr->prev_ref() } -> same_as<NodeType*&>;
} && !requires { NodeType::prev_ref(); });


template <typename NodeType>
concept atomic_prev_accessible =
    prev_node_container<NodeType> ||
    atomic_prev_node_container<NodeType> || ((
        requires(NodeType* node) {
            { node->atomic_prev() } -> same_as<atomic<NodeType*>&>;
        } || requires(NodeType* node) {
            { node->atomic_prev() } -> same_as<atomic_ref<NodeType*>>;
        }) && !requires { NodeType::atomic_prev(); }
    );

namespace details {
/**
 * Default proxy function to retrieve the `next` pointer of a node
 *
 * Will only participate in overload resolution if the `next` pointer is a non-static public data member or a
 * public function member with the signature, `NodeType* next() const`
 *
 * @param node - The node to retrieve the `next` pointer from
 *
 * @return value of the `next` pointer of `node`
 */
template <next_node_container NodeType>
constexpr NodeType* next(const NodeType& node) noexcept { return node.next; }
template <next_node_gettable NodeType> requires (!next_node_container<NodeType>)
constexpr NodeType* next(const NodeType& node) noexcept(noexcept(node.next())) { return node.next(); }

template <next_node_container NodeType>
constexpr atomic_ref<NodeType*> atomic_next(NodeType& node) noexcept { return atomic_ref<NodeType*>{ node.next }; }
template <atomic_next_node_container NodeType> requires (!next_node_container<NodeType>)
constexpr atomic<NodeType*>& atomic_next(NodeType& node) noexcept { return node.next; }
template <atomic_next_accessible NodeType> requires (!next_node_container<NodeType> && !atomic_next_node_container<NodeType>)
constexpr decltype(declval<NodeType>().atomic_next())
atomic_next(NodeType& node) noexcept(noexcept(node.atomic_next())) { return node.atomic_next(); }
template <next_node_referenceable NodeType> requires(!atomic_next_accessible<NodeType> && !next_node_container<NodeType>)
constexpr atomic_ref<NodeType*> atomic_next(NodeType& node) noexcept { return atomic_ref<NodeType*>{ node.next_ref() }; }

template <next_node_container NodeType>
constexpr NodeType*& next_ref(NodeType& node) noexcept { return node.next; }
template <next_node_referenceable NodeType> requires (!next_node_container<NodeType*>)
constexpr NodeType*& next_ref(NodeType& node) noexcept { return node.next_ref(); }

/**
 * Default proxy function to set the `next` pointer of a node
 *
 * Will only participate in overload resolution if the `next` pointer is a non-static public data member or a 
 * public function member with the signature, `R next(NodeType*)`, `R` could be any type but usually void
 *
 * @param node - The node to set the `next` pointer of
 * @param value - `next` pointer argument to set
 */
template <next_node_container NodeType>
constexpr void next(NodeType& node, NodeType* value) noexcept { node.next = value; }
template <next_node_settable NodeType> requires (!next_node_container<NodeType>)
constexpr void next(NodeType& node, NodeType* value) noexcept(noexcept(node.next(value))) { node.next(value); }

/**
 * Default proxy function to retrieve the `prev` pointer of a node
 *
 * Will only participate in overload resolution if the `prev` pointer is a non-static public data member or a public function member with the signature, `NodeType* prev() const`
 *
 * @param Node - The node to retrieve the `prev` pointer from
 *
 * @return value of the `prev` pointer of `Node`
 */
template <prev_node_container NodeType>
constexpr NodeType* prev(const NodeType& node) noexcept { return node->prev; }
template <prev_node_gettable NodeType> requires (!prev_node_container<NodeType>)
constexpr NodeType* prev(const NodeType& node) noexcept(noexcept(node->prev())) { return node->prev(); }

/**
 * Default proxy function to set the `prev` pointer of a node
 *
 * Will only participate in overload resolution if the `prev` pointer is a non-static public data member or a public function member with the signature, `R prev(NodeType*)`, `R` could be
 * any type but usually void
 *
 * @param node - The node to set the `prev` pointer of
 * @param value - `prev` pointer argument to set
 */
template <prev_node_container NodeType>
constexpr void prev(NodeType& node, NodeType* value) noexcept { node.prev = value; }
template <prev_node_settable NodeType> requires (!prev_node_container<NodeType>)
constexpr void prev(NodeType& node, NodeType* value) noexcept(noexcept(node.prev(value))) { node.prev(value); }

template <prev_node_container NodeType>
constexpr atomic_ref<NodeType*> atomic_prev(NodeType& node) noexcept { return atomic_ref<NodeType*>{ node.prev }; }
template <atomic_prev_node_container NodeType> requires (!prev_node_container<NodeType>)
constexpr atomic<NodeType*>& atomic_prev(NodeType& node) noexcept { return node.prev; }
template <atomic_prev_accessible NodeType> requires (!prev_node_container<NodeType> && !atomic_prev_node_container<NodeType>)
constexpr decltype(declval<NodeType>().atomic_prev())
atomic_prev(NodeType& node) noexcept(noexcept(node.atomic_prev())) { return node.atomic_prev(); }
template <prev_node_referenceable NodeType> requires(!atomic_prev_accessible<NodeType> && !prev_node_container<NodeType>)
constexpr atomic_ref<NodeType*> atomic_prev(NodeType& node) noexcept { return atomic_ref<NodeType*>{ node.prev_ref() }; }

template <prev_node_container NodeType>
constexpr NodeType*& prev_ref(NodeType& node) noexcept { return node.prev; }
template <prev_node_referenceable NodeType> requires (!prev_node_container<NodeType*>)
constexpr NodeType*& prev_ref(NodeType& node) noexcept { return node.prev_ref(); }

/**
 * A concept that is satisfied if and only if `NodeType` meets the API requirements of a unidirectional intrusive node
 */
template<typename NodeType>
concept unidirectional =
    requires(const NodeType& node) { {next(node)} -> same_as<NodeType*>; } &&
    requires(NodeType& node, NodeType* value) { next(node, value); };

template<typename NodeType>
concept atomic_unidirectional = 
    (unidirectional<NodeType> && requires(NodeType& node) { {next_ref(node)} -> same_as<NodeType*&>; }) ||
    requires(NodeType& node) { {atomic_next(node)} -> same_as<atomic<NodeType*>>; } ||
    requires(NodeType& node) { {atomic_next(node)} -> same_as<atomic_ref<NodeType*>>; };

/**
 * Customization point object for getting the next node given a unidirectional node
 *
 * @note This is essentially a proxy function object that helps to invoke next using ADL without letting user-defined functions
 * bypass the argument constraint
 * @see https://timsong-cpp.github.io/cppwp/n4861/customization.point.object
 */
struct get_next_t {
    template<unidirectional NodeType>
    constexpr decltype(auto) operator()(const NodeType& node) const noexcept(noexcept(next(node))) {
        return next(node);
    }
};

/**
 * Customization point object for setting the next node given a unidirectional node
 */
struct set_next_t {
    template<unidirectional NodeType>
    constexpr decltype(auto) operator()(NodeType& node, NodeType* value) const noexcept(noexcept(next(node, value))) {
        return next(node, value);
    }

    template<unidirectional NodeType>
    constexpr decltype(auto) operator()(NodeType& Node, decltype(nullptr) value) const noexcept(noexcept(next(node, node))) {
        return next(node, (NodeType*)value);
    }
};

struct get_next_ref_t {
    template<unidirectional NodeType>
    constexpr decltype(auto) operator()(const NodeType& node) const noexcept(noexcept(next_ref(node))) {
        return next_ref(node);
    }
};

/**
 * A concept that is satisfied if and only if `NodeType` meets the API requirements of a bidirectional intrusive node
 */
template<typename NodeType>
concept bidirectional = unidirectional<NodeType> &&
    requires(const NodeType& node) { { prev(node) } -> same_as<NodeType*>; } &&
    requires(NodeType& node, NodeType* value) { prev(node, value); };


/**
 * Customization point object for getting the prev node given a bidirectional node
 */
struct get_prev_t {
    template<bidirectional NodeType>
    constexpr decltype(auto) operator()(const NodeType& node) const noexcept(noexcept(prev(node))) {
        return prev(node);
    }
};

/**
 * Customization point object for setting the prev node given a bidirectional node
 */
struct set_prev_t {
    template<bidirectional NodeType>
    constexpr decltype(auto) operator()(NodeType& node, NodeType* value) const noexcept(noexcept(prev(node, value))) {
        return prev(node, value);
    }

    template<bidirectional NodeType>
    constexpr decltype(auto) operator()(NodeType& node, decltype(nullptr) value) const noexcept(noexcept(prev(node, addressof(node)))) {
        return prev(node, (NodeType*)value);
    }
};

struct get_prev_ref_t {
    template<unidirectional NodeType>
    constexpr decltype(auto) operator()(const NodeType& node) const noexcept(noexcept(prev_ref(node))) {
        return prev_ref(node);
    }
};


}    // namespace details

UTL_INLINE_CXX17 constexpr details::get_next_t get_next = {};
UTL_INLINE_CXX17 constexpr details::set_next_t set_next = {};
UTL_INLINE_CXX17 constexpr details::get_prev_t get_prev = {};
UTL_INLINE_CXX17 constexpr details::set_prev_t set_prev = {};
UTL_INLINE_CXX17 constexpr details::get_next_ref_t get_next_ref = {};
UTL_INLINE_CXX17 constexpr details::get_prev_ref_t get_prev_ref = {};

using details::unidirectional;
using details::bidirectional;

namespace details
{
template<typename Impl, typename NodeType>
concept release_invocable_impl = requires(Impl* policy, NodeType* ptr) {
    { policy->release_node(ptr) };
};

template<typename Impl, typename NodeType, typename... Args>
concept acquire_invocable_impl = requires(Impl* policy, NodeType* ptr, Args&&... arguments) {
    { policy->acquire_node(forward<Args>(arguments)...) } -> std::convertible_to<NodeType*>;
};
}

template<typename Policy, typename NodeType>
concept release_invocable = []() {
    struct Impl : private Policy {
        // function can be protected
        using Policy::release_node;
    };

    return details::release_invocable_impl<Impl, NodeType>;
}();

template<typename Policy, typename NodeType, typename... Args>
concept acquire_invocable = []() {
    struct Impl : private Policy {
        // function can be protected
        using Policy::acqurie_node;
    };

    return details::acquire_invocable_impl<Impl, NodeType, Args...>;
}();

#endif



}   // namespace intrusive


}   // namespace utl
#endif