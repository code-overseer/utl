// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_scalar;

#  if UTL_CXX17
using std::is_scalar_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_scalar_v = is_scalar<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_scalar 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_scalar)
#    define UTL_BUILTIN_is_scalar(...) __is_scalar(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_scalar)

#  ifdef UTL_BUILTIN_is_scalar

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_scalar : bool_constant<UTL_BUILTIN_is_scalar(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_scalar_v = UTL_BUILTIN_is_scalar(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_scalar 1

#  else // ifdef UTL_BUILTIN_is_scalar

#    include "utl/type_traits/utl_is_arithmetic.h"
#    include "utl/type_traits/utl_is_enum.h"
#    include "utl/type_traits/utl_is_member_pointer.h"
#    include "utl/type_traits/utl_is_null_pointer.h"
#    include "utl/type_traits/utl_is_pointer.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_scalar :
    bool_constant<UTL_TRAIT_is_arithmetic(T) || UTL_TRAIT_is_member_pointer(T) ||
        UTL_TRAIT_is_pointer(T) || UTL_TRAIT_is_null_pointer(T) || UTL_TRAIT_is_enum(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_scalar_v = is_scalar<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_scalar                                                 \
        UTL_TRAIT_SUPPORTED_is_arithmetic&& UTL_TRAIT_SUPPORTED_is_enum&&                 \
            UTL_TRAIT_SUPPORTED_is_member_pointer&& UTL_TRAIT_SUPPORTED_is_null_pointer&& \
                UTL_TRAIT_SUPPORTED_is_pointer

#  endif // ifdef UTL_BUILTIN_is_scalar

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_scalar
#  define UTL_TRAIT_is_scalar(...) UTL_BUILTIN_is_scalar(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_scalar(...) UTL_SCOPE is_scalar_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_scalar(...) UTL_SCOPE is_scalar<__VA_ARGS__>::value
#endif
