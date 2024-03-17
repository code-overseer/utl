// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_same_as.h"
#include "utl/memory/utl_addressof.h"

#ifdef UTL_CXX14

UTL_NAMESPACE_BEGIN
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
    constexpr bool unlinked() const noexcept { return next_ == nullptr; }

protected:
    constexpr unidirectional_node() noexcept = default;

private:
    using this_type = unidirectional_node;
    using value_type = Derive;
    using pointer_type = value_type*;
    using reference_type = value_type&;
    using const_reference_type = value_type const&;
    using const_pointer_type = value_type const*;

    pointer_type next_ = enable_if_t<is_base_of<this_type, value_type>::value, decltype(nullptr)>{};

    friend constexpr void next(value_type& node, pointer_type value) noexcept {
        node.next_ = value;
    }
    friend constexpr pointer_type next(const_reference_type node) noexcept { return node->next_; }
    friend constexpr UTL_ATTRIBUTE(CONST) pointer_type& next_ref(
        reference_type node UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
        return node->next_;
    }
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
    constexpr bool linked() const noexcept { return next_ != nullptr || prev_ != nullptr; }
    constexpr bool unlinked() const noexcept { return next_ == nullptr && prev_ == nullptr; }

protected:
    constexpr bidirectional_node() noexcept = default;

private:
    using this_type = bidirectional_node;
    using value_type = Derive;
    using pointer_type = value_type*;
    using reference_type = value_type&;
    using const_reference_type = value_type const&;
    using const_pointer_type = value_type const*;

    pointer_type next_ = enable_if_t<std::is_base_of_v<this_type, Derive>, decltype(nullptr)>{};
    pointer_type prev_ = nullptr;

    friend constexpr pointer_type next(const_reference_type node) noexcept { return node->next_; }
    friend constexpr void next(value_type& node, pointer_type value) noexcept {
        node.next_ = value;
    }
    UTL_ATTRIBUTE(CONST)
    friend constexpr pointer_type& next_ref(
        reference_type node UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
        return node->next_;
    }

    friend constexpr pointer_type prev(const_reference_type node) noexcept { return node->prev_; }
    friend constexpr void prev(value_type& node, pointer_type value) noexcept {
        node.prev_ = value;
    }
    UTL_ATTRIBUTE(CONST)
    friend constexpr pointer_type& prev_ref(
        reference_type node UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
        return node->prev_;
    }
};

