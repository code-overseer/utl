// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_USE_STD_swap

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::swap;

UTL_NAMESPACE_END

#else // UTL_USE_STD_swap

#  include "utl/ranges/utl_swap.h"

#  if UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename T>
requires requires(T& l, T& r) { ranges::swap(l, r); }
__UTL_HIDE_FROM_ABI inline constexpr void swap(T& l, T& r) noexcept(noexcept(ranges::swap(l, r))) {
    ranges::swap(l, r);
}

UTL_NAMESPACE_END

#  else

UTL_NAMESPACE_BEGIN

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr auto swap(T& l, T& r) noexcept(noexcept(ranges::swap(l, r)))
    -> decltype(ranges::swap(l, r)) {
    ranges::swap(l, r);
}

UTL_NAMESPACE_END

#  endif
#endif // UTL_CXX20 && UTL_USE_STD_swap
