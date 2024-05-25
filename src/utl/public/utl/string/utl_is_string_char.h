// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

template <typename>
struct is_string_char : false_type {};
template <typename T>
struct is_string_char<T const> : is_string_char<T> {};
template <typename T>
struct is_string_char<T volatile> : is_string_char<T> {};
template <typename T>
struct is_string_char<T const volatile> : is_string_char<T> {};

template <>
struct is_string_char<char> : true_type {};
template <>
struct is_string_char<wchar_t> : true_type {};
template <>
struct is_string_char<char16_t> : true_type {};
template <>
struct is_string_char<char32_t> : true_type {};

#if UTL_SUPPORTS_CHAR8_T
template <>
struct is_string_char<char8_t> : true_type {};
#endif
#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_string_char_v = is_string_char<T>::value;
#endif

#if UTL_CXX20
template <typename T>
concept string_char = is_string_char_v<T>;
#endif

UTL_NAMESPACE_END
