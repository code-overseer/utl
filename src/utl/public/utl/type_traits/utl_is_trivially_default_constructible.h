// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_trivially_default_constructible;

#  if UTL_CXX17

using std::is_trivially_default_constructible_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivially_default_constructible_v =
    is_trivially_default_constructible<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_trivially_default_constructible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_is_trivially_constructible.h"

#  ifdef UTL_BUILTIN_is_trivially_constructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_trivially_default_constructible :
    bool_constant<UTL_BUILTIN_is_trivially_constructible(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivially_default_constructible_v =
    UTL_BUILTIN_is_trivially_constructible(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_trivially_constructible 1

#  else // ifdef UTL_BUILTIN_is_trivially_constructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_trivially_default_constructible : is_trivially_constructible<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivially_default_constructible_v = is_trivially_constructible_v<T>;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_trivially_default_constructible \
        UTL_TRAIT_SUPPORTED_is_trivially_constructible

#  endif // ifdef UTL_BUILTIN_is_trivially_constructible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_trivially_constructible
#  define UTL_TRAIT_is_trivially_default_constructible(TYPE) \
      UTL_BUILTIN_is_trivially_constructible(TYPE)
#elif UTL_CXX14
#  define UTL_TRAIT_is_trivially_default_constructible(TYPE) \
      UTL_SCOPE is_trivially_default_constructible_v<TYPE>
#else
#  define UTL_TRAIT_is_trivially_default_constructible(TYPE) \
      UTL_SCOPE is_trivially_default_constructible<TYPE>::value
#endif
