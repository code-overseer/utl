// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX17 && UTL_USE_STD_as_const

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::as_const

    UTL_NAMESPACE_END

#else // if UTL_CXX17 && UTL_USE_STD_as_const

#  if UTL_USE_STD_as_const
UTL_PRAGMA_WARN(
    "The current standard does not implement constexpr as_const, `UTL_USE_STD_as_const` ignored")
#  endif // if UTL_CXX17 && UTL_USE_STD_as_const

UTL_NAMESPACE_BEGIN

template <typename T>
void as_const(T&&) noexcept = delete;

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE, _HIDE_FROM_ABI) constexpr T const& as_const(
    T const& value UTL_LIFETIMEBOUND) noexcept {
    return value;
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE, _HIDE_FROM_ABI) constexpr T const& as_const(
    T& value UTL_LIFETIMEBOUND) noexcept {
    return value;
}

UTL_NAMESPACE_END

#endif // UTL_CXX17 && UTL_USE_STD_as_const
