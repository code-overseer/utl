// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_same;

#  if UTL_CXX17
using std::is_same_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_same_v = is_same<T, U>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_same)
#    define UTL_BUILTIN_is_same(...) __is_same(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_same)

#  ifdef UTL_BUILTIN_is_same

UTL_NAMESPACE_BEGIN

template <typename T0, typename T1>
struct __UTL_PUBLIC_TEMPLATE is_same : bool_constant<UTL_BUILTIN_is_same(T0, T1)> {};

#    if UTL_CXX14
template <typename T0, typename T1>
UTL_INLINE_CXX17 constexpr bool is_same_v = UTL_BUILTIN_is_same(T0, T1);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_same

UTL_NAMESPACE_BEGIN

template <typename, typename>
struct __UTL_PUBLIC_TEMPLATE is_same : false_type {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_same<T, T> : true_type {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_same_v = is_same<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_same

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_same 1

#ifdef UTL_BUILTIN_is_same
#  define UTL_TRAIT_is_same(...) UTL_BUILTIN_is_same(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_same(...) __UTL is_same_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_same(...) __UTL is_same<__VA_ARGS__>::value
#endif
