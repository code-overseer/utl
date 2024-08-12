// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_function;

#  if UTL_CXX17
using std::is_function_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_function_v = is_function<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_function 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  if __UTL_SHOULD_USE_BUILTIN(is_function)
#    define UTL_BUILTIN_is_function(...) __is_function(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_function)

#  ifdef UTL_BUILTIN_is_function

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_function : bool_constant<UTL_BUILTIN_is_function(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_function_v = UTL_BUILTIN_is_function(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_function 1

#  else // ifdef UTL_BUILTIN_is_function

#    include "utl/type_traits/utl_is_const.h"
#    include "utl/type_traits/utl_is_reference.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_function :
    bool_constant<!is_const<T const>::value && !is_reference<T>::value> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_function_v = is_function<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_function \
        UTL_TRAIT_SUPPORTED_is_const&& UTL_TRAIT_SUPPORTED_is_reference

#  endif // ifdef UTL_BUILTIN_is_function

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_function
#  define UTL_TRAIT_is_function(...) UTL_BUILTIN_is_function(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_function(...) UTL_SCOPE is_function_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_function(...) UTL_SCOPE is_function<__VA_ARGS__>::value
#endif
