// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_same;

#  ifdef UTL_CXX17
using std::is_same_v;
#  elif defined(UTL_CXX14) // ifdef UTL_CXX17
template <typename T, typename U> UTL_INLINE_CXX17 constexpr bool is_same_v = is_same<T, U>::value;
#  endif                   // ifdef UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_same
#    define UTL_DISABLE_BUILTIN_is_same 0
#  endif

#  if UTL_SHOULD_USE_BUILTIN(is_same)
#    define UTL_BUILTIN_is_same(...) __is_same(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_same)

#  ifdef UTL_BUILTIN_is_same

UTL_NAMESPACE_BEGIN

template <typename T0, typename T1> struct is_same : bool_constant<UTL_BUILTIN_is_same(T0, T1)> {};

#    ifdef UTL_CXX14
template <typename T0, typename T1>
UTL_INLINE_CXX17 constexpr bool is_same_v = UTL_BUILTIN_is_same(T0, T1);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_same

UTL_NAMESPACE_BEGIN

template <typename, typename> struct is_same : false_type {};

template <typename T> struct is_same<T, T> : true_type {};

#    ifdef UTL_CXX14
template <typename T> UTL_INLINE_CXX17 constexpr bool is_same_v = is_same<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_same

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_same 1
