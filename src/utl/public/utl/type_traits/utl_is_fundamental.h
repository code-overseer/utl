// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_fundamental;

#  if UTL_CXX17
using std::is_fundamental_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_fundamental_v = is_fundamental<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_fundamental 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_fundamental)
#    define UTL_BUILTIN_is_fundamental(...) __is_fundamental(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_fundamental)

#  ifdef UTL_BUILTIN_is_fundamental

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_fundamental : bool_constant<UTL_BUILTIN_is_fundamental(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_fundamental_v = UTL_BUILTIN_is_fundamental(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_fundamental 1

#  else // ifdef UTL_BUILTIN_is_fundamental

#    include "utl/type_traits/utl_is_arithmetic.h"
#    include "utl/type_traits/utl_is_null_pointer.h"
#    include "utl/type_traits/utl_is_void.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_fundamental :
    bool_constant<is_void<T>::value || is_null_pointer<T>::value || is_arithmetic<T>::value> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_fundamental_v = is_fundamental<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_fundamental                              \
        UTL_TRAIT_SUPPORTED_is_void&& UTL_TRAIT_SUPPORTED_is_null_pointer&& \
            UTL_TRAIT_SUPPORTED_is_arithmetic

#  endif // ifdef UTL_BUILTIN_is_fundamental

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_fundamental
#  define UTL_TRAIT_is_fundamental(...) UTL_BUILTIN_is_fundamental(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_fundamental(...) UTL_SCOPE is_fundamental_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_fundamental(...) UTL_SCOPE is_fundamental<__VA_ARGS__>::value
#endif
