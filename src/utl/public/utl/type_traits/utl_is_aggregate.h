// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if defined(UTL_USE_STD_TYPE_TRAITS) && UTL_CXX17

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_aggregate;

#  if UTL_CXX17
using std::is_aggregate_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_aggregate_v = is_aggregate<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_aggregate 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_aggregate
#    define UTL_DISABLE_BUILTIN_is_aggregate 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_aggregate

#  if UTL_SHOULD_USE_BUILTIN(is_aggregate)
#    define UTL_BUILTIN_is_aggregate(...) __is_aggregate(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_aggregate)

#  ifdef UTL_BUILTIN_is_aggregate

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_aggregate : bool_constant<UTL_BUILTIN_is_aggregate(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_aggregate_v = UTL_BUILTIN_is_aggregate(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_aggregate 1

#  else // ifdef UTL_BUILTIN_is_aggregate

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_aggregate : undefined_trait<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_aggregate_v = is_aggregate<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_aggregate 0

#  endif // ifdef UTL_BUILTIN_is_aggregate

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_aggregate
#  define UTL_TRAIT_is_aggregate(...) UTL_BUILTIN_is_aggregate(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_aggregate(...) UTL_SCOPE is_aggregate_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_aggregate(...) UTL_SCOPE is_aggregate<__VA_ARGS__>::value
#endif
