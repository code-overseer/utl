// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/string/utl_libc_common.h"
#include "utl/utility/utl_signs.h"

UTL_NAMESPACE_BEGIN

#define __UTL_ATTRIBUTE_LIBC_PURE (PURE)(NODISCARD) __UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_LIBC_PURE
#define __UTL_ATTRIBUTE_LIBC_INLINE_PURE \
    (PURE)(NODISCARD)(ALWAYS_INLINE)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_LIBC_INLINE_PURE

namespace libc {
namespace runtime {
namespace standard {

template <UTL_CONCEPT_CXX20(trivially_copyable) T UTL_CONSTRAINT_CXX11(is_trivially_copyable<T>::value)>
UTL_ATTRIBUTES(ALWAYS_INLINE,_HIDE_FROM_ABI) inline T* memcpy(
    T* UTL_RESTRICT dst, T const* UTL_RESTRICT src, element_count_t count) noexcept {
    return (T*)__UTL_MEMCPY(dst, src, byte_count<T>(count));
}

template <UTL_CONCEPT_CXX20(trivially_copyable) T UTL_CONSTRAINT_CXX11(is_trivially_copyable<T>::value)>
UTL_ATTRIBUTES(ALWAYS_INLINE,_HIDE_FROM_ABI) inline T* memmove(
    T* dst, T const* src, element_count_t count) noexcept {
#if UTL_HAS_BUILTIN(__builtin_memcpy)
    return (T*)__builtin_memmove(dst, src, byte_count<T>(count));
#else
    return (T*)::memmove(dst, src, byte_count<T>(count));
#endif
}

#if UTL_COMPILER_MSVC
#  pragma intrinsic(memset)
#endif

template <UTL_CONCEPT_CXX20(trivially_copyable) T UTL_CONSTRAINT_CXX11(
    is_trivially_copyable<T>::value && exact_size<T, 1>::value)>
UTL_CONSTRAINT_CXX20(exact_size<T, 1>)
UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) inline T* memset(
    T* dst, T const value, element_count_t count) noexcept {
#if UTL_HAS_BUILTIN(__builtin_memcpy)
    return (T*)__builtin_memset(dst, as_byte(value), byte_count<T>(count));
#else
    return (T*)::memset(dst, as_byte(value), byte_count<T>(count));
#endif
}

template <UTL_CONCEPT_CXX20(exact_size<1>) T, UTL_CONCEPT_CXX20(exact_size<1>) U UTL_CONSTRAINT_CXX11(
    exact_size<T, 1>::value && exact_size<U, 1>::value)>
UTL_ATTRIBUTES(LIBC_INLINE_PURE) inline T* memchr(T const* ptr, U value, size_t bytes) noexcept {
#if UTL_HAS_BUILTIN(__builtin_char_memchr)
    return (T*)__builtin_char_memchr(ptr, as_byte(value), bytes);
#else
    return (T*)::memchr(ptr, as_byte(value), bytes);
#endif
}

template <typename T, typename U>
UTL_ATTRIBUTES(LIBC_INLINE_PURE) inline int memcmp(T const* lhs, U const* rhs, element_count_t count) noexcept {
    static_assert(is_trivially_lexicographically_comparable<T, U>::value,
        "Types must be lexicographically comparable");
    return __UTL_MEMCMP(lhs, rhs, byte_count<T>(count));
}

#if UTL_COMPILER_MSVC
#  pragma intrinsic(strlen)
#endif

UTL_ATTRIBUTES(LIBC_INLINE_PURE) inline size_t strlen(char const* str) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strlen)
    return __builtin_strlen(str);
#else
    return ::strlen(str);
#endif
}

UTL_ATTRIBUTES(LIBC_INLINE_PURE) inline size_t strlen(wchar_t const* str) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcslen)
    return __builtin_wcslen(str);
#else
    return ::wcslen(str);
#endif
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
UTL_ATTRIBUTES(LIBC_PURE) inline size_t strlen(T const* str) noexcept {
    size_t count = 0;
    while (*str) {
        ++str;
        ++count;
    }

    return count;
}

UTL_ATTRIBUTES(LIBC_INLINE_PURE) inline char* strchr(char const* str, char const ch) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strchr)
    return __builtin_strchr(str, ch);
