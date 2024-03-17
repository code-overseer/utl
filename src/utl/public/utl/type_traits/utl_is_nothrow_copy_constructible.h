// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_nothrow_copy_constructible;

#  ifdef UTL_CXX17

using std::is_nothrow_copy_constructible_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_copy_constructible_v =
    is_nothrow_copy_constructible<T, T const&>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_nothrow_copy_constructible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_is_nothrow_constructible.h"

#  ifdef UTL_BUILTIN_is_nothrow_constructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_nothrow_copy_constructible :
    bool_constant<UTL_BUILTIN_is_nothrow_constructible(T, T const&)> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_copy_constructible_v =
    UTL_BUILTIN_is_nothrow_constructible(T, T const&);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_copy_constructible 1

#  else // ifdef UTL_BUILTIN_is_nothrow_constructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_nothrow_copy_constructible : is_nothrow_constructible<T, T const&> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_copy_constructible_v =
    is_nothrow_constructible_v<T, T const&>;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_copy_constructible \
        UTL_TRAIT_SUPPORTED_is_nothrow_constructible

#  endif // ifdef UTL_BUILTIN_is_nothrow_constructible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
