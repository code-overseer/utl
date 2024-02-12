// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_object;

#ifdef UTL_CXX17
using std::is_object_v;
#elif defined(UTL_CXX14)   // ifdef UTL_CXX17
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_object_v = is_object<T>::value;
#endif  // ifdef UTL_CXX17

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_object 1

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

#ifndef UTL_DISABLE_BUILTIN_is_object
#define UTL_DISABLE_BUILTIN_is_object 0
#endif  // ifndef UTL_DISABLE_BUILTIN_is_object

#if UTL_SHOULD_USE_BUILTIN(is_object)
#define UTL_BUILTIN_is_object(...) __is_object(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_object)

#ifdef UTL_BUILTIN_is_object

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_object : bool_constant<UTL_BUILTIN_is_object(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_object_v = UTL_BUILTIN_is_object(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_object 1

#else   // ifdef UTL_BUILTIN_is_object

#include "utl/type_traits/utl_is_xarray.h"
#include "utl/type_traits/utl_is_class.h"
#include "utl/type_traits/utl_is_scalar.h"
#include "utl/type_traits/utl_is_union.h"

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_object : bool_constant<is_scalar<T>::value || is_array<T>::value || is_union<T>::value || is_class<T>::value> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_object_v = is_object<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_object UTL_TRAIT_SUPPORTED_is_class && \
    UTL_TRAIT_SUPPORTED_is_union && \
    UTL_TRAIT_SUPPORTED_is_scalar && \
    UTL_TRAIT_SUPPORTED_is_array

#endif  // ifdef UTL_BUILTIN_is_object

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS
