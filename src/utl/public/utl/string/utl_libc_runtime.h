// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/string/utl_libc_common.h"
#include "utl/utility/utl_seq.h"
#include "utl/utility/utl_signs.h"

#ifdef UTL_COMPILER_MSVC
#  include <string.h>
#endif

#ifdef UTL_SIMD_X86_AVX512
#  include <immintrin.h>
#elif defined(UTL_SIMD_X86_AVX2)
#  include <immintrin.h>
#elif defined(UTL_SIMD_X86_SSE4_2)
#  include <immintrin.h>
#endif // elif defined(UTL_SIMD_X86_SSE4_2)
#ifdef UTL_SIMD_ARM_SVE
#  include <arm_sve.h>
#elif defined(UTL_SIMD_ARM_NEON)
#  include <arm_neon.h>
#endif

UTL_NAMESPACE_BEGIN

namespace libc {
namespace runtime {
namespace standard {
template <UTL_CONCEPT_CXX20(exact_size<1>) T UTL_REQUIRES_CXX11(exact_size<T, 1>::value)>
T* memcpy(T* UTL_RESTRICT dst, T const* UTL_RESTRICT src, element_count_t count) noexcept {
#if UTL_HAS_BUILTIN(__builtin_memcpy)
    return (T*)__builtin_memcpy(dst, src, byte_count(src, count));
#else
    return (T*)::memcpy(dst, src, byte_count(src, count));
#endif
}

template <UTL_CONCEPT_CXX20(exact_size<1>) T UTL_REQUIRES_CXX11(exact_size<T, 1>::value)>
T* memmove(T* dst, T const* src, element_count_t count) noexcept {
#if UTL_HAS_BUILTIN(__builtin_memcpy)
    return (T*)__builtin_memmove(dst, src, byte_count(dst, count));
#else
    return (T*)::memmove(dst, src, byte_count(src, count));
#endif
}

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
#if UTL_HAS_BUILTIN(__builtin_char_memchr)
    return (T*)__builtin_char_memchr(ptr, value, bytes);
#else
    return (T*)::memchr(ptr, value, bytes);
#endif
}
} // namespace standard

#if defined(UTL_SUPPORTS_SIMD_INTRINSICS) && !defined(UTL_DISABLE_SIMD_LIBC)
namespace simd {
static constexpr size_t npos = (size_t)-1;
/**
 * @brief Calculates the number of trailing zeros in an integer.
 *
 * This function calculates the number of trailing zeros (ctz) in the binary representation of the
 * given integer. It provides a safe implementation across different platforms.
 *
 * @param x The integer value for which to calculate the number of trailing zeros.
 * @return The number of trailing zeros in the binary representation of the integer.
 *         If the input integer is zero, it returns the maximum value of size_t (npos).
 */
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) size_t safe_ctz(uint32_t x) noexcept {
#  if UTL_HAS_BUILTIN(__builtin_ctz)
    return x ? (size_t)__builtin_ctz(x) : npos;
#  elif defined(UTL_COMPILER_MSVC)
    return x ? (size_t)_BitScanForward(x) : npos;
#  else
#    error "Unsupported platform CTZ intrinsic not provided, defined UTL_DISABLE_SIMD_LIBC"
#  endif
}

/**
 * @brief Calculates the number of trailing zeros in a 64-bit integer.
 *
 * This function calculates the number of trailing zeros (ctz) in the binary representation of the
 * given 64-bit integer. It provides a safe implementation across different platforms.
 *
 * @param x The 64-bit integer value for which to calculate the number of trailing zeros.
 * @return The number of trailing zeros in the binary representation of the 64-bit integer.
 *         If the input integer is zero, it returns the maximum value of size_t (npos).
 * @note This function is marked as NODISCARD, CONST, and ALWAYS_INLINE.
 * @note This function is noexcept.
 */
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) size_t safe_ctzll(uint64_t x) noexcept {
#  if UTL_HAS_BUILTIN(__builtin_ctz)
    return x ? (size_t)__builtin_ctzll(x) : npos;
#  elif defined(UTL_COMPILER_MSVC)
    return x ? (size_t)_BitScanForward64(x) : npos;
#  else
#    error "Unsupported platform CTZ intrinsic not provided, defined UTL_DISABLE_SIMD_LIBC"
#  endif
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T min(T l, T r) noexcept {
    return l < r ? l : r;
}

namespace x86 {
#  ifdef UTL_SIMD_X86_AVX512

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
    using register_t = __m512i;
    static constexpr size_t register_size = sizeof(register_t);
    char to_copy;
    __m256i needles = _mm512_set1_epi8(*reinterpret_cast<uint8_t const*>(&value));
    T const* const end = ptr + bytes;
    size_t idx = npos;
    do {
        register_t haystack = _mm512_lddqu_si512((register_t const*)ptr);
        __mmask64 mask = _mm512_cmpeq_epu8_mask(haystack, needles);
        idx = safe_ctzll(_cvtmask64_u64(mask));
        if (idx != npos) {
            break;
        }
        ptr += register_size;
    } while (ptr < end);

    return idx < bytes ? str + idx : nullptr;
}

#  elif defined(UTL_SIMD_X86_AVX2)

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
    using register_t = __m256i;
    static constexpr size_t register_size = sizeof(register_t);
    register_t needles = _mm256_set1_epi8(*reinterpret_cast<uint8_t const*>(&value));
    T const* const end = ptr + bytes;
    size_t idx = npos;
    do {
        register_t haystack = _mm256_lddqu_si256((register_t const*)ptr);
        int const mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(haystack, needles));
        idx = safe_ctz(to_unsigned(mask));
        if (idx != npos) {
            break;
        }
        ptr += register_size;
    } while (ptr < end);

    return idx < bytes ? str + idx : nullptr;
}

