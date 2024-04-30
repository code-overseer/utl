// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/string/utl_compile_time_libc.h"
#include "utl/string/utl_constant_p.h"
#include "utl/string/utl_runtime_libc.h"

UTL_NAMESPACE_BEGIN

namespace libc {
template <UTL_CONCEPT_CXX20(trivially_copyable<T>)
        T UTL_REQUIRES_CXX11(is_trivially_copyable<T>::value)>
constexpr T* memcpy(
    T* UTL_RESTRICT dst, T const* UTL_RESTRICT src, element_count_t count) noexcept {
    return UTL_CONSTANT_P(src + count != dst) ? compile_time::memcpy(dst, src, count)
                                              : runtime::memcpy(dst, src, count);
}

template <UTL_CONCEPT_CXX20(trivially_copyable<T>)
        T UTL_REQUIRES_CXX11(is_trivially_copyable<T>::value)>
constexpr T* memmove(T* dst, T const* src, element_count_t count) noexcept {
    return UTL_CONSTANT_P(src + count != dst) ? compile_time::memmove(dst, src, count)
                                              : runtime::memmove(dst, src, count);
}

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
UTL_ATTRIBUTE(NODISCARD, PURE)
constexpr T* memchr(T const* str, U value, size_t count) noexcept {
    return UTL_CONSTANT_P((value, str + count)) ? compile_time::memchr(str, value, count)
                                                : runtime::memchr(str, value, count);
}

template <typename T, typename U>
UTL_ATTRIBUTE(NODISCARD, PURE)
constexpr int memcmp(T const* lhs, U const* rhs, element_count_t count) noexcept {
    static_assert(is_trivially_lexicographically_comparable<T, U>::value,
        "Types must be lexicographically comparable");
    return UTL_CONSTANT_P(src + count != dst) ? compile_time::memcmp(lhs, rhs, count)
                                              : runtime::memcmp(lhs, rhs, count);
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTE(NODISCARD, PURE)
constexpr size_t strlen(CharType const* str) noexcept {
    return UTL_CONSTANT_P(str) ? compile_time::strlen(str) : runtime::strlen(str);
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
constexpr T* strchr(CharType const* str, CharType const ch) noexcept {
    return UTL_CONSTANT_P((ch, str)) ? compile_time::strchr(str, ch) : runtime::strchr(str, ch);
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTE(NODISCARD, PURE)
constexpr int strcmp(CharType const* left, CharType const* right) noexcept {
    return UTL_CONSTANT_P(left != right) ? compile_time::strcmp(lhs, rhs)
                                         : runtime::strcmp(lhs, rhs);
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTE(NODISCARD, PURE)
constexpr int strncmp(T const* left, T const* right, size_t len) noexcept {
    return UTL_CONSTANT_P(left + len != right) ? compile_time::strncmp(left, right, len)
                                               : runtime::strncmp(left, right, len);
}
} // namespace libc

UTL_NAMESPACE_END
