// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_signed;

#  ifdef UTL_CXX17

using std::is_signed_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_signed_v = is_signed<T>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_signed
#    define UTL_DISABLE_BUILTIN_is_signed 0
#  endif

#  if UTL_SHOULD_USE_BUILTIN(is_signed)
#    define UTL_BUILTIN_is_signed(...) __is_signed(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_signed)

#  ifdef UTL_BUILTIN_is_signed

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_signed : bool_constant<UTL_BUILTIN_is_signed(T)> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_signed_v = UTL_BUILTIN_is_signed(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_signed 1

#  else // ifdef UTL_BUILTIN_is_signed

#    include "utl/type_traits/utl_is_arithmetic.h"
#    include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_signed : conjunction<is_arithmetic<T>, bool_constant<(T(-1) < T(0))>> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_signed_v = is_signed<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_signed UTL_TRAIT_SUPPORTED_is_arithmetic

#  endif // ifdef UTL_BUILTIN_is_signed

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