#  elif defined(UTL_SIMD_X86_SSE4_2)

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
    using register_t = __m128i;
    static constexpr size_t register_size = sizeof(register_t);
    register_t needles = _mm_set1_epi8(*reinterpret_cast<uint8_t const*>(&value));
    T const* const end = ptr + bytes;
    do {
        register_t haystack = _mm_lddqu_si128((register_t const*)ptr);
        int const mask = _mm_movemask_epi8(_mm_cmpeq_epi8(haystack, needles));
        size_t const idx = safe_ctz(to_unsigned(mask));
        if (idx != npos) {
            return const_cast<T*>(ptr + idx);
        }
        ptr += register_size;
    } while (ptr < end);

    return nullptr;
}

#  endif // elif defined(UTL_SIMD_X86_SSE4_2)
} // namespace x86

namespace arm {
#  ifdef UTL_SIMD_ARM_SVE

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
    svuint8_t const needles = svdup_n_u8(*reinterpret_cast<uint8_t const*>(&value));
    T const* const end = ptr + bytes;
    size_t const register_size = svcntb(); // max 255 (SVE has a maximum 2048-bit vector)
    svuint8_t const indices = svindex_u8(0, 1);
    size_t idx = npos;
    do {
        uint8_t const count = min((uint8_t)(end - ptr), (uint8_t)register_size);
        svbool_t const active = svcmplt_u8(indices, svdup_n_u8(count));
        svuint8_t haystack = svld1_u8(active, reinterpret_cast<uint8_t const*>(ptr));
        svbool_t const eq = svcmpeq_n_u8(active, haystack, needles);
        if (svptest_any(eq)) {
            uint8_t const idx = svlastb_u8(svbrka_b_z(active, eq), indices);
            return const_cast<T*>(ptr + idx);
        }
        ptr += count;
    } while (ptr < end);

    return nullptr;
}

#  elif defined(UTL_SIMD_ARM_NEON)

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) uint16_t movemask(uint8x16_t reg) noexcept {
    static constexpr size_t elements = 16;
    using exp_t = decltype(seq::tile<2>(seq::range_t<uint8_t, 8>{}));
    using ones_t = decltype(seq::repeat<elements>(seq::scalar_t<uint8_t, 1>{}));
    using bits_t = decltype(seq::bw_ls(ones_t{}, exp_t{}));
    uint8x16_t masks = vld1q_u8(seq::to_array(bits_t{}));
    uint8x16_t reduction =
        vreinterpretq_u8_u64(vpaddlq_u32(vpaddlq_u16(vpaddlq_u8(vandq_u8(masks, reg)))));
    uint16_t output;
    vst1q_lane_u8((uint8_t*)output, reduction, 0);
    vst1q_lane_u8((uint8_t*)output + 1, reduction, 8);
    return output;
}

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
    static constexpr size_t register_size = sizeof(uint8x16_t);
    uint8x16_t needles = vdupq_n_u8(*reinterpret_cast<uint8_t const*>(&value));
    T const* const end = ptr + bytes;
    do {
        uint8x16_t haystack = vld1q_u8((uint8_t const*)ptr);
        uint16_t const mask = movemask(vceqq_u8(haystack, needles));
        size_t const idx = safe_ctz((uint32_t)mask);
        if (idx != npos) {
            return const_cast<T*>(ptr + idx);
        }
        ptr += register_size;
    } while (ptr < end);

    return nullptr;
}

#  endif
} // namespace arm

#  ifdef UTL_ARCH_x86
using x86::memchr;
using x86::memcmp;
using x86::strchr;
using x86::strcmp;
using x86::strlen;
using x86::strncmp;
#  elif defined(UTL_ARCH_ARM)
using arm::memchr;
using arm::memcmp;
using arm::strchr;
using arm::strcmp;
using arm::strlen;
using arm::strncmp;
#  endif

} // namespace simd

using simd::memchr;
using simd::memcmp;
using simd::strchr;
using simd::strcmp;
using simd::strlen;
using simd::strncmp;
#else  // if defined(UTL_SUPPORTS_SIMD_INTRINSICS) && !defined(UTL_DISABLE_SIMD_LIBC)
using standard::memchr;
using standard::memcmp;
using standard::strchr;
using standard::strcmp;
using standard::strlen;
using standard::strncmp;
#endif // if defined(UTL_SUPPORTS_SIMD_INTRINSICS) && !defined(UTL_DISABLE_SIMD_LIBC)

using standard::memcpy;
using standard::memmove;
} // namespace runtime
} // namespace libc

UTL_NAMESPACE_END
