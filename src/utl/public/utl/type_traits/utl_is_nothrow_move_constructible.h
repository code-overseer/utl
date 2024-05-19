// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_nothrow_move_constructible;

#  if UTL_CXX17

using std::is_nothrow_move_constructible_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_move_constructible_v =
    is_nothrow_move_constructible<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_nothrow_move_constructible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_add_rvalue_reference.h"
#  include "utl/type_traits/utl_is_nothrow_constructible.h"

#  ifdef UTL_BUILTIN_is_nothrow_constructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_nothrow_move_constructible :
    bool_constant<UTL_BUILTIN_is_nothrow_constructible(T, add_rvalue_reference_t<T>)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_move_constructible_v =
    UTL_BUILTIN_is_nothrow_constructible(T, add_rvalue_reference_t<T>);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_move_constructible 1

#  else // ifdef UTL_BUILTIN_is_nothrow_constructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_nothrow_move_constructible : is_nothrow_constructible<T, add_rvalue_reference_t<T>> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_move_constructible_v =
    is_nothrow_constructible_v<T, add_rvalue_reference_t<T>>;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_move_constructible \
        UTL_TRAIT_SUPPORTED_is_nothrow_constructible

#  endif // ifdef UTL_BUILTIN_is_nothrow_constructible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#if UTL_CXX14
#  define UTL_TRAIT_is_nothrow_move_constructible(TYPE) \
      UTL_SCOPE is_nothrow_move_constructible_v<TYPE>
#else
#  define UTL_TRAIT_is_nothrow_move_constructible(TYPE) \
      UTL_SCOPE is_nothrow_move_constructible<TYPE>::value
#endif
