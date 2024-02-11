// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::rank;

UTL_NAMESPACE_END

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

/* (31.01.2024) Clang's __array_rank has a bug so disable for now [https://llvm.org/PR57133] */
#ifdef UTL_DISABLE_BUILTIN_array_rank
UTL_PRAGMA_WARN("builtin array_rank is disabled by default and cannot be enabled")
#undef UTL_DISABLE_BUILTIN_array_rank
#endif
#define UTL_DISABLE_BUILTIN_array_rank 1

#if UTL_SHOULD_USE_BUILTIN(array_rank)
#define UTL_BUILTIN_array_rank(...) __array_rank(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(array_rank) && 0

#ifdef UTL_BUILTIN_array_rank

UTL_NAMESPACE_BEGIN

template<typename T>
struct rank : size_constant<UTL_BUILTIN_array_rank(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr size_t rank_v = UTL_BUILTIN_array_rank(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#else   // ifdef UTL_BUILTIN_array_rank

UTL_NAMESPACE_BEGIN

template<typename T>
struct rank : size_constant<0> {};
template<typename T>
struct rank<T[]> : size_constant<rank<T>::value + 1> {};
template<typename T, size_t N>
struct rank<T[N]> : size_constant<rank<T>::value + 1> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr size_t rank_v = rank<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_array_rank

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_rank 1
