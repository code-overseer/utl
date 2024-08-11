// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::extent;

#  if UTL_CXX17
using std::extent_v;
#  elif UTL_CXX14 // UTL_CXX17
template <typename T, size_t Dim = 0>
UTL_INLINE_CXX17 constexpr size_t extent_v = extent<T, Dim>::value;
#  endif          // UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"
/* (31.01.2024) Clang's __array_extent has a bug so disable for now */
#  if UTL_SHOULD_USE_BUILTIN(array_extent)
#    define UTL_BUILTIN_array_extent(...) __array_extent(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(array_extent)

#  ifdef UTL_BUILTIN_array_extent

UTL_NAMESPACE_BEGIN

template <typename T, size_t Dim = 0>
struct UTL_PUBLIC_TEMPLATE extent : size_constant<UTL_BUILTIN_array_extent(T, Dim)> {};

#    if UTL_CXX14
template <typename T, size_t Dim = 0>
UTL_INLINE_CXX17 constexpr size_t extent_v = UTL_BUILTIN_array_extent(T, Dim);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_array_extent

UTL_NAMESPACE_BEGIN

template <typename T, size_t Dim>
struct UTL_PUBLIC_TEMPLATE extent : size_constant<0> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE extent<T[], 0> : size_constant<0> {};
template <typename T, size_t Dim>
struct UTL_PUBLIC_TEMPLATE extent<T[], Dim> : extent<T, Dim - 1> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE extent<T[N], 0> : size_constant<N> {};
template <typename T, size_t Dim>
struct UTL_PUBLIC_TEMPLATE extent<T[N], Dim> : extent<T, Dim - 1> {};

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_array_extent

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_extent 1

#if UTL_CXX14
#  define UTL_TRAIT_extent(...) UTL_SCOPE extent_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_extent(...) UTL_SCOPE extent<__VA_ARGS__>::value
#endif
