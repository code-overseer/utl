// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/concepts/utl_signed_integral.h"
#include "utl/concepts/utl_unsigned_integral.h"
#include "utl/numeric/utl_limits.h"
#include "utl/numeric/utl_saturation.h"
#include "utl/numeric/utl_sized_integral.h"
#include "utl/type_traits/utl_is_signed.h"
#include "utl/type_traits/utl_is_unsigned.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/utility/utl_constant_p.h"
#include "utl/utility/utl_signs.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace add_sat {

template <typename T>
constexpr T saturate(T result, T left) noexcept {
    return result | -(result < left);
}

template <UTL_CONCEPT_CXX20(unsigned_integral) T UTL_REQUIRES_CXX11(UTL_TRAIT_is_unsigned(T))>
constexpr T impl(T left, T right) noexcept {
    return UTL_SCOPE details::add_sat::saturate(left + right, left);
}

namespace compile_time {
template <typename T>
class signed_impl {
    using value_type = UTL_SCOPE remove_cv_t<T>;
    static constexpr value_type max = UTL_NUMERIC_max(value_type);
    static constexpr value_type shift = CHAR_BIT * sizeof(value_type) - 1;

public:
    constexpr signed_impl(T left, T right)
        : signed_impl(left >> shift, right, evaluate(left, right)) {}

    constexpr value_type result() const noexcept { return result_; }

    static constexpr value_type get_saturation(value_type left) noexcept {
        return saturation(left >> shift);
    }

private:
    static constexpr value_type saturation(value_type left_sign) noexcept {
        return left_sign ^ max;
    }
    static constexpr value_type evaluate(value_type left, value_type right) noexcept {
        return UTL_SCOPE to_signed(UTL_SCOPE to_unsigned(left) + UTL_SCOPE to_unsigned(right));
    }
    static constexpr bool is_saturated(
        value_type left_sign, value_type right_sign, value_type result_sign) noexcept {
        return left_sign == right_sign && (left_sign ^ result_sign);
    }

    constexpr signed_impl(value_type left_sign, value_type right, value_type intermediate)
        : result_(is_saturated(left_sign, right >> shift, intermediate >> shift)
                  ? saturation(left_sign)
                  : intermediate) {}

    value_type result_;
};

template <UTL_CONCEPT_CXX20(signed_integral) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_signed(T) && UTL_TRAIT_is_integral(T))>
constexpr T impl(T left, T right) noexcept {
    return signed_impl<T>(left, right).result();
}

#if UTL_HAS_BUILTIN(__builtin_saddll_overflow)
constexpr long long impl(long long left, long long right) noexcept {
    return __builtin_saddll_overflow(left, right, &right)
        ? signed_impl<long long>::get_saturation(left)
        : right;
}
#endif

#if UTL_HAS_BUILTIN(__builtin_saddl_overflow)
constexpr long impl(long left, long right) noexcept {
    return __builtin_saddl_overflow(left, right, &right) ? signed_impl<long>::get_saturation(left)
                                                         : right;
}
#endif

#if UTL_HAS_BUILTIN(__builtin_sadd_overflow)
constexpr int impl(int left, int right) noexcept {
    return __builtin_sadd_overflow(left, right, &right) ? signed_impl<int>::get_saturation(left)
                                                        : right;
}
#endif
} // namespace compile_time

