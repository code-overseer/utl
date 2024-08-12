// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_class;

#  if UTL_CXX17
using std::is_class_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_class_v = is_class<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_class 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_class)
#    define UTL_BUILTIN_is_class(...) __is_class(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_class)

#  ifdef UTL_BUILTIN_is_class

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_class : bool_constant<UTL_BUILTIN_is_class(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_class_v = UTL_BUILTIN_is_class(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_class 1

#  else // ifdef UTL_BUILTIN_is_class

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_class : undefined_trait<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_class_v = is_class<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_class

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_class
#  define UTL_TRAIT_is_class(...) UTL_BUILTIN_is_class(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_class(...) UTL_SCOPE is_class_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_class(...) UTL_SCOPE is_class<__VA_ARGS__>::value
#endif
