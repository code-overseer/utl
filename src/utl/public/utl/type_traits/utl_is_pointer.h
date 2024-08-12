// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_pointer;

#  if UTL_CXX17
using std::is_pointer_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_pointer_v = is_pointer<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_pointer)
#    define UTL_BUILTIN_is_pointer(...) __is_pointer(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_pointer)

#  ifdef UTL_BUILTIN_is_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_pointer : bool_constant<UTL_BUILTIN_is_pointer(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_pointer_v = UTL_BUILTIN_is_pointer(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_pointer : false_type {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_pointer<T*> : true_type {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_pointer<T* const> : true_type {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_pointer<T* volatile> : true_type {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_pointer<T* const volatile> : true_type {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_pointer_v = is_pointer<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_pointer

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_pointer 1

#ifdef UTL_BUILTIN_is_pointer
#  define UTL_TRAIT_is_pointer(...) UTL_BUILTIN_is_pointer(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_pointer(...) __UTL is_pointer_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_pointer(...) __UTL is_pointer<__VA_ARGS__>::value
#endif
