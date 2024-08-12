// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_volatile;

#  if UTL_CXX17

using std::is_volatile_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_volatile_v = is_volatile<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_volatile 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_volatile)
#    define UTL_BUILTIN_is_volatile(...) __is_volatile(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_volatile)

#  ifdef UTL_BUILTIN_is_volatile

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_volatile : bool_constant<UTL_BUILTIN_is_volatile(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_volatile_v = UTL_BUILTIN_is_volatile(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_volatile

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_volatile : false_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_volatile<T volatile> : true_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_volatile<T const volatile> : true_type {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_volatile_v = is_volatile<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_volatile

#  define UTL_TRAIT_SUPPORTED_is_volatile 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_volatile
#  define UTL_TRAIT_is_volatile(...) UTL_BUILTIN_is_volatile(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_volatile(...) UTL_SCOPE is_volatile_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_volatile(...) UTL_SCOPE is_volatile<__VA_ARGS__>::value
#endif
