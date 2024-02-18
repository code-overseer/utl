// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/memory/utl_intrusive_node_ptr.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/intrusive/utl_nodes.h"
#include "utl/utility/utl_exchange.h"
#include "utl/utility/utl_forward.h"


#include <atomic>

#ifdef UTL_CXX20
namespace utl {
namespace intrusive {
template <bidirectional NodeType, release_invocable<NodeType> Policy>
class list : private Policy {
public:
    using node_type = NodeType;
    using node_policy = Policy;
    using node_pointer = UTL_SCOPE node_ptr<node_type, node_policy>;
    using node_deleter = typename node_pointer::deleter_type;
private:
    static node_deleter&& decl_deleter() noexcept;
public:
    
};
}   // namespace intrusive
}   // namespace utl
#endif