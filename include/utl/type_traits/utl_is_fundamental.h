// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_fundamental;

#ifdef UTL_CXX17
using std::is_fundamental_v;
#elif defined(UTL_CXX14)   // ifdef UTL_CXX17
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_fundamental_v = is_fundamental<T>::value;
#endif  // ifdef UTL_CXX17

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_fundamental 1

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

#ifndef UTL_DISABLE_BUILTIN_is_fundamental
#define UTL_DISABLE_BUILTIN_is_fundamental 0
#endif  // ifndef UTL_DISABLE_BUILTIN_is_fundamental

#if UTL_SHOULD_USE_BUILTIN(is_fundamental)
#define UTL_BUILTIN_is_fundamental(...) __is_fundamental(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_fundamental)

#ifdef UTL_BUILTIN_is_fundamental

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_fundamental : bool_constant<UTL_BUILTIN_is_fundamental(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_fundamental_v = UTL_BUILTIN_is_fundamental(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_fundamental 1

#else   // ifdef UTL_BUILTIN_is_fundamental

#include "utl/type_traits/utl_is_arithmetic.h"
#include "utl/type_traits/utl_is_null_pointer.h"
#include "utl/type_traits/utl_is_void.h"

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_fundamental : bool_constant<is_void<T>::value || is_null_pointer<T>::value || is_arithmetic<T>::value> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_fundamental_v = is_fundamental<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_fundamental UTL_TRAIT_SUPPORTED_is_void && \
    UTL_TRAIT_SUPPORTED_is_null_pointer && \
    UTL_TRAIT_SUPPORTED_is_arithmetic

#endif  // ifdef UTL_BUILTIN_is_fundamental

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS
