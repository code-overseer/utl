// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_const;
using std::is_volatile;

#  ifdef UTL_CXX17

using std::is_const_v;
using std::is_volatile_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_const_v = is_const<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_volatile_v = is_volatile<T>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_const 1
#  define UTL_TRAIT_SUPPORTED_is_volatile 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_const
#    define UTL_DISABLE_BUILTIN_is_const 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_const
#  ifndef UTL_DISABLE_BUILTIN_is_volatile
#    define UTL_DISABLE_BUILTIN_is_volatile 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_volatile

#  if UTL_SHOULD_USE_BUILTIN(is_const)
#    define UTL_BUILTIN_is_const(...) __is_const(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_const)

#  if UTL_SHOULD_USE_BUILTIN(is_volatile)
#    define UTL_BUILTIN_is_volatile(...) __is_volatile(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_volatile)

#  ifdef UTL_BUILTIN_is_const

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_const : bool_constant<UTL_BUILTIN_is_const(T)> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_const_v = UTL_BUILTIN_is_const(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_const

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_const : false_type {};
template <typename T>
struct is_const<T const> : true_type {};
template <typename T>
struct is_const<T const volatile> : true_type {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_const_v = is_const<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_const

#  ifdef UTL_BUILTIN_is_volatile

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_volatile : bool_constant<UTL_BUILTIN_is_volatile(T)> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_volatile_v = UTL_BUILTIN_is_volatile(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_volatile

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_volatile : false_type {};
template <typename T>
struct is_volatile<T volatile> : true_type {};
template <typename T>
struct is_volatile<T const volatile> : true_type {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_volatile_v = is_volatile<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_volatile

#  define UTL_TRAIT_SUPPORTED_is_const 1
#  define UTL_TRAIT_SUPPORTED_is_volatile 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
