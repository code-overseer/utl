// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_function;

#ifdef UTL_CXX17
using std::is_function_v;
#elif defined(UTL_CXX14)   // ifdef UTL_CXX17
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_function_v = is_function<T>::value;
#endif  // ifdef UTL_CXX17

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_function 1

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#ifndef UTL_DISABLE_BUILTIN_is_function
#define UTL_DISABLE_BUILTIN_is_function 0
#endif  // ifndef UTL_DISABLE_BUILTIN_is_function

#if UTL_SHOULD_USE_BUILTIN(is_function)
#define UTL_BUILTIN_is_function(...) __is_function(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_function)

#ifdef UTL_BUILTIN_is_function

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_function : bool_constant<UTL_BUILTIN_is_function(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_function_v = UTL_BUILTIN_is_function(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_function 1

#else   // ifdef UTL_BUILTIN_is_function

#include "utl/type_traits/utl_is_const.h"
#include "utl/type_traits/utl_is_reference.h"

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_function : bool_constant<!is_const<const T>::value && !is_reference<T>::value> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_function_v = is_function<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_function UTL_TRAIT_SUPPORTED_is_const && UTL_TRAIT_SUPPORTED_is_reference

#endif  // ifdef UTL_BUILTIN_is_function

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS
