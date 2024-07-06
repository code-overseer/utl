// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/string/utl_libc_common.h"

UTL_NAMESPACE_BEGIN

namespace libc {
namespace compile_time {
namespace recursive {

template <typename T>
UTL_CONSTEVAL T* memcpy(T* dst, T const* src, element_count_t count, T* org) noexcept {
    return count == 0 ? org : ((*dst = *src), memcpy(dst + 1, src + 1, count - 1, org));
}

template <typename T>
UTL_CONSTEVAL T* reverse_memcpy(T* dst, T const* src, element_count_t count, T* org) noexcept {
    return count == 0 ? org : ((*--dst = *--src), memcpy(dst, src, count - 1, org));
}

template <typename T>
UTL_CONSTEVAL T* memset(T* dst, T const src, element_count_t count, T* org) noexcept {
    return count == 0 ? org : ((*dst = src), memset(dst + 1, src, count - 1, org));
}

template <typename T>
UTL_CONSTEVAL T* memmove(T* dst, T const* src, element_count_t count) noexcept {
    return is_pointer_in_range(src, src + count, dst)
        ? reverse_memcpy(dst + count, src + count, count, dst)
        : memcpy(dst, src, count, dst);
}

template <typename T, typename U>
UTL_CONSTEVAL int memcmp(T const* left, U const* right, element_count_t count) noexcept {
    static_assert(is_trivially_lexicographically_comparable<T, U>::value,
        "Types must be lexicographically comparable");
    return count == 0      ? 0
        : (*left < *right) ? -1
        : (*right < *left) ? 1
                           : memcmp(left + 1, right + 1, count - 1);
}

template <UTL_CONCEPT_CXX20(exact_size<1>) T, UTL_CONCEPT_CXX20(exact_size<1>) U UTL_REQUIRES_CXX11(
    exact_size<T, 1>::value && exact_size<U, 1>::value)>
UTL_CONSTEVAL T* memchr(T const* str, U value, size_t bytes) noexcept {
    return bytes == 0   ? nullptr
        : *str == value ? const_cast<T*>(str)
                        : memchr(str + 1, value, bytes - 1);
}

template <typename T>
UTL_CONSTEVAL size_t strlen(T const* str, size_t r = 0) noexcept {
    return !*str ? r : strlen(str, r + 1);
}

template <typename T>
UTL_CONSTEVAL T* strchr(T const* str, T const ch) noexcept {
    return *str == ch ? const_cast<T*>(str) : !*str ? nullptr : strchr(str + 1, ch);
}

template <typename T>
UTL_CONSTEVAL int strcmp(T const* left, T const* right) noexcept {
    return !*left && !*right ? 0
        : (*left < *right)   ? -1
        : (*right < *left)   ? 1
                             : strcmp(left + 1, right + 1);
}

template <typename T>
UTL_CONSTEVAL int strncmp(T const* left, T const* right, element_count_t len) noexcept {
    return len == 0 || (!*left && !*right) ? 0
        : (*left < *right)                 ? -1
        : (*right < *left)                 ? 1
                                           : strncmp(left + 1, right + 1, len - 1);
}

template <typename T>
UTL_CONSTEVAL T* strnchr(T const* str, T const ch, element_count_t len) noexcept {
    return len == 0    ? nullptr
        : (*str == ch) ? const_cast<T*>(str)
        : (!*str)      ? nullptr
                       : strnchr(str + 1, ch, len - 1);
}

template <typename T>
UTL_CONSTEVAL T* strnset(T* dst, T const val, element_count_t count, T* org) noexcept {
    return count == 0 ? org : ((*dst = val), strnset(dst + 1, val, count - 1, org));
}

} // namespace recursive

template <UTL_CONCEPT_CXX20(trivially_copyable) T UTL_REQUIRES_CXX11(
    is_trivially_copyable<T>::value)>
UTL_CONSTEVAL T* memcpy(T* dst, T const* src, element_count_t count) noexcept {
#if UTL_HAS_BUILTIN(__builtin_memcpy)
    return __builtin_memcpy(dst, src, byte_count<T>(count)), dst;
#else
    return recursive::memcpy(dst, src, count, dst);
#endif
}

template <UTL_CONCEPT_CXX20(trivially_copyable) T UTL_REQUIRES_CXX11(
    is_trivially_copyable<T>::value && exact_size<T, 1>::value)>
UTL_REQUIRES_CXX20(exact_size<T, 1>)
UTL_CONSTEVAL T* memset(T* dst, T const src, element_count_t count) noexcept {
#if UTL_HAS_BUILTIN(__builtin_memset)
    return __builtin_memset(dst, as_byte(src), byte_count<T>(count)), dst;
#else
    return recursive::memset(dst, as_byte(src), count, dst);
#endif
}

template <UTL_CONCEPT_CXX20(trivially_copyable) T UTL_REQUIRES_CXX11(
    is_trivially_copyable<T>::value)>
UTL_CONSTEVAL T* memmove(T* dst, T const* src, element_count_t count) noexcept {
#if UTL_HAS_BUILTIN(__builtin_memmove)
    return __builtin_memmove(dst, src, byte_count<T>(count)), dst;
#else
    return recursive::memmove(dst, src, count)
#endif
}

template <typename T, typename U>
UTL_CONSTEVAL int memcmp(T const* left, U const* right, element_count_t count) noexcept {
    static_assert(is_trivially_lexicographically_comparable<T, U>::value,
        "Types must be lexicographically comparable");
#if UTL_HAS_BUILTIN(__builtin_char_memcmp)
    return __builtin_memcmp(left, right, byte_count<T>(count));
#else
    return recursive::memcmp(left, right, count);
#endif
}

UTL_CONSTEVAL char* memchr(char const* str, char value, size_t bytes) noexcept {
#if UTL_HAS_BUILTIN(__builtin_char_memchr)
    return __builtin_char_memchr(str, (int)value, bytes);
#else
    return recuresive::memchr(str, value, bytes);
#endif
}

template <UTL_CONCEPT_CXX20(exact_size<1>) T, UTL_CONCEPT_CXX20(exact_size<1>) U UTL_REQUIRES_CXX11(
    exact_size<T, 1>::value && exact_size<U, 1>::value)>
UTL_CONSTEVAL T* memchr(T const* str, U value, size_t bytes) noexcept {
    return recursive::memchr(str, as_byte(value), bytes);
}

UTL_CONSTEVAL size_t strlen(char const* str) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strlen)
    return __builtin_strlen(str);
#else
    return recursive::strlen(str);
#endif
}

UTL_CONSTEVAL size_t strlen(wchar_t const* str) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcslen)
    return __builtin_wcslen(str);
#else
    return recursive::strlen(str);
#endif
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_REQUIRES_CXX11(is_string_char<T>::value)>
UTL_CONSTEVAL size_t strlen(T const* str) noexcept {
    return recursive::strlen(str);
}

UTL_CONSTEVAL char* strchr(char const* str, char const ch) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strchr)
    return __builtin_strchr(str, (int)ch);
#else
    return recursive::strchr(str, ch);
#endif
}

UTL_CONSTEVAL wchar_t* strchr(wchar_t const* str, wchar_t const ch) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcschr)
    return __builtin_wcschr(str, ch);
#else
    return recursive::strchr(str, ch);
#endif
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_REQUIRES_CXX11(is_string_char<T>::value)>
UTL_CONSTEVAL T* strchr(T const* str, T const ch) noexcept {
    return recursive::strchr(str, ch);
}

UTL_CONSTEVAL int strcmp(char const* left, char const* right) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strcmp)
    return __builtin_strcmp(left, right);
#else
    return recursive::strcmp(left, right);
#endif
}

UTL_CONSTEVAL int strcmp(wchar_t const* left, wchar_t const* right) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcscmp)
    return __builtin_wcscmp(left, right);
#else
    return recursive::strcmp(left, right);
#endif
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_REQUIRES_CXX11(is_string_char<T>::value)>
UTL_CONSTEVAL int strcmp(T const* left, T const* right) noexcept {
    return recursive::strcmp(left, right);
}

UTL_CONSTEVAL int strncmp(char const* left, char const* right, element_count_t len) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strncmp)
    return __builtin_strncmp(left, right, (size_t)len);
#else
    return recursive::strncmp((unsigned char const*)left, (unsigned char const*)right, len);
#endif
}

UTL_CONSTEVAL int strncmp(wchar_t const* left, wchar_t const* right, element_count_t len) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcsncmp)
    return __builtin_wcsncmp(left, right, (size_t)len);
#else
    return recursive::strncmp(left, right, len);
#endif
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_REQUIRES_CXX11(is_string_char<T>::value)>
UTL_CONSTEVAL int strncmp(T const* left, T const* right, element_count_t len) noexcept {
    return recursive::strncmp(left, right, len);
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_REQUIRES_CXX11(is_string_char<T>::value)>
UTL_CONSTEVAL T* strnset(T* dst, T const val, element_count_t max_len) noexcept {
    return recursive::strnset(dst, val, max_len, dst);
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_REQUIRES_CXX11(is_string_char<T>::value)>
UTL_CONSTEVAL T* strnchr(T const* str, T const val, element_count_t max_len) noexcept {
    return recursive::strnchr(str, val, max_len);
}

} // namespace compile_time
} // namespace libc

UTL_NAMESPACE_END
