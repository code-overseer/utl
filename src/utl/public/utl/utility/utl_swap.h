// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_USE_STD_swap

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::swap;

UTL_NAMESPACE_END

#else // UTL_USE_STD_swap

#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_swappable.h"

UTL_NAMESPACE_BEGIN

template <typename T>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 auto swap(T& l, T& r) noexcept(
    UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_assignable(T))
    -> enable_if_t<UTL_TRAIT_is_move_constructible(T) && UTL_TRAIT_is_move_assignable(T)> {
    T tmp{__UTL move(l)};
    l = __UTL move(r);
    r = __UTL move(tmp);
}

template <typename T, size_t N>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 auto swap(T (&l)[N], T (&r)[N]) noexcept(
    UTL_TRAIT_is_nothrow_swappable(T)) -> enable_if_t<UTL_TRAIT_is_swappable(T)> {
    for (auto i = 0u; i < N; ++i) {
        swap(l[i], r[i]);
    }
}

UTL_NAMESPACE_END

#endif // UTL_USE_STD_swap