#  ifdef UTL_CXX20
namespace details {
/**
 * A concept that is satisfied iff `NodeType` has a public non-static data member with name `next`
 * of type `NodeType*`
 */
template <typename NodeType>
concept next_node_container = same_as<decltype(&NodeType::next), NodeType*(NodeType::*)>;

/**
 * A concept that is satisfied iff `NodeType` has a public non-static function member of signature
 * `NodeType* next() const`
 */
template <typename NodeType>
concept next_node_gettable = requires(NodeType const* ptr) {
    { ptr->next() } noexcept -> same_as<NodeType*>;
} && !requires { NodeType::next(); };

/**
 * A concept that is satisfied iff `NodeType` has a public non-static function member of signature
 * `NodeType* next(NodeType*)`
 */
template <typename NodeType>
concept next_node_settable = requires(NodeType& node, NodeType* value) {
    { node.next(value) } noexcept;
} && !requires(NodeType* value) { NodeType::next(value); };

/**
 * A concept that is satisfied iff `NodeType` either has a public non-static function member of
 * signature `NodeType*& next_ref()` or satisfies next_node_container
 */
template <typename NodeType>
concept next_node_referenceable = next_node_container<NodeType> ||
    (
        requires(NodeType* ptr) {
            { ptr->next_ref() } noexcept -> same_as<NodeType*&>;
        } && !requires { NodeType::next_ref(); });

/**
 * A concept that is satisfied iff `NodeType` has a public non-static data member with name `prev`
 * of type `NodeType*`
 */
template <typename NodeType>
concept prev_node_container = same_as<decltype(&NodeType::prev), NodeType*(NodeType::*)>;

/**
 * A concept that is satisfied iff `NodeType` has a public non-static function member of signature
 * `NodeType* prev() const`
 */
template <typename NodeType>
concept prev_node_gettable = requires(NodeType const* ptr) {
    { ptr->prev() } noexcept -> same_as<NodeType*>;
} && !requires { NodeType::prev(); };

/**
 * A concept that is satisfied iff `NodeType` has a public non-static function member of signature
 * `NodeType* prev(NodeType*)`
 */
template <typename NodeType>
concept prev_node_settable = requires(NodeType& node, NodeType* value) {
    { node.prev(value) } noexcept;
} && !requires(NodeType* value) { NodeType::prev(value); };

/**
 * A concept that is satisfied iff `NodeType` either has a public non-static function member of
 * signature `NodeType*& prev_ref()` or satisfies prev_node_container
 */
template <typename NodeType>
concept prev_node_referenceable = prev_node_container<NodeType> ||
    (
        requires(NodeType* ptr) {
            { ptr->prev_ref() } noexcept -> same_as<NodeType*&>;
        } && !requires { NodeType::prev_ref(); });
/**
 * Default proxy function to retrieve the `next` pointer of a node
 *
 * Will only participate in overload resolution if the `next` pointer is a non-static public data
 * member or a public function member with the signature, `NodeType* next() const`
 *
 * @param node - The node to retrieve the `next` pointer from
 *
 * @return value of the `next` pointer of `node`
 */
template <next_node_container NodeType>
constexpr NodeType* next(NodeType const& node) noexcept {
    return node.next;
}
template <next_node_gettable NodeType>
requires (!next_node_container<NodeType>)
constexpr NodeType* next(NodeType const& node) noexcept(noexcept(node.next())) {
    return node.next();
}

template <next_node_container NodeType>
UTL_ATTRIBUTE(CONST)
constexpr NodeType*& next_ref(NodeType& node UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return node.next;
}
template <next_node_referenceable NodeType>
requires (!next_node_container<NodeType*>)
UTL_ATTRIBUTE(CONST) constexpr NodeType*& next_ref(
    NodeType& node UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return node.next_ref();
}

/**
 * Default proxy function to set the `next` pointer of a node
 *
 * Will only participate in overload resolution if the `next` pointer is a non-static public data
 * member or a public function member with the signature, `R next(NodeType*)`, `R` could be any type
 * but usually void
 *
 * @param node - The node to set the `next` pointer of
 * @param value - `next` pointer argument to set
 */
template <next_node_container NodeType>
constexpr void next(NodeType& node, NodeType* value) noexcept {
    node.next = value;
}
template <next_node_settable NodeType>
requires (!next_node_container<NodeType>)
constexpr void next(NodeType& node, NodeType* value) noexcept(noexcept(node.next(value))) {
    node.next(value);
}

/**
 * Default proxy function to retrieve the `prev` pointer of a node
 *
 * Will only participate in overload resolution if the `prev` pointer is a non-static public data
 * member or a public function member with the signature, `NodeType* prev() const`
 *
 * @param Node - The node to retrieve the `prev` pointer from
 *
 * @return value of the `prev` pointer of `Node`
 */
template <prev_node_container NodeType>
constexpr NodeType* prev(NodeType const& node) noexcept {
    return node->prev;
}
template <prev_node_gettable NodeType>
requires (!prev_node_container<NodeType>)
constexpr NodeType* prev(NodeType const& node) noexcept(noexcept(node->prev())) {
    return node->prev();
}

/**
 * Default proxy function to set the `prev` pointer of a node
 *
 * Will only participate in overload resolution if the `prev` pointer is a non-static public data
 * member or a public function member with the signature, `R prev(NodeType*)`, `R` could be any type
 * but usually void
 *
 * @param node - The node to set the `prev` pointer of
 * @param value - `prev` pointer argument to set
 */
template <prev_node_container NodeType>
constexpr void prev(NodeType& node, NodeType* value) noexcept {
    node.prev = value;
}
template <prev_node_settable NodeType>
requires (!prev_node_container<NodeType>)
constexpr void prev(NodeType& node, NodeType* value) noexcept(noexcept(node.prev(value))) {
    node.prev(value);
}

template <prev_node_container NodeType>
UTL_ATTRIBUTE(CONST)
constexpr NodeType*& prev_ref(NodeType& node UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return node.prev;
}
template <prev_node_referenceable NodeType>
requires (!prev_node_container<NodeType*>)
UTL_ATTRIBUTE(CONST) constexpr NodeType*& prev_ref(
    NodeType& node UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return node.prev_ref();
}

/**
 * A concept that is satisfied if and only if `NodeType` meets the API requirements of a
 * unidirectional intrusive node
 */
template <typename NodeType>
concept unidirectional = requires(NodeType const& node) {
    { next(node) } -> same_as<NodeType*>;
} && requires(NodeType& node, NodeType* value) { next(node, value); } && requires(NodeType& node) {
    { next_ref(node) } -> same_as<NodeType*&>;
};

/**
 * Customization point object for getting the next node given a unidirectional node
 *
 * @note This is essentially a proxy function object that helps to invoke next using ADL without
 * letting user-defined functions bypass the argument constraint
 * @see https://timsong-cpp.github.io/cppwp/n4861/customization.point.object
 */
struct get_next_t {
    template <unidirectional NodeType>
    constexpr decltype(auto) operator()(NodeType const& node) const noexcept {
        return next(node);
    }
};

/**
 * Customization point object for setting the next node given a unidirectional node
 */
struct set_next_t {
    template <unidirectional NodeType>
    constexpr void operator()(NodeType& node, NodeType* value) const noexcept {
        next(node, value);
    }

