// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_bounded_array;

using std::is_bounded_array_v;

UTL_NAMESPACE_END

#else // UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_bounded_array : false_type {};
template <typename T, size_t N>
struct __UTL_PUBLIC_TEMPLATE is_bounded_array<T[N]> : true_type {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
#  endif // UTL_CXX14

UTL_NAMESPACE_END

#endif // UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#define UTL_TRAIT_SUPPORTED_is_bounded_array 1

#if UTL_CXX14
#  define UTL_TRAIT_is_bounded_array(...) UTL_SCOPE is_bounded_array_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_bounded_array(...) UTL_SCOPE is_bounded_array<__VA_ARGS__>::value
#endif
