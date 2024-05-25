// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_trivially_move_assignable;

#  if UTL_CXX17

using std::is_trivially_move_assignable_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_trivially_move_assignable 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_add_lvalue_reference.h"
#  include "utl/type_traits/utl_add_rvalue_reference.h"
#  include "utl/type_traits/utl_is_trivially_assignable.h"

#  ifdef UTL_BUILTIN_is_trivially_assignable

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_trivially_move_assignable :
    bool_constant<UTL_BUILTIN_is_trivially_assignable(
        add_lvalue_reference_t<T>, add_rvalue_reference_t<T>)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivially_move_assignable_v =
    UTL_BUILTIN_is_trivially_assignable(add_lvalue_reference_t<T>, add_rvalue_reference_t<T>);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_trivially_move_assignable 1

#  else // ifdef UTL_BUILTIN_is_trivially_assignable

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_trivially_move_assignable :
    is_trivially_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_trivially_move_assignable_v =
    is_trivially_assignable_v<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>>;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_trivially_move_assignable \
        UTL_TRAIT_SUPPORTED_is_trivially_assignable

#  endif // ifdef UTL_BUILTIN_is_trivially_assignable

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#if UTL_CXX14
#  define UTL_TRAIT_is_trivially_move_assignable(TYPE) \
      UTL_SCOPE is_trivially_move_assignable_v<TYPE>
#else
#  define UTL_TRAIT_is_trivially_move_assignable(TYPE) \
      UTL_SCOPE is_trivially_move_assignable<TYPE>::value
#endif
