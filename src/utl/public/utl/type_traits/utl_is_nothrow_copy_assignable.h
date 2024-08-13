// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_nothrow_copy_assignable;

#  if UTL_CXX17

using std::is_nothrow_copy_assignable_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_nothrow_copy_assignable 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_add_lvalue_reference.h"
#  include "utl/type_traits/utl_is_nothrow_assignable.h"

#  ifdef UTL_BUILTIN_is_nothrow_assignable

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_copy_assignable :
    bool_constant<UTL_BUILTIN_is_nothrow_assignable(
        add_lvalue_reference_t<T>, add_lvalue_reference_t<T const>)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_copy_assignable_v =
    UTL_BUILTIN_is_nothrow_assignable(add_lvalue_reference_t<T>, add_lvalue_reference_t<T const>);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_copy_assignable 1

#  else // ifdef UTL_BUILTIN_is_nothrow_assignable

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_copy_assignable :
    is_nothrow_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<T const>> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_copy_assignable_v =
    is_nothrow_assignable_v<add_lvalue_reference_t<T>, add_lvalue_reference_t<T const>>;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_copy_assignable UTL_TRAIT_SUPPORTED_is_nothrow_assignable

#  endif // ifdef UTL_BUILTIN_is_nothrow_assignable

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#if UTL_CXX14
#  define UTL_TRAIT_is_nothrow_copy_assignable(TYPE) __UTL is_nothrow_copy_assignable_v<TYPE>
#else
#  define UTL_TRAIT_is_nothrow_copy_assignable(TYPE) __UTL is_nothrow_copy_assignable<TYPE>::value
#endif
