// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_trivial;

#  if UTL_CXX17
using std::is_trivial_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivial_v = is_trivial<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_trivial 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_trivial)
#    define UTL_BUILTIN_is_trivial(...) __is_trivial(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_trivial)

#  ifdef UTL_BUILTIN_is_trivial

UTL_NAMESPACE_BEGIN

template <typename T>
struct UTL_PUBLIC_TEMPLATE is_trivial : bool_constant<UTL_BUILTIN_is_trivial(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivial_v = UTL_BUILTIN_is_trivial(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_trivial 1

#  else // ifdef UTL_BUILTIN_is_trivial

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct UTL_PUBLIC_TEMPLATE is_trivial : undefined_trait<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivial_v = is_trivial<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_trivial

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_trivial
#  define UTL_TRAIT_is_trivial(...) UTL_BUILTIN_is_trivial(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_trivial(...) UTL_SCOPE is_trivial_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_trivial(...) UTL_SCOPE is_trivial<__VA_ARGS__>::value
#endif
