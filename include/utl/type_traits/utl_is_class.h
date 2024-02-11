// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_class;

#ifdef UTL_CXX17
using std::is_class_v;
#elif defined(UTL_CXX14)   // ifdef UTL_CXX17
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_class_v = is_class<T>::value;
#endif  // ifdef UTL_CXX17

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_class 1

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

#ifndef UTL_DISABLE_BUILTIN_is_class
#define UTL_DISABLE_BUILTIN_is_class 0
#endif  // ifndef UTL_DISABLE_BUILTIN_is_class

#if UTL_SHOULD_USE_BUILTIN(is_class)
#define UTL_BUILTIN_is_class(...) __is_class(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_class)

#ifdef UTL_BUILTIN_is_class

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_class : bool_constant<UTL_BUILTIN_is_class(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_class_v = UTL_BUILTIN_is_class(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_class 1

#else   // ifdef UTL_BUILTIN_is_class

#include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_class : undefined_trait<T> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_class_v = is_class<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_is_class

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS
