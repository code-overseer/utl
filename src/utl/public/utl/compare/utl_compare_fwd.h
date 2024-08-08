// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"

#if UTL_COMPILER_GCC
// TODO: add preprocessor flag to either include std header/use this UB/disable std compatibility
namespace std {
/* UTL_UNDEFINED_BEHAVIOUR */
/* @note (22/12/2023) GCC does not put the orderings under an inline namespace */
class UTL_ABI_PUBLIC strong_ordering;
class UTL_ABI_PUBLIC partial_ordering;
class UTL_ABI_PUBLIC weak_ordering;
} // namespace std
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

namespace details {
namespace compare {
using value_t = signed char;
enum class order_t : value_t {
    equal = 0,
    less = -1,
    greater = 1
};
enum class unorder_t : value_t {
    unordered = 2
};
struct zero_t {
    UTL_CONSTEVAL zero_t(zero_t*) noexcept {}
};
template <typename T>
struct less_value {
    UTL_ABI_PUBLIC static T const less;
};
template <typename T>
struct greater_value {
    UTL_ABI_PUBLIC static T const greater;
};
template <typename T>
struct equal_value {
    UTL_ABI_PUBLIC static T const equal;
};
template <typename T>
struct equivalent_value {
    UTL_ABI_PUBLIC static T const equivalent;
};
template <typename T>
struct unordered_value {
    UTL_ABI_PUBLIC static T const unordered;
};

template <typename T>
UTL_ABI_PUBLIC_DATA constexpr T less_value<T>::less{order_t::less};
template <typename T>
UTL_ABI_PUBLIC_DATA constexpr T greater_value<T>::greater{order_t::greater};
template <typename T>
UTL_ABI_PUBLIC_DATA constexpr T equal_value<T>::equal{order_t::equal};
template <typename T>
UTL_ABI_PUBLIC_DATA constexpr T equivalent_value<T>::equivalent{order_t::equal};
template <typename T>
UTL_ABI_PUBLIC_DATA constexpr T unordered_value<T>::unordered{unorder_t::unordered};
} // namespace compare
} // namespace details

UTL_NAMESPACE_END
