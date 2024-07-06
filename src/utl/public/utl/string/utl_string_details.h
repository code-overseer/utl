// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/algorithm/utl_min.h"
#include "utl/string/utl_char_traits.h"
#include "utl/string/utl_libc.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace string {

UTL_ATTRIBUTES(NODISCARD, CONST) constexpr int negative_if_true(bool b) noexcept {
    return -+b | 1;
}

UTL_ATTRIBUTES(NODISCARD, CONST) constexpr int other_if_zero(int x, int other) noexcept {
    return x ? x : other;
}

UTL_ATTRIBUTES(NODISCARD, CONST) constexpr int value_if_true(bool b, int value) noexcept {
    return b ? value : 0;
}

UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr size_t saturate_min(size_t result, size_t left) noexcept {
    return result & -(result <= left);
}

UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr size_t saturate_max(size_t result, size_t left) noexcept {
    return result | -(result < left);
}

UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr size_t saturated_subtract(size_t left, size_t right) noexcept {
    return saturate_min(left - right, left);
}

UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr size_t saturated_add(size_t left, size_t right) noexcept {
    return saturate_max(left + right, left);
}

UTL_INLINE_CXX17 constexpr size_t npos = (size_t)-1;
using UTL_SCOPE libc::element_count_t;
namespace compile_time {

template <typename T>
UTL_CONSTEVAL T* rfind_char(T const* str, T const ch, T const* org) noexcept {
    return *str == ch ? const_cast<T*>(str)
        : str == org  ? nullptr
                      : find_char_reverse(str - 1, ch, org);
}

template <typename T>
UTL_CONSTEVAL T* rfind_char(T const* str, T const ch, element_count_t len) noexcept {
    return str_rfind_char((str + len) - 1, ch, str);
}

template <typename Traits, typename T>
UTL_CONSTEVAL T* find_first_of(
    T const* str, element_count_t len, T const* chars, element_count_t char_count) noexcept {
    return len == 0 ? nullptr
        : Traits::find(chars, size_t(char_count), *str)
        ? str
        : find_first_of<Traits>(str + 1, len - 1, chars, char_count);
}

template <typename Traits, typename T>
UTL_CONSTEVAL T* find_first_not_of(
    T const* str, element_count_t len, T const* chars, element_count_t char_count) noexcept {
    return len == 0 ? nullptr
        : !Traits::find(chars, size_t(char_count), *str)
        ? str
        : find_first_not_of<Traits>(str + 1, len - 1, chars, char_count);
}

template <typename Traits, typename T>
UTL_CONSTEVAL T* find_last_of(
    T const* str, T const* current, T const* chars, element_count_t char_count) noexcept {
    return current < str ? nullptr
        : Traits::find(chars, size_t(char_count), *current)
        ? current
        : find_first_of<Traits>(str, current - 1, chars, char_count);
}

template <typename Traits, typename T>
UTL_CONSTEVAL T* find_last_not_of(
    T const* str, T const* current, T const* chars, element_count_t char_count) noexcept {
    return current < str ? nullptr
        : !Traits::find(chars, size_t(char_count), *current)
        ? current
        : find_first_of<Traits>(str, current - 1, chars, char_count);
}
} // namespace compile_time

namespace runtime {
template <typename T>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
T* rfind_char(T const* str, T const ch, element_count_t count) noexcept {
    size_t len = size_t(count);
    for (auto p = str + len - 1; p >= str; --p) {
        if (*p == ch) {
            return const_cast<T*>(str);
        }
    }

    return nullptr;
}

template <typename Traits, typename T>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
T* find_first_of(
    T const* str, element_count_t len, T const* chars, element_count_t char_count) noexcept {
    auto const chars_len = size_t(char_count);
    while ((size_t)len) {
        if (Traits::find(chars, chars_len, *str) != nullptr) {
            return str;
        }

        len = len - 1;
        ++str;
    }

    return nullptr;
}

template <typename Traits, typename T>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
T* find_first_not_of(
    T const* str, element_count_t len, T const* chars, element_count_t char_count) noexcept {
    auto const chars_len = size_t(char_count);
    while ((size_t)len) {
        if (Traits::find(chars, chars_len, *str) == nullptr) {
            return str;
        }

        len = len - 1;
        ++str;
    }

    return nullptr;
}

template <typename Traits, typename T>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
T* find_last_of(
    T const* str, T const* current, T const* chars, element_count_t char_count) noexcept {
    while (current >= str) {
        if (Traits::find(chars, size_t(char_count), *current)) {
            return current;
        }

        --current;
    }

    return nullptr;
}

template <typename Traits, typename T>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
T* find_last_not_of(
    T const* str, T const* current, T const* chars, element_count_t char_count) noexcept {
    while (current >= str) {
        if (!Traits::find(chars, size_t(char_count), *current)) {
            return current;
        }

        --current;
    }

    return nullptr;
}
} // namespace runtime

