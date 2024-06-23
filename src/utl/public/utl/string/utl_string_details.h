// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/string/utl_char_traits.h"
#include "utl/string/utl_libc.h"

namespace details {
namespace string {
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
UTL_ATTRIBUTES(NODISCARD, PURE)
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
UTL_ATTRIBUTES(NODISCARD, PURE)
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
UTL_ATTRIBUTES(NODISCARD, PURE)
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
} // namespace runtime

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
inline UTL_CONSTEXPR_CXX14 CharType const* find(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count, size_t l_pos) noexcept {
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
    return UTL_CONSTANT_P(*(str + length) == ch)
        ? compile_time::rfind_char(str, ch, element_count_t(length))
        : runtime::rfind_char(str, ch, element_count_t(length));
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTES(NODISCARD, PURE)
inline UTL_CONSTEXPR_CXX14 CharType const* rfind(CharType const* const l, size_t l_count,
    CharType const* const r, size_t const r_count) noexcept {
    auto l_last = l + l_count - 1;
    if (r_count == 0) {
        return l_last;
    }

    if (l_count < r_count) {
        return nullptr;
    }

    for (auto const r_char = r[r_count - 1];; --l_last) {

        l_count = l_last - l + 1;
        if (l_count < r_count) {
            return nullptr;
        }

        l = rfind(l, l_count, r_char);
        if (l == nullptr) {
            return nullptr;
        }

        auto const l_begin = l_last - r_count + 1;
        if (Traits::compare(l_begin, r, r_count) == 0) {
            return l_begin;
        }

        --l_last;
    }
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
} // namespace string
} // namespace details
