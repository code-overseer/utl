// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_member_object_pointer;

#  ifdef UTL_CXX17

using std::is_member_object_pointer_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_member_object_pointer 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_member_object_pointer
#    define UTL_DISABLE_BUILTIN_is_member_object_pointer 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_member_object_pointer

#  if UTL_SHOULD_USE_BUILTIN(is_member_object_pointer)
#    define UTL_BUILTIN_is_member_object_pointer(...) __is_member_object_pointer(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_member_object_pointer)

#  ifdef UTL_BUILTIN_is_member_object_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_member_object_pointer : bool_constant<UTL_BUILTIN_is_member_object_pointer(T)> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_member_object_pointer_v =
    UTL_BUILTIN_is_member_object_pointer(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_member_object_pointer

#    include "utl/type_traits/utl_is_function.h"

#    ifdef UTL_CXX14
#      define UTL_TRAIT_VALUE(TRAIT, ...) TRAIT##_v<__VA_ARGS__>
#    else
#      define UTL_TRAIT_VALUE(TRAIT, ...) TRAIT<__VA_ARGS__>::value
#    endif

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_member_object_pointer : false_type {};

template <typename T, typename U>
struct is_member_object_pointer<T U::*> : bool_constant<!UTL_TRAIT_VALUE(is_function, T)> {};

#    undef UTL_TRAIT_VALUE

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_member_object_pointer

#  define UTL_TRAIT_SUPPORTED_is_member_object_pointer 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
