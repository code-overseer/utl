// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#ifndef UTL_NUMERIC_PRIVATE_HEADER_GUARD
#  error "Private header accessed"
#endif

#if !UTL_ARCH_ARM
#  error "This header is only available on ARM targets"
#endif // UTL_ARCH_ARM

#include "utl/numeric/utl_limits.h"
#include "utl/numeric/utl_sized_integral.h"

#if UTL_SUPPORTS_GNU_ASM

#  if UTL_ARCH_AARCH64

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace runtime {

template <typename T UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<8>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    static constexpr auto min = UTL_NUMERIC_minimum(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    __asm("subs    %x[left], %x[left], %x[right]\n\t"
          "asr     %x[right], %x[right], %x[bits]\n\t"
          "eor     %x[right], %x[right], %x[min]\n\t"
          "csel    %x[left], %x[right], %x[left], vs"
          : [left] "+r"(l), [right] "+r"(r)
          : [min] "i"(min), [bits] "Ji"(shift)
          : "cc");
    return l;
}

template <typename T UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(4, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<4>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(4, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    static constexpr auto min = UTL_NUMERIC_minimum(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    __asm("subs    %w[left], %w[left], %w[right]\n\t"
          "asr     %w[right], %w[right], %w[bits]\n\t"
          "eor     %w[right], %w[right], %w[min]\n\t"
          "csel    %w[left], %w[right], %w[left], vs"
          : [left] "+r"(l), [right] "+r"(r)
          : [min] "i"(min), [bits] "Ji"(shift)
          : "cc");
    return l;
}

template <typename T UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<2>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    static constexpr auto max = UTL_NUMERIC_maximum(T);
    static constexpr int ls = 2 * CHAR_BIT;
    static constexpr int rs = sizeof(l) * CHAR_BIT - 1;
    __asm("lsl     %w[left], %w[left], %[ls]\n\t"
          "subs    %w[right], %w[left], %w[right], lsl#%[ls]\n\t"
          "asr     %w[left], %w[left], %[bits]\n\t"
          "eor     %w[left], %w[left], %[max]\n\t"
          "csel    %w[left], %w[left], %w[right], vs\n\t"
          "lsr     %w[left], %w[left], %[ls]"
          : [left] "+r"(l), [right] "+r"(r)
          : [max] "i"(max), [ls] "Ji"(ls), [bits] "Ji"(rs)
          : "cc");
    return l;
}

template <typename T UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(1, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<1>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(1, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    static constexpr auto max = UTL_NUMERIC_maximum(T);
    static constexpr int ls = 3 * CHAR_BIT;
    static constexpr int rs = sizeof(l) * CHAR_BIT - 1;
    __asm("lsl     %w[left], %w[left], %[ls]\n\t"
          "subs    %w[right], %w[left], %w[right], lsl#%[ls]\n\t"
          "asr     %w[left], %w[left], %[bits]\n\t"
          "eor     %w[left], %w[left], %[max]\n\t"
          "csel    %w[left], %w[left], %w[right], vs\n\t"
          "lsr     %w[left], %w[left], %[ls]"
          : [left] "+r"(l), [right] "+r"(r)
          : [max] "i"(max), [ls] "Ji"(ls), [bits] "Ji"(rs)
          : "cc");
    return l;
}

} // namespace runtime
} // namespace sub_sat
} // namespace details
UTL_NAMESPACE_END

#  else // UTL_ARCH_AARCH64

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace runtime {

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_sized_signed_integral(4, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<4>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(4, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    __asm("qsub    %[left], %[left], %[right]" : [left] "+r"(l) : [right] "r"(r) : "cc");
    return l;
}

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_sized_signed_integral(2, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;

template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<2>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(2, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    __asm("qsub16    %[left], %[left], %[right]" : [left] "+r"(l) : [right] "r"(r) : "cc");
    return l;
}

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_sized_signed_integral(1, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<1>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(1, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    __asm("qsub8    %[left], %[left], %[right]" : [left] "+r"(l) : [right] "r"(r) : "cc");
    return l;
}

} // namespace runtime
} // namespace sub_sat
} // namespace details
UTL_NAMESPACE_END

#  endif // UTL_ARCH_AARCH64

#endif // UTL_SUPPORTS_GNU_ASM

#if UTL_ARCH_AARCH64 & UTL_SIMD_ARM_NEON

#  include <arm_neon.h>

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace runtime {
__UTL_HIDE_FROM_ABI
int64_t impl(int64_t l, int64_t r) noexcept {
    return vqsubd_s64(l, r);
}

__UTL_HIDE_FROM_ABI
int32_t impl(int32_t l, int32_t r) noexcept {
    return vqsubs_s32(l, r);
}

__UTL_HIDE_FROM_ABI
int16_t impl(int16_t l, int16_t r) noexcept {
    return vqsubh_s16(l, r);
}

__UTL_HIDE_FROM_ABI
int8_t impl(int8_t l, int8_t r) noexcept {
    return vqsubb_s8(l, r);
}
} // namespace runtime
} // namespace sub_sat
} // namespace details
UTL_NAMESPACE_END

#endif

// TODO find out how to deal with MSVC ARM32
