// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_arithmetic;

#  if UTL_CXX17
using std::is_arithmetic_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_arithmetic 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"
#  include "utl/type_traits/utl_is_floating_point.h"
#  include "utl/type_traits/utl_is_integral.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_arithmetic :
    bool_constant<is_integral<T>::value || is_floating_point<T>::value> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
#  endif // UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_arithmetic \
      UTL_TRAIT_SUPPORTED_is_integral&& UTL_TRAIT_SUPPORTED_is_floating_point

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#if UTL_CXX14
#  define UTL_TRAIT_is_arithmetic(...) UTL_SCOPE is_arithmetic_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_arithmetic(...) UTL_SCOPE is_arithmetic<__VA_ARGS__>::value
#endif