template <typename Traits, typename CharType>
UTL_ATTRIBUTES(NODISCARD, PURE)
constexpr CharType* find(
    CharType const* str, size_t length, CharType const ch, size_t pos) noexcept {
    return Traits::find(
        str + UTL_SCOPE numeric_min(length, pos), saturated_subtract(length, pos), ch);
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTES(NODISCARD, PURE)
inline UTL_CONSTEXPR_CXX14 CharType const* search_substring(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count) noexcept {
    if (r_count == 0) {
        return l;
    }
    auto const l_last = l + l_count;

    if (l_count < r_count) {
        return l_last;
    }

    for (auto const r_char = *r;; ++l) {
        l_count = l_last - l;

        if (l_count < r_count) {
            return l + l_count;
        }

        l = Traits::find(l, l_count - r_count + 1, r_char);
        if (l == nullptr) {
            return l_last;
        }

        if (Traits::compare(l, r, r_count) == 0) {
            return l;
        }

        ++l;
    }
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTES(NODISCARD, PURE)
inline UTL_CONSTEXPR_CXX14 CharType const* find(CharType const* l, size_t l_count, CharType const* r,
    size_t r_count, size_t l_pos = 0) noexcept {
    if (l_pos > l_count) {
        return nullptr;
    }
    auto const result = search_substring<Traits>(l + l_pos, l_count - l_pos, r, r_count);
    if (result == l + l_count) {
        return nullptr;
    }

    return result;
}

template <typename CharType>
UTL_ATTRIBUTES(NODISCARD, PURE)
constexpr CharType* rfind(CharType const* str, size_t length, CharType const ch) noexcept {
    return UTL_CONSTANT_P(str[length - 1] == ch)
        ? compile_time::rfind_char(str, ch, element_count_t(length))
        : runtime::rfind_char(str, ch, element_count_t(length));
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTES(NODISCARD, PURE)
constexpr CharType* rfind(
    CharType const* str, size_t length, CharType const ch, size_t pos) noexcept {
    return rfind(str, UTL_SCOPE numeric_min(length, saturated_add(pos, 1)), ch);
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTES(NODISCARD, PURE)
inline UTL_CONSTEXPR_CXX14 CharType const* rsearch_substring(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count) noexcept {
    if (r_count == 0) {
        return nullptr;
    }
    auto const r_last = r[r_count - 1];
    while (true) {
        if (l_count < r_count) {
            return nullptr;
        }

        auto const l_last = rfind(l, l_count, r_last);
        if (l_last == nullptr || l_last - l + 1 < r_count) {
            return nullptr;
        }

        auto const l_begin = l_last + 1 - r_count;
        if (Traits::compare(l_begin, r, r_count) == 0) {
            return l_begin;
        }

        l_count = l_last - l;
    }
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTES(NODISCARD, PURE)
inline UTL_CONSTEXPR_CXX14 CharType const* rfind(CharType const* const l, size_t l_count,
    CharType const* const r, size_t const r_count, size_t l_pos) noexcept {
    l_count = UTL_SCOPE numeric_min(saturated_add(l_pos, 1), l_count);
    return rsearch_substring<Traits>(l, l_count, r, r_count);
}

template <typename Traits, typename T>
UTL_ATTRIBUTES(NODISCARD, PURE)
constexpr T* find_first_of(
    T const* str, element_count_t len, T const* chars, element_count_t char_count) noexcept {
    return UTL_CONSTANT_P((str != chars) + len + char_count)
        ? compile_time::find_first_of<Traits>(str, len, chars, char_count)
        : runtime::find_first_of<Traits>(str, len, chars, char_count);
}

template <typename Traits, typename T>
UTL_ATTRIBUTES(NODISCARD, PURE)
constexpr T* find_first_not_of(
    T const* str, element_count_t len, T const* chars, element_count_t char_count) noexcept {
    return UTL_CONSTANT_P((str != chars) + len + char_count)
        ? compile_time::find_first_not_of<Traits>(str, len, chars, char_count)
        : runtime::find_first_not_of<Traits>(str, len, chars, char_count);
}

template <typename Traits, typename T>
UTL_ATTRIBUTES(NODISCARD, PURE)
constexpr T* find_last_of(
    T const* str, element_count_t len, T const* chars, element_count_t char_count) noexcept {
    return UTL_CONSTANT_P((str != chars) + len + char_count)
        ? compile_time::find_last_of<Traits>(str, str + len - 1, chars, char_count)
        : runtime::find_last_of<Traits>(str, str + len - 1, chars, char_count);
}

template <typename Traits, typename T>
UTL_ATTRIBUTES(NODISCARD, PURE)
constexpr T* find_last_not_of(
    T const* str, element_count_t len, T const* chars, element_count_t char_count) noexcept {
    return UTL_CONSTANT_P((str != chars) + len + char_count)
        ? compile_time::find_last_not_of<Traits>(str, str + len - 1, chars, char_count)
        : runtime::find_last_not_of<Traits>(str, str + len - 1, chars, char_count);
}

UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr int compare_size(int strcmp, size_t left, size_t right) noexcept {
    return other_if_zero(strcmp, value_if_true(left != right, negative_if_true(left < right)));
}

template <typename Traits, typename T>
UTL_ATTRIBUTES(NODISCARD, PURE)
UTL_CONSTEXPR_CXX14 int compare(
    T const* left, element_count_t l_len, T const* right, element_count_t r_len) noexcept {
    auto const len = UTL_SCOPE numeric_min(size_t(l_len), size_t(r_len));
    auto const cmp = Traits::compare(left, right, len);
    return compare_size(cmp, size_t(l_len), size_t(r_len));
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_REQUIRES_CXX11(is_string_char<T>::value)>
constexpr size_t to_index(T const* base, T const* ptr) noexcept {
    return ptr ? ptr - base : UTL_SCOPE details::string::npos;
}

} // namespace string
} // namespace details

UTL_NAMESPACE_END
