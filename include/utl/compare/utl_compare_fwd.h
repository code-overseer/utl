// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/preprocessor/utl_compiler.h"

#ifdef UTL_COMPILER_GCC
// TODO: add preprocessor flag to either include std header/use this UB/disable std compatibility
namespace std {
/* UTL_UNDEFINED_BEHAVIOUR */
/* @note (22/12/2023) GCC does not put the orderings under an inline namespace */
class strong_ordering;
class partial_ordering;
class weak_ordering;
}
#else
UTL_STD_NAMESPACE_BEGIN
/* UTL_UNDEFINED_BEHAVIOUR */
class strong_ordering;
class partial_ordering;
class weak_ordering;
UTL_STD_NAMESPACE_END
#endif

UTL_NAMESPACE_BEGIN

class strong_ordering;
class partial_ordering;
class weak_ordering;

namespace compare {
namespace details {
using value_t = signed char;
enum class order_t : value_t { equal = 0, less = -1, greater = 1 };
enum class unorder_t : value_t { unordered = 2 };
class obscure {
    friend class UTL_SCOPE strong_ordering;
    friend class UTL_SCOPE partial_ordering;
    friend class UTL_SCOPE weak_ordering;
    struct zero_t {
        UTL_CONSTEVAL_CXX20 zero_t(zero_t*) noexcept {}
    };
};
}
}

UTL_NAMESPACE_END

