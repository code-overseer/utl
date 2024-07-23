// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/concepts/utl_arithmetic_type.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_arithmetic.h"

namespace std {
/* UTL_UNDEFINED_BEHAVIOUR */
template <typename>
class initializer_list;
} // namespace std

UTL_NAMESPACE_BEGIN

template <typename T>
UTL_ATTRIBUTES(NODISCARD) constexpr T const& max(T const& l, T const& r) {
    return l < r ? r : l;
}

template <typename T, typename F>
UTL_ATTRIBUTES(NODISCARD) constexpr T const& max(T const& l, T const& r, F compare) {
    return compare(l, r) ? r : l;
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD) constexpr T const& max(std::initializer_list<T> list) = delete;

template <typename T, typename F>
UTL_ATTRIBUTES(NODISCARD) constexpr T const& max(std::initializer_list<T> list, F&& comp) = delete;

UTL_NAMESPACE_END
