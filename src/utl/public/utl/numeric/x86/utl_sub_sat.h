// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#ifndef UTL_NUMERIC_PRIVATE_HEADER_GUARD
#  error "Private header accessed"
#endif

#if !UTL_ARCH_x86
#  error "This header is only available on x86 targets"
#endif // UTL_ARCH_x86

#include "utl/numeric/utl_limits.h"
#include "utl/numeric/utl_sized_integral.h"

#if UTL_SUPPORTS_GNU_ASM

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace runtime {

#  if UTL_ARCH_x86_64
template <typename T UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<8>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
T impl(T l, T r) noexcept {
    auto sat = l;
    // Need to move imm64 to a register before it is usable
    auto const max = UTL_NUMERIC_max(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    __asm__("sarq    %[bits], %[sat]\n\t"
            "xorq    %[max], %[sat]\n\t"
            "subq    %[right], %[left]\n\t"
            "cmovnoq %[left],  %[sat]"
            : [sat] "+r"(sat), [left] "+r"(l)
            : [right] "r"(r), [max] "r"(max), [bits] "Ji"(shift)
            : "cc");
    return sat;
}
#  endif // UTL_ARCH_x86_64

template <typename T UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(4, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<4>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(4, T))>
T impl(T l, T r) noexcept {
    static constexpr auto max = UTL_NUMERIC_max(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    auto sat = l;
    __asm__("sarl    %[bits], %[sat]\n\t"
            "xorl    %[max], %[sat]\n\t"
            "subl    %[right], %[left]\n\t"
            "cmovnol %[left], %[sat]"
            : [left] "+r"(l), [sat] "+r"(sat)
            : [max] "i"(max), [bits] "Ji"(shift), [right] "r"(r)
            : "cc");
    return sat;
}

template <typename T UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<2>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
T impl(T l, T r) noexcept {
    static constexpr auto max = UTL_NUMERIC_max(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    auto sat = l;
    __asm__("sarw    %[bits], %[sat]\n\t"
            "xorw    %[max], %[sat]\n\t"
            "subw    %[right], %[left]\n\t"
            "cmovnow %[left], %[sat]"
            : [left] "+r"(l), [sat] "+r"(sat)
            : [max] "i"(max), [bits] "Ji"(shift), [right] "r"(r)
            : "cc");
    return sat;
}

template <typename T UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(1, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<1>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(1, T))>
T impl(T l, T r) noexcept {
    using x86w = int16_t;
    static constexpr x86w max = UTL_NUMERIC_max(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    x86w sat = l;
    x86w wleft;
    __asm__("sarw    %[bits], %[sat]\n\t"
            "xorw    %[max], %[sat]\n\t"
            "subb    %[right], %[left]\n\t"
            "movsbw  %[left], %[wleft]\n\t"
            "cmovnow %[wleft], %[sat]"
            : [left] "+r"(l), [sat] "+r"(sat), [wleft] "=r"(wleft)
            : [max] "i"(max), [bits] "Ji"(shift), [right] "r"(r)
            : "cc");
    return sat;
}

} // namespace runtime
} // namespace sub_sat
} // namespace details

UTL_NAMESPACE_END

#elif UTL_COMPILER_MSVC // UTL_SUPPORTS_GNU_ASM

#  include <intrin.h>

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace runtime {

#  if UTL_ARCH_x86_64
template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_sized_signed_integral(8, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<8>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(8, T))>
T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_max(T);
    UTL_SCOPE make_unsigned_t<T> tmp;
    return !_subborrow_u64(0, l, r, &tmp) ? (T)tmp : sat;
}
#  endif // UTL_ARCH_x86_64

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_sized_signed_integral(4, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<4>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(4, T))>
T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_max(T);
    UTL_SCOPE make_unsigned_t<T> tmp;
    return !_subborrow_u32(0, l, r, &tmp) ? (T)tmp : sat;
}

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_sized_signed_integral(2, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<2>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(2, T))>
T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_max(T);
    UTL_SCOPE make_unsigned_t<T> tmp;
    return !_subborrow_u16(0, l, r, &tmp) ? (T)tmp : sat;
}

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_sized_signed_integral(1, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<1>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(1, T))>
T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_max(T);
    UTL_SCOPE make_unsigned_t<T> tmp;
    return !_subborrow_u8(0, l, r, &tmp) ? (T)tmp : sat;
}
} // namespace runtime
} // namespace sub_sat
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_SUPPORTS_GNU_ASM
