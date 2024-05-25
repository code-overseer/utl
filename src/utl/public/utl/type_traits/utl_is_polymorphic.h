// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_polymorphic;

#  if UTL_CXX17
using std::is_polymorphic_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_polymorphic_v = is_polymorphic<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_polymorphic 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_polymorphic)
#    define UTL_BUILTIN_is_polymorphic(...) __is_polymorphic(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_polymorphic)

#  ifdef UTL_BUILTIN_is_polymorphic

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_polymorphic : bool_constant<UTL_BUILTIN_is_polymorphic(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_polymorphic_v = UTL_BUILTIN_is_polymorphic(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_polymorphic 1

#  else // ifdef UTL_BUILTIN_is_polymorphic

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_polymorphic : undefined_trait<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_polymorphic_v = is_polymorphic<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_polymorphic

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_polymorphic
#  define UTL_TRAIT_is_polymorphic(...) UTL_BUILTIN_is_polymorphic(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_polymorphic(...) UTL_SCOPE is_polymorphic_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_polymorphic(...) UTL_SCOPE is_polymorphic<__VA_ARGS__>::value
#endif
