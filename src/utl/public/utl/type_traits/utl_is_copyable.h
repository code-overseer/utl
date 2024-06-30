// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_is_copy_assignable.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_movable.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_copyable :
    bool_constant<UTL_TRAIT_is_movable(T) && UTL_TRAIT_is_copy_constructible(T) &&
        UTL_TRAIT_is_copy_assignable(T)> {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_copyable_v = is_copyable<T>::value;
#endif // UTL_CXX14

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_copyable(...) UTL_SCOPE is_copyable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_copyable(...) UTL_SCOPE is_copyable<__VA_ARGS__>::value
#endif
