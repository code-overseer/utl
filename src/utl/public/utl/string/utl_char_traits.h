// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/compare/utl_compare_fwd.h"

#include "utl/string/utl_libc.h"

#include <cwchar>

UTL_NAMESPACE_BEGIN
#define __UTL_ATTRIBUTE_PURE_API (PURE)(NODISCARD)(ALWAYS_INLINE)__UTL_ATTRIBUTE_HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_PURE_API
#define __UTL_ATTRIBUTE_CONST_API (CONST)(NODISCARD)(ALWAYS_INLINE)__UTL_ATTRIBUTE_HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_CONST_API

template <typename T>
struct UTL_PUBLIC_TEMPLATE char_traits {
    static_assert(is_string_char<T>::value, "Invalid character type");
    using char_type = T;
    using pointer = char_type*;
    using const_pointer = char_type const*;
    using int_type = int;
    using off_type = decltype((pointer)0 - (pointer)0);
    using pos_type = size_t;
    using state_type = mbstate_t;
#if UTL_CXX20
    using comparison_category = strong_ordering;
#endif

    UTL_ATTRIBUTE(PURE_API) static inline constexpr int compare(
        char_type const* lhs, char_type const* rhs, size_t length) noexcept {
        return libc::strncmp(lhs, rhs, libc::element_count_t(length));
    }

    UTL_ATTRIBUTE(PURE_API) static inline constexpr size_t length(char_type const* str) noexcept {
        return libc::strlen(str);
    }

    UTL_ATTRIBUTE(PURE_API) static inline constexpr char_type const* find(
        char_type const* str, size_t length, char_type const ch) noexcept {
        return libc::strnchr(str, ch, libc::element_count_t(length));
    }

    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr char_type* move(
        char_type* s1, char_type const* s2, size_t length) noexcept {
        return libc::memmove(s1, s2, libc::element_count_t(length));
    }

    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr char_type* copy(
        char_type* UTL_RESTRICT dst, char_type const* UTL_RESTRICT src, size_t length) noexcept {
        return libc::memcpy(dst, src, libc::element_count_t(length));
    }

    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr char_type* assign(
        char_type* str, size_t length, char_type ch) noexcept {
        return libc::strnset(str, ch, libc::element_count_t(length));
    }

    UTL_ATTRIBUTE(CONST_API) static inline constexpr int_type not_eof(int_type c) noexcept {
        return eq_int_type(c, eof()) ? ~eof() : c;
    }
    UTL_ATTRIBUTE(CONST_API) static inline constexpr char_type to_char_type(int_type c) noexcept {
        return char_type(c);
    }
    UTL_ATTRIBUTE(CONST_API) static inline constexpr int_type to_int_type(char_type c) noexcept {
        return int_type(as_unsigned(c));
    }
    UTL_ATTRIBUTE(CONST_API) static inline constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
        return c1 == c2;
    }
    UTL_ATTRIBUTE(CONST_API) static inline constexpr int_type eof() noexcept { return int_type(EOF); }
    UTL_ATTRIBUTE(CONST_API) static inline constexpr bool eq(char_type l, char_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTE(CONST_API) static inline constexpr bool lt(char_type l, char_type r) noexcept {
        return l < r;
    }
};

template <>
UTL_ATTRIBUTE(CONST_API) inline constexpr bool char_traits<char>::eq(char_type l, char_type r) noexcept {
    return static_cast<unsigned char>(l) == static_cast<unsigned char>(r);
}

template <>
UTL_ATTRIBUTE(CONST_API) inline constexpr bool char_traits<char>::lt(char_type l, char_type r) noexcept {
    return static_cast<unsigned char>(l) < static_cast<unsigned char>(r);
}

template <typename T>
struct char_traits<T const> {};
template <typename T>
struct char_traits<T volatile> {};

#undef __UTL_ATTRIBUTE_PURE_API
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_PURE_API
#undef __UTL_ATTRIBUTE_CONST_API
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_CONST_API
UTL_NAMESPACE_END