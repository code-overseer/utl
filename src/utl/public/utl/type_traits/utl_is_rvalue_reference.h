// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_rvalue_reference;

#  ifdef UTL_CXX17

using std::is_rvalue_reference_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_rvalue_reference
#    define UTL_DISABLE_BUILTIN_is_rvalue_reference 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_rvalue_reference

#  if UTL_SHOULD_USE_BUILTIN(is_rvalue_reference)
#    define UTL_BUILTIN_is_rvalue_reference(...) __is_rvalue_reference(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_rvalue_reference)

#  ifdef UTL_BUILTIN_is_rvalue_reference

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_rvalue_reference : bool_constant<UTL_BUILTIN_is_rvalue_reference(T)> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_rvalue_reference_v = UTL_BUILTIN_is_rvalue_reference(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_rvalue_reference

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_rvalue_reference : false_type {};

template <typename T>
struct is_rvalue_reference<T&&> : true_type {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_rvalue_reference

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_rvalue_reference 1
