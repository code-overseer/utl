// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_nothrow_move_assignable;

#  ifdef UTL_CXX17

using std::is_nothrow_move_assignable_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_move_assignable_v =
    is_nothrow_move_assignable<T, T&&>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_nothrow_move_assignable 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/assignment_traits/utl_is_nothrow_assignable.h"

#  ifdef UTL_BUILTIN_is_nothrow_assignable

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_nothrow_move_assignable : bool_constant<UTL_BUILTIN_is_nothrow_assignable(T, T&&)> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_move_assignable_v =
    UTL_BUILTIN_is_nothrow_assignable(T, T&&);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_move_assignable 1

#  else // ifdef UTL_BUILTIN_is_nothrow_assignable

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_nothrow_move_assignable : is_nothrow_assignable<T, T&&> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_move_assignable_v = is_nothrow_assignable_v<T, T&&>;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_move_assignable UTL_TRAIT_SUPPORTED_is_nothrow_assignable

#  endif // ifdef UTL_BUILTIN_is_nothrow_assignable

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
