// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::has_virtual_destructor;

#  if UTL_CXX17
using std::has_virtual_destructor_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_has_virtual_destructor 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(has_virtual_destructor)
#    define UTL_BUILTIN_has_virtual_destructor(...) __has_virtual_destructor(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(has_virtual_destructor)

#  ifdef UTL_BUILTIN_has_virtual_destructor

UTL_NAMESPACE_BEGIN

template <typename T>
struct has_virtual_destructor : bool_constant<UTL_BUILTIN_has_virtual_destructor(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool has_virtual_destructor_v = UTL_BUILTIN_has_virtual_destructor(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_has_virtual_destructor 1

#  else // ifdef UTL_BUILTIN_has_virtual_destructor

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct has_virtual_destructor : undefined_trait<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_has_virtual_destructor

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_has_virtual_destructor
#  define UTL_TRAIT_has_virtual_destructor(...) UTL_BUILTIN_has_virtual_destructor(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_has_virtual_destructor(...) UTL_SCOPE has_virtual_destructor_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_has_virtual_destructor(...) UTL_SCOPE has_virtual_destructor<__VA_ARGS__>::value
#endif