    template <unidirectional NodeType>
    constexpr void operator()(NodeType& Node, decltype(nullptr) value) const noexcept {
        next(node, (NodeType*)value);
    }
};

struct get_next_ref_t {
    template <unidirectional NodeType>
    requires (!is_base_of_v<unidirectional_node<NodeType>, NodeType> &&
        !is_base_of_v<bidirectional_node<NodeType>, NodeType>)
    constexpr NodeType& operator()(
        NodeType const& node UTL_ATTRIBUTE(LIFETIMEBOUND)) const noexcept {
        return next_ref(node);
    }

    template <typename NodeType>
    requires (is_base_of_v<unidirectional_node<NodeType>, NodeType> ||
        is_base_of_v<bidirectional_node<NodeType>, NodeType>)
    UTL_ATTRIBUTE(CONST) constexpr NodeType& operator()(
        NodeType const& node UTL_ATTRIBUTE(LIFETIMEBOUND)) const noexcept {
        return next_ref(node);
    }
};

/**
 * A concept that is satisfied if and only if `NodeType` meets the API requirements of a
 * bidirectional intrusive node
 */
template <typename NodeType>
concept bidirectional = unidirectional<NodeType> && requires(NodeType const& node) {
    { prev(node) } -> same_as<NodeType*>;
} && requires(NodeType& node, NodeType* value) {
    prev(node, value);
} && requires(NodeType const& node) {
    { prev_ref(node) } -> same_as<NodeType*&>;
};

/**
 * Customization point object for getting the prev node given a bidirectional node
 */
struct get_prev_t {
    template <bidirectional NodeType>
    constexpr decltype(auto) operator()(NodeType const& node) const noexcept {
        return prev(node);
    }
};

/**
 * Customization point object for setting the prev node given a bidirectional node
 */
struct set_prev_t {
    template <bidirectional NodeType>
    constexpr void operator()(NodeType& node, NodeType* value) const noexcept {
        prev(node, value);
    }

    template <bidirectional NodeType>
    constexpr void operator()(NodeType& node, decltype(nullptr) value) const noexcept {
        prev(node, (NodeType*)value);
    }
};

struct get_prev_ref_t {
    template <bidirectional NodeType>
    requires (!is_base_of_v<bidirectional_node<NodeType>, NodeType>)
    constexpr decltype(auto) operator()(
        NodeType const& node UTL_ATTRIBUTE(LIFETIMEBOUND)) const noexcept {
        return prev_ref(node);
    }

    template <typename NodeType>
    requires is_base_of_v<bidirectional_node<NodeType>, NodeType>
    UTL_ATTRIBUTE(CONST) constexpr NodeType& operator()(
        NodeType const& node UTL_ATTRIBUTE(LIFETIMEBOUND)) const noexcept {
        return prev_ref(node);
    }
};
} // namespace details

UTL_INLINE_CXX17 constexpr details::get_next_t get_next = {};
UTL_INLINE_CXX17 constexpr details::set_next_t set_next = {};
UTL_INLINE_CXX17 constexpr details::get_prev_t get_prev = {};
UTL_INLINE_CXX17 constexpr details::set_prev_t set_prev = {};
UTL_INLINE_CXX17 constexpr details::get_next_ref_t get_next_ref = {};
UTL_INLINE_CXX17 constexpr details::get_prev_ref_t get_prev_ref = {};

