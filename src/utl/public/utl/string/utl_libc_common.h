// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/string/utl_is_string_char.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_empty.h"

#include <string.h>

#ifdef UTL_DISABLE_SSE_LIBC
#  define UTL_DISABLE_SIMD_LIBC
#elif defined(UTL_DISABLE_AVX_LIBC)
#  define UTL_DISABLE_AVX512_LIBC
#endif

#ifndef UTL_DISABLE_SIMD_LIBC
#  if defined(UTL_SUPPORTS_SIMD_INTRINSICS) && defined(UTL_ARCH_x86)
#    include <immintrin.h>
#  endif // defined(UTL_SUPPORTS_SIMD_INTRINSICS) && defined(UTL_ARCH_x86)

#  if defined(UTL_SIMD_ARM_SVE) && !defined(UTL_DISABLE_SVE_LIBC)
#    include <arm_sve.h>
#  elif defined(UTL_SIMD_ARM_NEON) && !defined(UTL_DISABLE_NEON_LIBC)
#    include <arm_neon.h>
#  else
#    define UTL_DISABLE_SIMD_LIBC
#  endif
#endif

UTL_NAMESPACE_BEGIN

namespace libc {

enum class element_count_t : size_t {
};

template <typename T>
constexpr size_t byte_count(element_count_t c) noexcept {
    return sizeof(T) * size_t(c);
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool is_pointer_in_range(T const* begin, T const* end, T const* ptr) noexcept {
    return UTL_CONSTANT_P(begin <= end && ptr < end) || !(ptr < begin) && ptr < end;
}
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr element_count_t operator-(element_count_t val, size_t op) noexcept {
    return (element_count_t)((size_t)val - op);
}

UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool operator==(element_count_t val, size_t op) noexcept {
    return (size_t)val == op;
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T* operator+(T* ptr, element_count_t offset) noexcept {
    return ptr + (size_t)offset;
}

template <typename T, typename U>
struct is_trivially_lexicographically_comparable :
    bool_constant<is_same<remove_cv_t<T>, remove_cv_t<U>>::value && sizeof(T) == 1 &&
        is_unsigned<T>::value> {};

#ifdef UTL_CXX20
template <typename T, size_t N>
concept exact_size = !is_empty_v<T> && (sizeof(T) == N);
template <typename T>
concept trivially_copyable = UTL_SCOPE is_trivially_copyable_v<T>;
#else
template <typename T, size_t N>
using exact_size = bool_constant<!is_empty<T>::value && (sizeof(T) == N)>;
#endif

template <UTL_CONCEPT_CXX20(exact_size<1>) T UTL_REQUIRES_CXX11(exact_size<T, 1>::value)>
unsigned char as_byte(T val) noexcept {
    return *((unsigned char const*)&val);
}
} // namespace libc

UTL_NAMESPACE_END
