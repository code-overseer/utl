// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/string/utl_libc_common.h"
#include "utl/utility/utl_signs.h"

UTL_NAMESPACE_BEGIN

namespace libc {
namespace compile_time {
namespace recursive {

/**
 * Non-pure, multi statement functions are _not_ constexpr in C++11,
 * the recursive implementation is only used to suppress warnings
 * about C++14 extensions
 */
template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr T* memcpy(
    T* dst, T const* src, element_count_t count, T* org) noexcept {
    return count == 0 ? org : ((*dst = *src), memcpy(dst + 1, src + 1, count - 1, org));
}

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr T* reverse_memcpy(
    T* dst, T const* src, element_count_t count, T* org) noexcept {
    return count == 0 ? org : ((*--dst = *--src), memcpy(dst, src, count - 1, org));
}

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr T* memset(
    T* dst, T const src, element_count_t count, T* org) noexcept {
    return count == 0 ? org : ((*dst = src), memset(dst + 1, src, count - 1, org));
}

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr T* memmove(
    T* dst, T const* src, element_count_t count) noexcept {
    return is_pointer_in_range(src, src + count, dst)
        ? reverse_memcpy(dst + count, src + count, count, dst)
        : memcpy(dst, src, count, dst);
}

template <typename T, typename U>
__UTL_HIDE_FROM_ABI inline constexpr int memcmp(
    T const* left, U const* right, element_count_t count) noexcept {
    static_assert(is_trivially_lexicographically_comparable<T, U>::value,
        "Types must be lexicographically comparable");
    return count == 0      ? 0
        : (*left < *right) ? -1
        : (*right < *left)
        ? 1
        : __UTL libc::compile_time::recursive::memcmp(left + 1, right + 1, count - 1);
}

template <UTL_CONCEPT_CXX20(exact_size<1>) T, UTL_CONCEPT_CXX20(
    exact_size<1>) U UTL_CONSTRAINT_CXX11(exact_size<T, 1>::value && exact_size<U, 1>::value)>
__UTL_HIDE_FROM_ABI inline constexpr T* memchr(T const* str, U value, size_t bytes) noexcept {
    return bytes == 0   ? nullptr
        : *str == value ? const_cast<T*>(str)
                        : __UTL libc::compile_time::recursive::memchr(str + 1, value, bytes - 1);
}

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr size_t strlen(T const* str, size_t r = 0) noexcept {
    return !*str ? r : strlen(str + 1, r + 1);
}

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr T* strchr(T const* str, T const ch) noexcept {
    return *str == ch ? const_cast<T*>(str)
        : !*str       ? nullptr
                      : __UTL libc::compile_time::recursive::strchr(str + 1, ch);
}

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr int strcmp(T const* left, T const* right) noexcept {
    return !*left && !*right                                     ? 0
        : (__UTL to_unsigned(*left) < __UTL to_unsigned(*right)) ? -1
        : (__UTL to_unsigned(*right) < __UTL to_unsigned(*left))
        ? 1
        : __UTL libc::compile_time::recursive::strcmp(left + 1, right + 1);
}

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr int strncmp(
    T const* left, T const* right, element_count_t len) noexcept {
    return len == 0 || (!*left && !*right)                       ? 0
        : (__UTL to_unsigned(*left) < __UTL to_unsigned(*right)) ? -1
        : (__UTL to_unsigned(*right) < __UTL to_unsigned(*left))
        ? 1
        : __UTL libc::compile_time::recursive::strncmp(left + 1, right + 1, len - 1);
}

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr T* strnchr(
    T const* str, T const ch, element_count_t len) noexcept {
    return len == 0    ? nullptr
        : (*str == ch) ? const_cast<T*>(str)
        : (!*str)      ? nullptr
                       : __UTL libc::compile_time::recursive::strnchr(str + 1, ch, len - 1);
}

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr T* strnset(
    T* dst, T const val, element_count_t count, T* org) noexcept {
    return count == 0
        ? org
        : ((*dst = val),
              __UTL libc::compile_time::recursive::strnset(dst + 1, val, count - 1, org));
}

} // namespace recursive

template <UTL_CONCEPT_CXX20(trivially_copyable) T UTL_CONSTRAINT_CXX11(
    is_trivially_copyable<T>::value)>
__UTL_HIDE_FROM_ABI inline constexpr T* memcpy(
    T* dst, T const* src, element_count_t count) noexcept {
    return recursive::memcpy(dst, src, count, dst);
}

template <UTL_CONCEPT_CXX20(trivially_copyable) T UTL_CONSTRAINT_CXX11(
    is_trivially_copyable<T>::value && exact_size<T, 1>::value)>
UTL_CONSTRAINT_CXX20(exact_size<T, 1>)
__UTL_HIDE_FROM_ABI inline constexpr T* memset(
    T* dst, T const src, element_count_t count) noexcept {
    return recursive::memset(dst, as_byte(src), count, dst);
}

template <UTL_CONCEPT_CXX20(trivially_copyable) T UTL_CONSTRAINT_CXX11(
    is_trivially_copyable<T>::value)>
__UTL_HIDE_FROM_ABI inline constexpr T* memmove(
    T* dst, T const* src, element_count_t count) noexcept {
    return recursive::memmove(dst, src, count);
}

template <typename T, typename U>
__UTL_HIDE_FROM_ABI inline constexpr int memcmp(
    T const* left, U const* right, element_count_t count) noexcept {
    static_assert(is_trivially_lexicographically_comparable<T, U>::value,
        "Types must be lexicographically comparable");
    return recursive::memcmp(left, right, count);
}

__UTL_HIDE_FROM_ABI inline constexpr char* memchr(
    char const* str, char value, size_t bytes) noexcept {
    return recursive::memchr(str, value, bytes);
}

template <UTL_CONCEPT_CXX20(exact_size<1>) T, UTL_CONCEPT_CXX20(
    exact_size<1>) U UTL_CONSTRAINT_CXX11(exact_size<T, 1>::value && exact_size<U, 1>::value)>
__UTL_HIDE_FROM_ABI inline constexpr T* memchr(T const* str, U value, size_t bytes) noexcept {
    return recursive::memchr(str, as_byte(value), bytes);
}

__UTL_HIDE_FROM_ABI inline constexpr size_t strlen(char const* str) noexcept {
    return recursive::strlen(str);
}

__UTL_HIDE_FROM_ABI inline constexpr size_t strlen(wchar_t const* str) noexcept {
    return recursive::strlen(str);
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
__UTL_HIDE_FROM_ABI inline constexpr size_t strlen(T const* str) noexcept {
    return recursive::strlen(str);
}

__UTL_HIDE_FROM_ABI inline constexpr char* strchr(char const* str, char const ch) noexcept {
    return recursive::strchr(str, ch);
}

__UTL_HIDE_FROM_ABI inline constexpr wchar_t* strchr(
    wchar_t const* str, wchar_t const ch) noexcept {
    return recursive::strchr(str, ch);
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
__UTL_HIDE_FROM_ABI inline constexpr T* strchr(T const* str, T const ch) noexcept {
    return recursive::strchr(str, ch);
}

__UTL_HIDE_FROM_ABI inline constexpr int strcmp(char const* left, char const* right) noexcept {
    return recursive::strcmp(left, right);
}

__UTL_HIDE_FROM_ABI inline constexpr int strcmp(
    wchar_t const* left, wchar_t const* right) noexcept {
    return recursive::strcmp(left, right);
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
__UTL_HIDE_FROM_ABI inline constexpr int strcmp(T const* left, T const* right) noexcept {
    return recursive::strcmp(left, right);
}

__UTL_HIDE_FROM_ABI inline constexpr int strncmp(
    char const* left, char const* right, element_count_t len) noexcept {
    return recursive::strncmp(left, right, len);
}

__UTL_HIDE_FROM_ABI inline constexpr int strncmp(
    wchar_t const* left, wchar_t const* right, element_count_t len) noexcept {
    return recursive::strncmp(left, right, len);
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
__UTL_HIDE_FROM_ABI inline constexpr int strncmp(
    T const* left, T const* right, element_count_t len) noexcept {
    return recursive::strncmp(left, right, len);
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
__UTL_HIDE_FROM_ABI inline constexpr T* strnset(
    T* dst, T const val, element_count_t max_len) noexcept {
    return recursive::strnset(dst, val, max_len, dst);
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
__UTL_HIDE_FROM_ABI inline constexpr T* strnchr(
    T const* str, T const val, element_count_t max_len) noexcept {
    return recursive::strnchr(str, val, max_len);
}

} // namespace compile_time
} // namespace libc

UTL_NAMESPACE_END
