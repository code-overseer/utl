// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_integral;

#ifdef UTL_CXX17
using std::is_integral_v;
#elif defined(UTL_CXX14)   // ifdef UTL_CXX17
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_integral_v = is_integral<T>::value;
#endif  // ifdef UTL_CXX17

UTL_NAMESPACE_END

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

#ifndef UTL_DISABLE_BUILTIN_is_integral
#define UTL_DISABLE_BUILTIN_is_integral 0
#endif  // ifndef UTL_DISABLE_BUILTIN_is_integral

#if UTL_SHOULD_USE_BUILTIN(is_integral)
#define UTL_BUILTIN_is_integral(...) __is_integral(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_integral)

#ifdef UTL_BUILTIN_is_integral

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_integral : bool_constant<UTL_BUILTIN_is_integral(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_integral_v = UTL_BUILTIN_is_integral(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#else   // ifdef UTL_BUILTIN_is_integral

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_integral : false_type {};

template <> struct is_integral<bool>: true_type {};
template <> struct is_integral<char>: true_type {};
template <> struct is_integral<signed char>: true_type {};
template <> struct is_integral<unsigned char>: true_type {};
template <> struct is_integral<wchar_t>: true_type {};
template <> struct is_integral<char16_t>: true_type {};
template <> struct is_integral<char32_t>: true_type {};
template <> struct is_integral<short>: true_type {};
template <> struct is_integral<unsigned short>: true_type {};
template <> struct is_integral<int>: true_type {};
template <> struct is_integral<unsigned int>: true_type {};
template <> struct is_integral<long>: true_type {};
template <> struct is_integral<unsigned long>: true_type {};
template <> struct is_integral<long long>: true_type {};
template <> struct is_integral<unsigned long long>: true_type {};

#if defined(UTL_CXX20)
template <> struct is_integral<char8_t>: true_type {};
#endif

#if defined(UTL_SUPPORTS_INT128) && !defined(UTL_DISABLE_INT128_SUPPORT)
template <> struct is_integral<__int128_t>: true_type {};
template <> struct is_integral<__uint128_t>: true_type {};
#endif

template<typename T>
struct is_integral<T const> : is_integral<T> {};
template<typename T>
struct is_integral<T volatile> : is_integral<T> {};
template<typename T>
struct is_integral<T const volatile> : is_integral<T> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_integral_v = is_integral<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_is_integral

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_integral 1
