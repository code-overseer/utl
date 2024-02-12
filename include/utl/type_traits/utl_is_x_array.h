// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_array;

#ifdef UTL_CXX17
using std::is_array_v;
#elif defined(UTL_CXX14)   // ifdef UTL_CXX17
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_array_v = is_array<T>::value;
#endif  // ifdef UTL_CXX17

UTL_NAMESPACE_END

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

/** (31.01.2024) Clang's __is_array has a bug so disable for now [https://llvm.org/PR54705];
 * maybe just disable any compiler with Clang frontend? */
#ifdef UTL_DISABLE_BUILTIN_is_array
UTL_PRAGMA_WARN("builtin is_array is disabled by default and cannot be enabled")
#undef UTL_DISABLE_BUILTIN_is_array
#endif

#define UTL_DISABLE_BUILTIN_is_array 1
#if UTL_SHOULD_USE_BUILTIN(is_array)
#define UTL_BUILTIN_is_array(...) __is_array(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_array)

#ifdef UTL_BUILTIN_is_array

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_array : bool_constant<UTL_BUILTIN_is_array(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_array_v = UTL_BUILTIN_is_array(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#else   // ifdef UTL_BUILTIN_is_array

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_array : false_type {};

template<typename T>
struct is_array<T[]> : true_type {};

template<typename T, size_t N>
struct is_array<T[N]> : true_type {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_array_v = is_array<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_is_array

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_array 1

#if defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX20)

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_bounded_array;
using std::is_unbounded_array;

#ifdef UTL_CXX17

using std::is_bounded_array_v;
using std::is_unbounded_array_v;

#elif defined(UTL_CXX14)   // ifdef UTL_CXX17

template<typename T>
UTL_INLINE_CXX17 constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

#endif  // ifdef UTL_CXX17

UTL_NAMESPACE_END

#else   // defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX20)

#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_bounded_array : false_type {};
template<typename T, decltype(sizeof(0)) N>
struct is_bounded_array<T[N]> : true_type {};

template<typename T>
struct is_unbounded_array : false_type {};
template<typename T>
struct is_unbounded_array<T[]> : true_type {};

UTL_NAMESPACE_END

#endif  // defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX20)

#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_bounded_array_v = is_bounded_array<T>::value;

template<typename T>
UTL_INLINE_CXX17 constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_bounded_array 1
#define UTL_TRAIT_SUPPORTED_is_unbounded_array 1
