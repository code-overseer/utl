// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_enum;

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_enum 1

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

#ifndef UTL_DISABLE_BUILTIN_is_enum
#define UTL_DISABLE_BUILTIN_is_enum 0
#endif  // ifndef UTL_DISABLE_BUILTIN_is_enum

#if UTL_SHOULD_USE_BUILTIN(is_enum)
#define UTL_BUILTIN_is_enum(...) __is_enum(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_enum)

#ifdef UTL_BUILTIN_is_enum

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_enum : bool_constant<UTL_BUILTIN_is_enum(T)> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_enum_v = UTL_BUILTIN_is_enum(T);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_enum 1

#else   // ifdef UTL_BUILTIN_is_enum

#include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template<typename T>
struct is_enum : undefined_trait<T> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_enum_v = is_enum<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_is_enum



#endif  // ifdef UTL_USE_STD_TYPE_TRAITS


