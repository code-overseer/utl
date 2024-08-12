// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_compound;

#  if UTL_CXX17
using std::is_compound_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_compound_v = is_compund<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_compound 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_compound)
#    define UTL_BUILTIN_is_compound(...) __is_compound(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_compound)

#  ifdef UTL_BUILTIN_is_compound

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_compound : bool_constant<UTL_BUILTIN_is_compound(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_compound_v = UTL_BUILTIN_is_compound(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_compound 1

#  else // ifdef UTL_BUILTIN_is_compound

#    include "utl/type_traits/utl_is_fundamental.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_compound : bool_constant<!is_fundamental<T>::value> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_compound_v = !is_fundamental<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_compound UTL_TRAIT_SUPPORTED_is_fundamental

#  endif // ifdef UTL_BUILTIN_is_compound

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#if UTL_CXX14
#  define UTL_TRAIT_is_compound(TYPE) UTL_SCOPE is_compound_v<TYPE>
#else
#  define UTL_TRAIT_is_compound(TYPE) UTL_SCOPE is_compound<TYPE>::value
#endif
