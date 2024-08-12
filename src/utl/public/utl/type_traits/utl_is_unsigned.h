// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_unsigned;

#  if UTL_CXX17

using std::is_unsigned_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unsigned_v = is_unsigned<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_unsigned)
#    define UTL_BUILTIN_is_unsigned(...) __is_unsigned(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_unsigned)

#  ifdef UTL_BUILTIN_is_unsigned

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_unsigned : bool_constant<UTL_BUILTIN_is_unsigned(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unsigned_v = UTL_BUILTIN_is_unsigned(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_unsigned 1

#  else // ifdef UTL_BUILTIN_is_unsigned

#    include "utl/type_traits/utl_is_arithmetic.h"
#    include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_unsigned :
    conjunction<is_arithmetic<T>, bool_constant<(T(-1) > T(0))>> {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_unsigned<bool> : true_type {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unsigned_v = is_unsigned<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_unsigned UTL_TRAIT_SUPPORTED_is_arithmetic

#  endif // ifdef UTL_BUILTIN_is_unsigned

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_unsigned
#  define UTL_TRAIT_is_unsigned(...) UTL_BUILTIN_is_unsigned(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_unsigned(...) __UTL is_unsigned_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_unsigned(...) __UTL is_unsigned<__VA_ARGS__>::value
#endif
