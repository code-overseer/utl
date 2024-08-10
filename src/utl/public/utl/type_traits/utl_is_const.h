// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_const;

#  if UTL_CXX17

using std::is_const_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_const_v = is_const<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_const 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_const)
#    define UTL_BUILTIN_is_const(...) __is_const(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_const)

#  ifdef UTL_BUILTIN_is_const

UTL_NAMESPACE_BEGIN

template <typename T>
struct UTL_PUBLIC_TEMPLATE is_const : bool_constant<UTL_BUILTIN_is_const(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_const_v = UTL_BUILTIN_is_const(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_const

UTL_NAMESPACE_BEGIN

template <typename T>
struct UTL_PUBLIC_TEMPLATE is_const : false_type {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_const<T const> : true_type {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_const<T const volatile> : true_type {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_const_v = is_const<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_const

#  define UTL_TRAIT_SUPPORTED_is_const 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_const
#  define UTL_TRAIT_is_const(...) UTL_BUILTIN_is_const(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_const(...) UTL_SCOPE is_const_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_const(...) UTL_SCOPE is_const<__VA_ARGS__>::value
#endif
