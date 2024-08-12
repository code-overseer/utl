// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_boolean : false_type {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_boolean<bool> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_boolean<bool const> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_boolean<bool volatile> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_boolean<bool const volatile> : true_type {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_boolean_v = false;
template <>
UTL_INLINE_CXX17 constexpr bool is_boolean_v<bool> = true;
template <>
UTL_INLINE_CXX17 constexpr bool is_boolean_v<bool const> = true;
template <>
UTL_INLINE_CXX17 constexpr bool is_boolean_v<bool volatile> = true;
template <>
UTL_INLINE_CXX17 constexpr bool is_boolean_v<bool const volatile> = true;
#endif

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_boolean(...) UTL_SCOPE is_boolean_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_boolean(...) UTL_SCOPE is_boolean<__VA_ARGS__>::value
#endif
