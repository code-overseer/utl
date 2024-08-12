// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_unbounded_array;

#  if UTL_CXX17

using std::is_unbounded_array_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#else // UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_unbounded_array : false_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_unbounded_array<T[]> : true_type {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;
#  endif // UTL_CXX14

UTL_NAMESPACE_END

#endif // UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#define UTL_TRAIT_SUPPORTED_is_unbounded_array 1

#if UTL_CXX14
#  define UTL_TRAIT_is_unbounded_array(...) UTL_SCOPE is_unbounded_array_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_unbounded_array(...) UTL_SCOPE is_unbounded_array<__VA_ARGS__>::value
#endif
