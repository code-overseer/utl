// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_trivially_assignable;

#  if UTL_CXX17

using std::is_trivially_assignable_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_trivially_assignable 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_trivially_assignable)
#    define UTL_BUILTIN_is_trivially_assignable(...) __is_trivially_assignable(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_trivially_assignable)

#  ifdef UTL_BUILTIN_is_trivially_assignable

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_trivially_assignable :
    bool_constant<UTL_BUILTIN_is_trivially_assignable(T, U)> {};

#    if UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_trivially_assignable_v = UTL_BUILTIN_is_trivially_assignable(T, U);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_trivially_assignable 1

#  else // ifdef UTL_BUILTIN_is_trivially_assignable

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_trivially_assignable : undefined_trait<T, U> {};

#    if UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;
#    endif // UTL_CXX14

#    define UTL_TRAIT_SUPPORTED_is_nothrow_assignable 0

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_trivially_assignable

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
