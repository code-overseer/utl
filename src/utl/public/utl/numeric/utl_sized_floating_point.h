// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20
#  include "utl/concepts/utl_floating_point.h"

UTL_NAMESPACE_BEGIN

template <typename T, size_t N>
concept sized_floating_point = floating_point<T> && sizeof(T) == N;

template <size_t N, typename T>
struct __UTL_PUBLIC_TEMPLATE is_sized_floating_point : bool_constant<sized_floating_point<T, N>> {};

template <size_t N, typename T>
inline constexpr bool is_sized_floating_point_v = sized_floating_point<T, N>;

#  define UTL_TRAIT_is_sized_floating_point(...) sized_floating_point<__VA_ARGS__>

UTL_NAMESPACE_END
#else

#  include "utl/type_traits/utl_is_floating_point.h"
UTL_NAMESPACE_BEGIN

template <size_t N, typename T>
struct __UTL_PUBLIC_TEMPLATE is_sized_floating_point :
    bool_constant<UTL_TRAIT_is_floating_point(T) && sizeof(T) == N> {};

#  if UTL_CXX14
template <size_t N, typename T>
UTL_INLINE_CXX17 constexpr bool is_sized_floating_point_v = UTL_TRAIT_is_floating_point(T) && sizeof(T) == N;
#  endif

UTL_NAMESPACE_END

#  if UTL_CXX14
#    define UTL_TRAIT_is_sized_floating_point(...) is_sized_floating_point_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_sized_floating_point(...) is_sized_floating_point<__VA_ARGS__>::value
#  endif
#endif
