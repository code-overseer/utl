// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/intrusive/utl_nodes.h"
#include "utl/memory/utl_unique_ptr.h"
#include "utl/type_traits/utl_is_constructible.h"

#ifdef UTL_CXX20
UTL_NAMESPACE_BEGIN
namespace intrusive {

/**
 * Deleter policy to be used with unique_ptr
 *
 * @tparam Node - the node type
 * @tparam Policy - Policy type that will be used to destroy/release a node
 *
 * @note Partially specialized to allow forward declarations
 */
template <typename Node, typename Policy>
class default_node_deleter;

template <unidirectional Node, release_invocable<NodeType> Policy>
class default_node_deleter<Node, Policy> {
public:
    constexpr default_node_deleter() noexcept(is_nothrow_default_constructible_v<Policy>) = default;
    template <typename... Args>
    requires is_construtible_v<Policy, Args...>
    constexpr default_node_deleter(Args&&... args) noexcept(
        is_nothrow_constructible_v<Policy, Args...>)
        : policy(forward<Args>(args)...) {}

    constexpr void operator()(Node* ptr) const noexcept { policy.release(ptr); }

    UTL_ATTRIBUTE(NODISCARD)
    UTL_CONSTEXPR_CXX14 policy_type& get_policy() noexcept { return policy; }

    UTL_ATTRIBUTE(NODISCARD)
    constexpr policy_type const& get_policy() const noexcept { return policy; }

private:
    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) Policy policy;
};

/**
 * unique_ptr alias for intrusive nodes
 */
template <typename Node, typename NodePolicy>
using node_ptr = unique_ptr<Node, default_node_deleter<Node, NodePolicy>>;
} // namespace intrusive
UTL_NAMESPACE_END
#endif