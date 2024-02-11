// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_scalar;

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_scalar 1

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

#ifndef UTL_DISABLE_BUILTIN_is_scalar
#define UTL_DISABLE_BUILTIN_is_scalar 0
#endif

#if UTL_SHOULD_USE_BUILTIN(is_scalar)
#define UTL_BUILTIN_is_scalar(...) __is_scalar(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_scalar)

#ifdef UTL_BUILTIN_is_scalar

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_scalar : bool_constant<UTL_BUILTIN_is_scalar(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_scalar_v = UTL_BUILTIN_is_scalar(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_scalar 1

#else   // ifdef UTL_BUILTIN_is_scalar

#include "utl/type_traits/utl_is_arithmetic.h"
#include "utl/type_traits/utl_is_enum.h"
#include "utl/type_traits/utl_is_member_pointer.h"
#include "utl/type_traits/utl_is_null_pointer.h"
#include "utl/type_traits/utl_is_pointer.h"

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_scalar : bool_constant<is_arithmetic<T>::value ||
                is_member_pointer<T>::value ||
                is_pointer<T>::value ||
                is_null_pointer<T>::value ||
                is_enum<T>::value>> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_scalar_v = is_scalar<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_scalar UTL_TRAIT_SUPPORTED_is_arithmetic && \
    UTL_TRAIT_SUPPORTED_is_enum && \
    UTL_TRAIT_SUPPORTED_is_member_pointer && \
    UTL_TRAIT_SUPPORTED_is_null_pointer && \
    UTL_TRAIT_SUPPORTED_is_pointer

#endif  // ifdef UTL_BUILTIN_is_scalar

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS


