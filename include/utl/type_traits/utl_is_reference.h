// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_reference;
using std::is_lvalue_reference;
using std::is_rvalue_reference;

UTL_NAMESPACE_END

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

#ifndef UTL_DISABLE_BUILTIN_is_reference
#define UTL_DISABLE_BUILTIN_is_reference 0
#endif  // ifndef UTL_DISABLE_BUILTIN_is_reference

#ifndef UTL_DISABLE_BUILTIN_is_lvalue_reference
#define UTL_DISABLE_BUILTIN_is_lvalue_reference 0
#endif  // ifndef UTL_DISABLE_BUILTIN_is_lvalue_reference

#ifndef UTL_DISABLE_BUILTIN_is_rvalue_reference
#define UTL_DISABLE_BUILTIN_is_rvalue_reference 0
#endif  // ifndef UTL_DISABLE_BUILTIN_is_rvalue_reference

#if UTL_SHOULD_USE_BUILTIN(is_reference)
#define UTL_BUILTIN_is_reference(...) __is_reference(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_reference)

#if UTL_SHOULD_USE_BUILTIN(is_lvalue_reference)
#define UTL_BUILTIN_is_lvalue_reference(...) __is_lvalue_reference(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_lvalue_reference)

#if UTL_SHOULD_USE_BUILTIN(is_rvalue_reference)
#define UTL_BUILTIN_is_rvalue_reference(...) __is_rvalue_reference(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_rvalue_reference)

#ifdef UTL_BUILTIN_is_reference

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_reference : bool_constant<UTL_BUILTIN_is_reference(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_reference_v = UTL_BUILTIN_is_reference(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#else   // ifdef UTL_BUILTIN_is_reference

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_reference : false_type {};

template<typename T>
struct is_reference<T&> : true_type {};

template<typename T>
struct is_reference<T&&> : true_type {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_reference_v = is_reference<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_is_reference

#ifdef UTL_BUILTIN_is_lvalue_reference

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_lvalue_reference : bool_constant<UTL_BUILTIN_is_lvalue_reference(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_lvalue_reference_v = UTL_BUILTIN_is_lvalue_reference(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#else   // ifdef UTL_BUILTIN_is_lvalue_reference

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_lvalue_reference : false_type {};

template<typename T>
struct is_lvalue_reference<T&> : true_type {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_is_lvalue_reference

#ifdef UTL_BUILTIN_is_rvalue_reference

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_rvalue_reference : bool_constant<UTL_BUILTIN_is_rvalue_reference(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_rvalue_reference_v = UTL_BUILTIN_is_rvalue_reference(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#else   // ifdef UTL_BUILTIN_is_rvalue_reference

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_rvalue_reference : false_type {};

template<typename T>
struct is_rvalue_reference<T&&> : true_type {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_is_rvalue_reference

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_reference 1
#define UTL_TRAIT_SUPPORTED_is_rvalue_reference 1
#define UTL_TRAIT_SUPPORTED_is_lvalue_reference 1
