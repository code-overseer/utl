// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_trivially_assignable;

#  ifdef UTL_CXX17

using std::is_trivially_assignable_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_trivially_assignable 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_trivially_assignable
#    define UTL_DISABLE_BUILTIN_is_trivially_assignable 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_trivially_assignable

#  if UTL_SHOULD_USE_BUILTIN(is_trivially_assignable)
#    define UTL_BUILTIN_is_trivially_assignable(...) __is_trivially_assignable(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_trivially_assignable)

#  ifdef UTL_BUILTIN_is_trivially_assignable

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct is_trivially_assignable : bool_constant<UTL_BUILTIN_is_trivially_assignable(T, U)> {};

#    ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_trivially_assignable_v =
    UTL_BUILTIN_is_trivially_assignable(T, U);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_trivially_assignable 1

#  else // ifdef UTL_BUILTIN_is_trivially_assignable

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct is_trivially_assignable : undefined_trait<T, U> {};

#    ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;
#    endif // UTL_CXX14

#    define UTL_TRAIT_SUPPORTED_is_nothrow_assignable 0

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_trivially_assignable

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
