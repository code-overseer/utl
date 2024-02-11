// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_void;

#ifdef UTL_CXX17
using std::is_void_v;
#elif defined(UTL_CXX14)   // ifdef UTL_CXX17
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_void_v = is_void<T>::value;
#endif  // ifdef UTL_CXX17

UTL_NAMESPACE_END

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

#ifndef UTL_DISABLE_BUILTIN_is_void
#define UTL_DISABLE_BUILTIN_is_void 0
#endif

#if UTL_SHOULD_USE_BUILTIN(is_void)
#define UTL_BUILTIN_is_void(...) __is_void(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_void)

#ifdef UTL_BUILTIN_is_void

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_void : bool_constant<UTL_BUILTIN_is_void(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_void_v = UTL_BUILTIN_is_void(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#else   // ifdef UTL_BUILTIN_is_void

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_void : false_type {};

template<>
struct is_void<void> : true_type {};

template<>
struct is_void<void const> : true_type {};

template<>
struct is_void<void volatile> : true_type {};

template<>
struct is_void<void const volatile> : true_type {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_void_v = is_void<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_is_void

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_void 1
