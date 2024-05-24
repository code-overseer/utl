// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/string/utl_is_string_char.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_empty.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_unsigned.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_std_traits.h" // is_trivially_copyable

#include <string.h>

#if UTL_DISABLE_SSE_LIBC
#  define UTL_DISABLE_SIMD_LIBC
#elif defined(UTL_DISABLE_AVX_LIBC)
#  define UTL_DISABLE_AVX512_LIBC
#endif

#if !UTL_DISABLE_SIMD_LIBC
#  if UTL_SUPPORTS_SIMD_INTRINSICS && UTL_ARCH_x86
#    include <immintrin.h>
#  endif // defined(UTL_SUPPORTS_SIMD_INTRINSICS) && defined(UTL_ARCH_x86)

#  if UTL_SIMD_ARM_SVE && !UTL_DISABLE_SVE_LIBC
#    include <arm_sve.h>
#  elif UTL_SIMD_ARM_NEON && !UTL_DISABLE_NEON_LIBC
#    include <arm_neon.h>
#  else
#    define UTL_DISABLE_SIMD_LIBC
#  endif
#endif

UTL_NAMESPACE_BEGIN

namespace libc {

enum class element_count_t : size_t {
};

#define UTL_LIBC_CONST UTL_ATTRIBUTES(NODISCARD, CONST)

template <typename T>
UTL_LIBC_CONST constexpr size_t byte_count(element_count_t c) noexcept {
    return sizeof(T) * size_t(c);
}

template <typename T>
UTL_LIBC_CONST constexpr bool is_pointer_in_range(
    T const* begin, T const* end, T const* ptr) noexcept {
    return UTL_CONSTANT_P(begin <= end && ptr < end) || !(ptr < begin) && ptr < end;
}
UTL_LIBC_CONST
constexpr element_count_t operator-(element_count_t val, size_t op) noexcept {
    return (element_count_t)((size_t)val - op);
}

UTL_LIBC_CONST
constexpr bool operator==(element_count_t val, size_t op) noexcept {
    return (size_t)val == op;
}

template <typename T>
UTL_LIBC_CONST constexpr T* operator+(T* ptr, element_count_t offset) noexcept {
    return ptr + (size_t)offset;
}

template <typename T, typename U>
struct is_trivially_lexicographically_comparable :
    bool_constant<UTL_TRAIT_is_same(remove_cv_t<T>, remove_cv_t<U>) && sizeof(T) == 1 &&
        UTL_TRAIT_is_unsigned(T)> {};

#if UTL_CXX20
template <typename T, size_t N>
concept exact_size = !is_empty_v<T> && (sizeof(T) == N);
template <typename T>
concept trivially_copyable = UTL_SCOPE is_trivially_copyable_v<T>;
#else
template <typename T, size_t N>
using exact_size = bool_constant<!UTL_TRAIT_is_empty(T) && (sizeof(T) == N)>;
#endif

template <UTL_CONCEPT_CXX20(exact_size<1>) T UTL_REQUIRES_CXX11(exact_size<T, 1>::value)>
unsigned char as_byte(T val) noexcept {
    return *((unsigned char const*)&val);
}
} // namespace libc

#undef UTL_LIBC_CONST

UTL_NAMESPACE_END