namespace details {
struct linked_t {
    template <bidirectional NodeType>
    constexpr bool operator()(NodeType const& node) const noexcept {
        return get_next(node) != nullptr || get_prev(node) != nullptr;
    }

    template <unidirectional NodeType>
    requires (!bidirectional<NodeType>)
    constexpr bool operator()(NodeType const& node) const noexcept {
        return get_next(node) != nullptr;
    }
};

struct unlinked_t {
    template <bidirectional NodeType>
    constexpr bool operator()(NodeType const& node) const noexcept {
        return get_next(node) == nullptr && get_prev(node) == nullptr;
    }

    template <unidirectional NodeType>
    requires (!bidirectional<NodeType>)
    constexpr bool operator()(NodeType const& node) const noexcept {
        return get_next(node) == nullptr;
    }
};
} // namespace details
UTL_INLINE_CXX17 constexpr details::linked_t linked = {};
UTL_INLINE_CXX17 constexpr details::unlinked_t unlinked = {};

using details::bidirectional;
using details::unidirectional;

template <typename Policy, typename NodeType>
concept release_invocable = requires(Impl* policy, NodeType* ptr) {
    { policy->release(ptr) };
};

template <typename Policy, typename NodeType>
concept clone_invocable = requires(Impl* policy, NodeType const& node) {
    { policy->clone(node) } -> convertible_to<NodeType*>;
};

template <typename Impl, typename NodeType, typename... Args>
concept acquire_invocable = requires(Impl* policy, Args&&... args) {
    { policy->acquire(foward<Args>(args)...) } -> convertible_to<NodeType*>;
};

namespace details {
template <typename Policy, typename = void>
struct propagate_on_container_copy_assignment : false_type {};
template <typename Policy>
struct propagate_on_container_copy_assignment<Policy,
    void_t<typename Policy::propagate_on_container_copy_assignment>> :
    Policy::propagate_on_container_copy_assignment {};

template <typename Policy, typename = void>
struct propagate_on_container_move_assignment : false_type {};
template <typename Policy>
struct propagate_on_container_move_assignment<Policy,
    void_t<typename Policy::propagate_on_container_move_assignment>> :
    Policy::propagate_on_container_move_assignment {};

template <typename Policy, typename = void>
struct propagate_on_container_swap : false_type {};
template <typename Policy>
struct propagate_on_container_swap<Policy, void_t<typename Policy::propagate_on_container_swap>> :
    Policy::propagate_on_container_swap {};

template <typename Policy, typename = void>
struct is_always_equal : is_empty<Policy> {};
template <typename Policy>
struct is_always_equal<Policy, void_t<typename Policy::is_always_equal>> :
    Policy::is_always_equal {};

template <typename Policy>
concept selectable_copy_construction = requires(Policy const& p) {
    { p.select_on_container_copy_construction() } -> same_as<Policy>;
};
} // namespace details

template <typename Policy>
struct node_policy_traits {
    using node_type = typename Policy::node_type;
    using policy_type = Policy;
    using fancy_pointer =
        UTL_SCOPE node_ptr<NodeType, default_node_deleter<node_type, policy_type>>;
    using node_deleter = typename fancy_pointer::deleter_type;
    using reference = typename base_type::reference;
    using const_reference = typename base_type::const_reference;
    using pointer = typename base_type::pointer;
    using const_pointer = typename base_type::const_pointer;
    using size_type = size_t;
    using difference_type = decltype((pointer)0 - (pointer)0);
    using propagate_on_container_copy_assignment =
        details::propagate_on_container_copy_assignment<policy_type>;
    using propagate_on_container_move_assignment =
        details::propagate_on_container_move_assignment<policy_type>;
    using propagate_on_container_swap = details::propagate_on_container_swap<policy_type>;
    using is_always_equal = details::is_always_equal<policy_type>;
    using is_node_clonable = clone_invocable<policy_type, node_type>;

