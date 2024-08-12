// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_trivially_copyable;

#  if UTL_CXX17

using std::is_trivially_copyable_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_trivially_copyable 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_trivially_copyable)
#    define UTL_BUILTIN_is_trivially_copyable(...) __is_trivially_copyable(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_trivially_copyable)

#  ifdef UTL_BUILTIN_is_trivially_copyable

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_trivially_copyable :
    bool_constant<UTL_BUILTIN_is_trivially_copyable(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivially_copyable_v = UTL_BUILTIN_is_trivially_copyable(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_trivially_copyable 1

#  else // ifdef UTL_BUILTIN_is_trivially_copyable

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_trivially_copyable : undefined_trait<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;
#    endif // UTL_CXX14

#    define UTL_TRAIT_SUPPORTED_is_trivially_copyable 0

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_trivially_copyable

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_trivially_copyable
#  define UTL_TRAIT_is_trivially_copyable(...) UTL_BUILTIN_is_trivially_copyable(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_trivially_copyable(...) __UTL is_trivially_copyable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_trivially_copyable(...) __UTL is_trivially_copyable<__VA_ARGS__>::value
#endif
