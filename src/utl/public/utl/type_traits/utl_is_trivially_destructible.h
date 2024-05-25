// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_trivially_destructible;

#  if UTL_CXX17

using std::is_trivially_destructible_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_trivially_destructible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_trivially_destructible)
#    define UTL_BUILTIN_is_trivially_destructible(...) __is_trivially_destructible(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_trivially_destructible)

#  ifdef UTL_BUILTIN_is_trivially_destructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_trivially_destructible : bool_constant<UTL_BUILTIN_is_trivially_destructible(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivially_destructible_v = UTL_BUILTIN_is_trivially_destructible(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_trivially_destructible 1

#  else // ifdef UTL_BUILTIN_is_trivially_destructible

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_trivially_destructible : undefined_trait<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;
#    endif // UTL_CXX14

#    define UTL_TRAIT_SUPPORTED_is_nothrow_constructible 0

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_trivially_destructible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_trivially_destructible
#  define UTL_TRAIT_is_trivially_destructible(...) \
      UTL_BUILTIN_is_trivially_destructible(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_trivially_destructible(...) \
      UTL_SCOPE is_trivially_destructible_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_trivially_destructible(...) \
      UTL_SCOPE is_trivially_destructible<__VA_ARGS__>::value
#endif