#else
    return ::strchr(str, ch);
#endif
}

UTL_ATTRIBUTES(LIBC_INLINE_PURE) inline wchar_t* strchr(wchar_t const* str, wchar_t const ch) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcschr)
    return __builtin_wcschr(str, ch);
#else
    return ::wcschr(str, ch);
#endif
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
UTL_ATTRIBUTES(LIBC_PURE) inline T* strnchr(T const* str, T const ch, element_count_t count) noexcept {
    size_t len = size_t(count);
    while (len) {
        if (*str == ch) {
            return const_cast<T*>(str);
        }
        if (!*str) {
            return nullptr;
        }

        ++str;
        --len;
    }

    return nullptr;
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
UTL_ATTRIBUTES(LIBC_PURE) inline T* strchr(T const* str, T const ch) noexcept {
    while (*str != ch) {
        if (!*str) {
            return nullptr;
        }
        ++str;
    }

    return (T*)str;
}

#if UTL_COMPILER_MSVC
#  pragma intrinsic(strcmp)
#endif

UTL_ATTRIBUTES(LIBC_INLINE_PURE) inline int strcmp(char const* left, char const* right) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strcmp)
    return __builtin_strcmp(left, right);
#else
    return ::strcmp(left, right);
#endif
}

UTL_ATTRIBUTES(LIBC_INLINE_PURE) inline int strcmp(wchar_t const* left, wchar_t const* right) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcscmp)
    return __builtin_wcscmp(left, right);
#else
    return ::wcscmp(left, right);
#endif
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
UTL_ATTRIBUTES(LIBC_PURE) inline int strcmp(T const* left, T const* right) noexcept {
    while (*left == *right) {
        if (!*left && !*right) {
            return 0;
        }

        ++left;
        ++right;
    }

    return (*left < *right) ? -1 : 1;
}

UTL_ATTRIBUTES(LIBC_INLINE_PURE) inline int strncmp(
    char const* left, char const* right, element_count_t len) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strcmp)
    return __builtin_strncmp(left, right, (size_t)len);
#else
    return ::strncmp(left, right, (size_t)len);
#endif
}

UTL_ATTRIBUTES(LIBC_INLINE_PURE) inline int strncmp(
    wchar_t const* left, wchar_t const* right, element_count_t len) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcscmp)
    return __builtin_wcsncmp(left, right, (size_t)len);
#else
    return ::wcsncmp(left, right, (size_t)len);
#endif
}

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
UTL_ATTRIBUTES(LIBC_PURE) inline int strncmp(T const* left, T const* right, element_count_t elements) noexcept {
    size_t len = (size_t)elements;
    while (len && *left == *right) {
        if (!*left) {
            return 0;
        }

        ++left;
        ++right;
        --len;
    }

    return (len == 0) ? 0 : (*left < *right) ? -1 : 1;
}

#if UTL_COMPILER_MSVC
#  pragma intrinsic(memcpy)
#endif

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
UTL_ATTRIBUTES(LIBC_PURE) inline T* strnset(T* dst, T const val, element_count_t elements) noexcept {
    size_t len = (size_t)elements;
    if (!len) {
        return dst;
    }

    static constexpr size_t buffer_count = 16 / sizeof(T);
    alignas(16) T buffer[buffer_count];
    for (auto& e : buffer) {
        e = val;
    }

    T* ptr = dst;
    do {
        size_t const copy_size = len < buffer_count ? len : buffer_count;
        ::memcpy(ptr, buffer, byte_count<T>(static_cast<element_count_t>(copy_size)));
        len -= copy_size;
        ptr += copy_size;
    } while (len);

    return dst;
}
} // namespace standard

using standard::memchr;
using standard::memcmp;
using standard::memcpy;
using standard::memmove;
using standard::memset;
using standard::strchr;
using standard::strcmp;
using standard::strlen;
using standard::strnchr;
using standard::strncmp;
using standard::strnset;
} // namespace runtime
} // namespace libc

#undef __UTL_ATTRIBUTE_LIBC_PURE
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_LIBC_PURE
#undef __UTL_ATTRIBUTE_LIBC_INLINE_PURE
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_LIBC_INLINE_PURE
UTL_NAMESPACE_END
