// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_integral;

#  if UTL_CXX17
using std::is_integral_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_integral_v = is_integral<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_integral)
#    define UTL_BUILTIN_is_integral(...) __is_integral(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_integral)

#  ifdef UTL_BUILTIN_is_integral

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_integral : bool_constant<UTL_BUILTIN_is_integral(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_integral_v = UTL_BUILTIN_is_integral(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_integral

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_integral : false_type {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<bool> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<char> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<signed char> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<unsigned char> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<wchar_t> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<char16_t> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<char32_t> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<short> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<unsigned short> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<int> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<unsigned int> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<long> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<unsigned long> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<long long> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<unsigned long long> : true_type {};

#    if UTL_SUPPORTS_CHAR8_T
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<char8_t> : true_type {};
#    endif

#    ifdef UTL_SUPPORTS_INT128
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<__int128_t> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_integral<__uint128_t> : true_type {};
#    endif

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_integral<T const> : is_integral<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_integral<T volatile> : is_integral<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_integral<T const volatile> : is_integral<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_integral_v = is_integral<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_integral

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_integral 1

#ifdef UTL_BUILTIN_is_integral
#  define UTL_TRAIT_is_integral(...) UTL_BUILTIN_is_integral(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_integral(...) UTL_SCOPE is_integral_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_integral(...) UTL_SCOPE is_integral<__VA_ARGS__>::value
#endif
