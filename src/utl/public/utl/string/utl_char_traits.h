// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/compare/utl_compare_fwd.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/string/utl_libc.h"

UTL_NAMESPACE_BEGIN
using size_t = decltype(sizeof(0));

template <>
struct char_traits<char> {
    using char_type = char;
    using pointer = char_type*;
    using const_pointer = char_type const*;
    using int_type = int;
    using off_type = decltype((pointer)0 - (pointer)0);
    using pos_type = size_t;
    using state_type = mbstate_t;
#ifdef UTL_CXX20
    using comparison_category = strong_ordering;
#endif

    // constexpr_memcmp requires a trivially lexicographically comparable type, but char is not
    // when char is a signed type
    UTL_ATTRIBUTES(NODISCARD, PURE)
    static constexpr int compare(
        char_type const* lhs, char_type const* rhs, size_t length) noexcept {
        return libc::memcmp(lhs, rhs, length);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    static inline constexpr size_t length(char_type const* str) noexcept {
        return __builtin_strlen(str);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    static inline constexpr char_type const* find(
        char_type const* str, size_t length, char_type const ch) noexcept {
        if (length == 0)
            return nullptr;
        return __builtin_memchr(str, length, ch);
    }

    static inline constexpr char_type* move(
        char_type* s1, char_type const* s2, size_t length) noexcept {
        return __builtin_memmove(s1, s2, length);
    }

    static inline constexpr char_type* copy(
        char_type* UTL_RESTRICT dst, char_type const* UTL_RESTRICT src, size_t length) noexcept {
        __builtin_memcpy(dst, src, length)[length] = char_type();
        return dst;
    }

    static inline constexpr char_type* assign(
        char_type* str, size_t length, char_type ch) noexcept {
        __builtin_memset(str, length, ch);
        return str;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST)
    static inline constexpr int_type not_eof(int_type c) noexcept {
        return eq_int_type(c, eof()) ? ~eof() : c;
    }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    static inline constexpr char_type to_char_type(int_type c) noexcept { return char_type(c); }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    static inline constexpr int_type to_int_type(char_type c) noexcept {
        return int_type((unsigned char)c);
    }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    static inline constexpr bool eq_int_type(int_type c1, int_type c2) noexcept { return c1 == c2; }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    static inline constexpr int_type eof() noexcept { return int_type(EOF); }
};

UTL_NAMESPACE_END