// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX14

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_null_pointer;

#  if UTL_CXX17
using std::is_null_pointer_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_null_pointer)
#    define UTL_BUILTIN_is_null_pointer(...) __is_null_pointer(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_null_pointer)

#  ifdef UTL_BUILTIN_is_null_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_null_pointer : bool_constant<UTL_BUILTIN_is_null_pointer(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_null_pointer_v = UTL_BUILTIN_is_null_pointer(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_null_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_null_pointer : false_type {};

template <>
struct is_null_pointer<decltype(nullptr)> : true_type {};

template <>
struct is_null_pointer<decltype(nullptr) const> : true_type {};

template <>
struct is_null_pointer<decltype(nullptr) volatile> : true_type {};

template <>
struct is_null_pointer<decltype(nullptr) const volatile> : true_type {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_null_pointer

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_null_pointer 1

#ifdef UTL_BUILTIN_is_null_pointer
#  define UTL_TRAIT_is_null_pointer(...) UTL_BUILTIN_is_null_pointer(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_null_pointer(...) UTL_SCOPE is_null_pointer_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_null_pointer(...) UTL_SCOPE is_null_pointer<__VA_ARGS__>::value
#endif
