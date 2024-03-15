// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_trivially_constructible;

#  ifdef UTL_CXX17

using std::is_trivially_constructible_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_trivially_constructible_v =
    is_trivially_constructible<T, Args...>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_trivially_constructible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_trivially_constructible
#    define UTL_DISABLE_BUILTIN_is_trivially_constructible 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_trivially_constructible

#  if UTL_SHOULD_USE_BUILTIN(is_trivially_constructible)
#    define UTL_BUILTIN_is_trivially_constructible(...) __is_trivially_constructible(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_trivially_constructible)

#  ifdef UTL_BUILTIN_is_trivially_constructible

UTL_NAMESPACE_BEGIN

template <typename T, typename... Args>
struct is_trivially_constructible :
    bool_constant<UTL_BUILTIN_is_trivially_constructible(T, Args...)> {};

#    ifdef UTL_CXX14
template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_trivially_constructible_v =
    UTL_BUILTIN_is_trivially_constructible(T, Args...);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_trivially_constructible 1

#  else // ifdef UTL_BUILTIN_is_trivially_constructible

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename... Args>
struct is_trivially_constructible : undefined_trait<T, Args...> {};

#    ifdef UTL_CXX14
template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_trivially_constructible_v =
    is_trivially_constructible<T, Args...>::value;
#    endif // UTL_CXX14

#    define UTL_TRAIT_SUPPORTED_is_nothrow_constructible 0

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_trivially_constructible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
