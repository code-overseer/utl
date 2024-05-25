// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_member_pointer;

#  if UTL_CXX17

using std::is_member_pointer_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_member_pointer 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_member_pointer)
#    define UTL_BUILTIN_is_member_pointer(...) __is_member_pointer(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_member_pointer)

#  ifdef UTL_BUILTIN_is_member_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_member_pointer : bool_constant<UTL_BUILTIN_is_member_pointer(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_member_pointer_v = UTL_BUILTIN_is_member_pointer(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_member_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_member_pointer : false_type {};

template <typename T, typename U>
struct is_member_pointer<T U::*> : true_type {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_member_pointer_v = is_member_pointer<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_member_pointer

#  define UTL_TRAIT_SUPPORTED_is_member_pointer 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_member_pointer
#  define UTL_TRAIT_is_member_pointer(...) UTL_BUILTIN_is_member_pointer(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_member_pointer(...) UTL_SCOPE is_member_pointer_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_member_pointer(...) UTL_SCOPE is_member_pointer<__VA_ARGS__>::value
#endif
