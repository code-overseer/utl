// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_assignable.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_object.h"
#include "utl/type_traits/utl_is_swappable.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_movable :
    bool_constant<UTL_TRAIT_is_object(T) && UTL_TRAIT_is_move_constructible(T) &&
        UTL_TRAIT_is_assignable(T&, T) && UTL_TRAIT_is_swappable(T)> {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_movable_v =
    UTL_TRAIT_is_object(T) && UTL_TRAIT_is_move_constructible(T) &&
    UTL_TRAIT_is_assignable(T&, T) && UTL_TRAIT_is_swappable(T);
#endif // UTL_CXX14

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_movable(...) UTL_SCOPE is_movable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_movable(...) UTL_SCOPE is_movable<__VA_ARGS__>::value
#endif