    static_assert(UTL_SCOPE is_nothrow_copy_constructible<policy_type>::value,
        "Policy must be nothrow copy constructible");
    static_assert(UTL_SCOPE is_nothrow_move_constructible<policy_type>::value,
        "Policy must be nothrow move constructible");
    static_assert(
        is_always_equal::value || UTL_SCOPE is_nothrow_equality_comparable<policy_type>::value,
        "Policy must be nothrow equality comparable if it is not always equal");
    static_assert(!propagate_on_container_copy_assignment::value ||
            UTL_SCOPE is_nothrow_copy_assignable<policy_type>::value,
        "If propogation on copy assignment is required, policy must be nothrow copy assignable");
    static_assert(!propagate_on_container_move_assignment::value ||
            UTL_SCOPE is_nothrow_move_assignable<policy_type>::value,
        "If propogation on move assignment is required, policy must be nothrow move assignable");
    static_assert(
        !propagate_on_container_swap::value || UTL_SCOPE is_nothrow_swappable<policy_type>::value,
        "If propogation on swap is required, policy must be nothrow swappable");

    template <typename... Args>
    requires acquire_invocable<policy_type, node_type, Args...>
    static pointer acquire(policy_type& p, Args&&... args) noexcept(
        noexcept(declval<policy_type>().acquire(declval<Args>()...))) {
        return p.acquire(forward<Args>(args)...);
    }

    template <same_as<node_type> T>
    requires clone_invocable<policy_type, node_type>
    static pointer clone(policy_type& p, T const& other) {
        // clone will likely involve an allocation so make it not noexcept
        return p.clone(other);
    }

    template <same_as<node_type> T>
    requires (!clone_invocable<policy_type, node_type>)
    static [[noreturn]] pointer clone(policy_type& p, T const& other) {
        UTL_ASSERT(false);
        UTL_BUILTIN_unreachable();
    }

    static void release(policy_type& p, pointer ptr) noexcept {
        static_assert(noexcept(p.release(ptr)), "policy release must be noexcept");
        p.release(ptr);
    }

    template <typename T = policy_type>
    static UTL_CONSTEXPR_CXX14
        enable_if_t<is_same<T, policy_type>::value && propagate_on_container_copy_assignment::value,
            void>
        assign(T& dst, policy_type const& src) noexcept {
        dst = src;
        // policy should be equal even after copy assignment
        UTL_ASSERT(policy_traits::equals(dsr, src));
    }

    template <typename T = policy_type>
    static UTL_CONSTEXPR_CXX14 enable_if_t<
        is_same<T, policy_type>::value && !propagate_on_container_copy_assignment::value, void>
    assign(T& dst, policy_type const& src) noexcept {}

    template <typename T = policy_type>
    static UTL_CONSTEXPR_CXX14
        enable_if_t<is_same<T, policy_type>::value && propagate_on_container_move_assignment::value,
            void>
        assign(T& dst, policy_type&& src) noexcept {
        dst = move(src);
        // policy should be equal even after move assignment
        UTL_ASSERT(policy_traits::equals(dsr, src));
    }

    template <typename T = policy_type>
    static UTL_CONSTEXPR_CXX14 enable_if_t<
        is_same<T, policy_type>::value && !propagate_on_container_move_assignment::value, void>
    assign(T& dst, policy_type&& src) noexcept {}

    template <typename T = policy_type>
    static UTL_CONSTEXPR_CXX14 enable_if_t<is_same<T, policy_type>::value &&
            details::selectable_copy_construction<policy_type>,
        policy_type>
    select_on_container_copy_construction(T const& p) noexcept(
        noexcept(declval<policy_type>().select_on_container_copy_construction())) {
        return p.select_on_container_copy_construction();
    }

    template <typename T = policy_type>
    static UTL_CONSTEXPR_CXX14 enable_if_t<is_same<T, policy_type>::value &&
            !details::selectable_copy_construction<policy_type>,
        policy_type>
    select_on_container_copy_construction(T const& p) noexcept {
        return p;
    }

    template <typename T = policy_type>
    static constexpr enable_if_t<is_same<T, policy_type>::value && is_always_equal::value, bool>
    equals(T const&, T const&) noexcept {
        return true;
    }

    template <typename T = policy_type>
    static constexpr enable_if_t<is_same<T, policy_type>::value && !is_always_equal::value, bool>
    equals(T const& left, T const& right) noexcept {
        return left == right;
    }
};

#  endif

} // namespace intrusive

UTL_NAMESPACE_END
#endif
