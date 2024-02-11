// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_pointer;

#ifdef UTL_CXX17
using std::is_pointer_v;
#elif defined(UTL_CXX14)   // ifdef UTL_CXX17
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_pointer_v = is_pointer<T>::value;
#endif  // ifdef UTL_CXX17

UTL_NAMESPACE_END

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

#ifndef UTL_DISABLE_BUILTIN_is_pointer
#define UTL_DISABLE_BUILTIN_is_pointer 0
#endif

#if UTL_SHOULD_USE_BUILTIN(is_pointer)
#define UTL_BUILTIN_is_pointer(...) __is_pointer(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_pointer)

#ifdef UTL_BUILTIN_is_pointer

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_pointer : bool_constant<UTL_BUILTIN_is_pointer(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_pointer_v = UTL_BUILTIN_is_pointer(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#else   // ifdef UTL_BUILTIN_is_pointer

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_pointer : false_type {};

template<typename T>
struct is_pointer<T*> : true_type {};

template<>
struct is_pointer<T* const> : true_type {};

template<>
struct is_pointer<T* volatile> : true_type {};

template<>
struct is_pointer<T* const volatile> : true_type {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_pointer_v = is_pointer<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_is_pointer

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_pointer 1