namespace runtime {

#if UTL_SUPPORTS_GNU_ASM

template <typename T>
auto has_overload_impl(float) noexcept -> UTL_SCOPE false_type;

#  if UTL_ARCH_x86
#    if UTL_ARCH_x86_64

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<8>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
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
            "addq    %[right], %[left]\n\t"
            "cmovoq  %[sat],  %[left]"
            : [sat] "+r"(sat), [left] "+r"(l)
            : [right] "r"(r), [max] "r"(max), [bits] "Ji"(shift)
            : "cc");
    return l;
}
#    endif // UTL_ARCH_x86_64

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<4>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(4, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<2>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<1>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(1, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<4>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(4, T))>
T impl(T l, T r) noexcept {
    static constexpr auto max = UTL_NUMERIC_max(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    auto sat = l;
    __asm__("sarl    %[bits], %[sat]\n\t"
            "xorl    %[max], %[sat]\n\t"
            "addl    %[right], %[left]\n\t"
            "cmovnol  %[left], %[sat]"
            : [left] "+r"(l), [sat] "+r"(sat)
            : [max] "i"(max), [bits] "Ji"(shift), [right] "r"(r)
            : "cc");
    return sat;
}

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<2>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
T impl(T l, T r) noexcept {
    static constexpr auto max = UTL_NUMERIC_max(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    auto sat = l;
    __asm__("sarw    %[bits], %[sat]\n\t"
            "xorw    %[max], %[sat]\n\t"
            "addw    %[right], %[left]\n\t"
            "cmovnow  %[left], %[sat]"
            : [left] "+r"(l), [sat] "+r"(sat)
            : [max] "i"(max), [bits] "Ji"(shift), [right] "r"(r)
            : "cc");
    return sat;
}

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
            "addb    %[right], %[left]\n\t"
            "movsbw  %[left], %[wleft]\n\t"
            "cmovnow %[wleft], %[sat]"
            : [left] "+r"(l), [sat] "+r"(sat), [wleft] "=r"(wleft)
            : [max] "i"(max), [bits] "Ji"(shift), [right] "r"(r)
            : "cc");
    return sat;
}

#  elif UTL_ARCH_ARM
#    if UTL_ARCH_AARCH64

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<8>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(8, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<8>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(8, T))>
T impl(T l, T r) noexcept {
    static constexpr auto max = UTL_NUMERIC_max(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    __asm("adds    %x[left], %x[left], %x[right]\n\t"
          "asr     %x[right], %x[right], %x[bits]\n\t"
          "eor     %x[right], %x[right], %x[max]\n\t"
          "csel    %x[left], %x[right], %x[left], vs"
          : [left] "+r"(l), [right] "+r"(r)
          : [max] "i"(max), [bits] "Ji"(shift)
          : "cc");
    return l;
}

#    endif

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<4>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(4, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<2>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(2, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<1>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(1, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<4>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(4, T))>
T impl(T l, T r) noexcept {
    static constexpr auto max = UTL_NUMERIC_max(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    __asm("adds    %w[left], %w[left], %w[right]\n\t"
          "asr     %w[right], %w[right], %w[bits]\n\t"
          "eor     %w[right], %w[right], %w[max]\n\t"
          "csel    %w[left], %w[right], %w[left], vs"
          : [left] "+r"(l), [right] "+r"(r)
          : [max] "i"(max), [bits] "Ji"(shift)
          : "cc");
    return l;
}

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<2>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(2, T))>
T impl(T l, T r) noexcept {
    using w_type = int32_t;
    static constexpr auto max = UTL_NUMERIC_max(w_type);
    static constexpr int ls = 2 * CHAR_BIT;
    static constexpr int rs = sizeof(l) * CHAR_BIT - 1;
    __asm("lsl     %w[left], %w[left], %w[ls]\n\t"
          "lsl     %w[right], %w[right], %w[ls]\n\t"
          "adds    %w[left], %w[left], %w[right]\n\t"
          "asr     %w[right], %w[right], %w[bits]\n\t"
          "eor     %w[right], %w[right], %w[max]\n\t"
          "csel    %w[left], %w[right], %w[left], vs\n\t"
          "lsr     %w[left], %w[left], %w[ls]"
          : [left] "+r"(l), [right] "+r"(r)
          : [max] "i"(max), [ls] "Ji"(ls), [bits] "Ji"(rs)
          : "cc");
    return l;
}

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<1>) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_sized_signed_integral(1, T))>
T impl(T l, T r) noexcept {
    using w_type = int32_t;
    static constexpr auto max = UTL_NUMERIC_max(w_type);
    static constexpr int ls = 3 * CHAR_BIT;
    static constexpr int rs = sizeof(l) * CHAR_BIT - 1;
    __asm("lsl     %w[left], %w[left], %w[ls]\n\t"
          "lsl     %w[right], %w[right], %w[ls]\n\t"
          "adds    %w[left], %w[left], %w[right]\n\t"
          "asr     %w[right], %w[right], %w[bits]\n\t"
          "eor     %w[right], %w[right], %w[max]\n\t"
          "csel    %w[left], %w[right], %w[left], vs\n\t"
          "lsr     %w[left], %w[left], %w[ls]"
          : [left] "+r"(l), [right] "+r"(r)
          : [max] "i"(max), [ls] "Ji"(ls), [bits] "Ji"(rs)
          : "cc");
    return l;
}

#  endif

#endif // UTL_SUPPORTS_GNU_ASM

template <typename T>
using has_overload = decltype(UTL_SCOPE details::add_sat::runtime::has_overload_impl<T>(0));

template <UTL_CONCEPT_CXX20(signed_integral) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_signed(T) && UTL_TRAIT_is_integral(T) && !has_overload<T>::value)>
T impl(T left, T right) noexcept {
    return UTL_SCOPE details::add_sat::compile_time::impl(left, right);
}

} // namespace runtime

template <UTL_CONCEPT_CXX20(signed_integral) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_signed(T) && UTL_TRAIT_is_integral(T) && !has_overload<T>::value)>
constexpr T impl(T left, T right) noexcept {
    return UTL_CONSTANT_P(left == right)
        ? UTL_SCOPE details::add_sat::compile_time::impl(left, right)
        : UTL_SCOPE details::add_sat::runtime::impl(left, right);
}

} // namespace add_sat
} // namespace details

template <UTL_CONCEPT_CXX20(saturatable) T UTL_REQUIRES_CXX11(UTL_TRAIT_is_saturatable(T))>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T add_sat(T left, T right) noexcept {
    return UTL_SCOPE details::add_sat::impl(left, right);
}

UTL_NAMESPACE_END
