// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX14)

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_null_pointer;

#  ifdef UTL_CXX17
using std::is_null_pointer_v;
#  elif defined(UTL_CXX14) // ifdef UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
#  endif                   // ifdef UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_null_pointer
#    define UTL_DISABLE_BUILTIN_is_null_pointer 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_null_pointer

#  if UTL_SHOULD_USE_BUILTIN(is_null_pointer)
#    define UTL_BUILTIN_is_null_pointer(...) __is_null_pointer(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_null_pointer)

#  ifdef UTL_BUILTIN_is_null_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_null_pointer : bool_constant<UTL_BUILTIN_is_null_pointer(T)> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_null_pointer_v = UTL_BUILTIN_is_null_pointer(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_null_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_null_pointer : false_type {};

template <>
struct is_null_pointer<void> : true_type {};

template <>
struct is_null_pointer<void const> : true_type {};

template <>
struct is_null_pointer<void volatile> : true_type {};

template <>
struct is_null_pointer<void const volatile> : true_type {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_null_pointer

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_null_pointer 1
