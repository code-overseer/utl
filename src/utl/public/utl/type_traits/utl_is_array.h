// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_array;

#  if UTL_CXX17
using std::is_array_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_array_v = is_array<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_array 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_COMPILER_CLANG && defined(UTL_DISABLE_BUILTIN_is_array) && !UTL_DISABLE_BUILTIN_is_array
/** (31.01.2024) Clang's __is_array has a bug so disable for now [https://llvm.org/PR54705];
 * maybe just disable any compiler with Clang frontend? */
UTL_PRAGMA_WARN("builtin is_array is disabled by default and cannot be enabled")
#    undef UTL_DISABLE_BUILTIN_is_array
#    define UTL_DISABLE_BUILTIN_is_array 1
#  elif !defined(UTL_DISABLE_BUILTIN_is_array)
#    define UTL_DISABLE_BUILTIN_is_array 1
#  endif

#  if UTL_SHOULD_USE_BUILTIN(is_array)
#    define UTL_BUILTIN_is_array(...) __is_array(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_array)

#  ifdef UTL_BUILTIN_is_array

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_array : bool_constant<UTL_BUILTIN_is_array(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_array_v = UTL_BUILTIN_is_array(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_array

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_array : false_type {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_array<T[]> : true_type {};

template <typename T, size_t N>
struct __UTL_PUBLIC_TEMPLATE is_array<T[N]> : true_type {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_array_v = is_array<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_array

#  define UTL_TRAIT_SUPPORTED_is_array 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_array
#  define UTL_TRAIT_is_array(...) UTL_BUILTIN_is_array(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_array(...) UTL_SCOPE is_array_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_array(...) UTL_SCOPE is_array<__VA_ARGS__>::value
#endif
