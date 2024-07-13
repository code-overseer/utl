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
namespace sub_sat {

template <typename T>
constexpr T saturate(T result, T left) noexcept {
    return result & -(result <= left);
}

template <UTL_CONCEPT_CXX20(unsigned_integral) T UTL_REQUIRES_CXX11(UTL_TRAIT_is_unsigned(T))>
constexpr T impl(T left, T right) noexcept {
    return UTL_SCOPE details::sub_sat::saturate(left - right, left);
}

namespace compile_time {
template <typename T>
class signed_impl {
    using value_type = UTL_SCOPE remove_cv_t<T>;
    static constexpr value_type max = UTL_NUMERIC_max(T);
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
        return UTL_SCOPE to_signed(UTL_SCOPE to_unsigned(left) - UTL_SCOPE to_unsigned(right));
    }
    static constexpr bool is_saturated(
        value_type left_sign, value_type right_sign, value_type result_sign) noexcept {
        return (left_sign ^ right_sign) & (left_sign ^ result_sign);
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

#if UTL_HAS_BUILTIN(__builtin_ssubll_overflow)
constexpr long long impl(long long left, long long right) noexcept {
    return __builtin_ssubll_overflow(left, right, &right)
        ? signed_impl<long long>::get_saturation(left)
        : right;
}
#endif

#if UTL_HAS_BUILTIN(__builtin_ssubl_overflow)
constexpr long impl(long left, long right) noexcept {
    return __builtin_ssubl_overflow(left, right, &right) ? signed_impl<long>::get_saturation(left)
                                                         : right;
}
#endif

#if UTL_HAS_BUILTIN(__builtin_ssub_overflow)
constexpr int impl(int left, int right) noexcept {
    return __builtin_ssub_overflow(left, right, &right) ? signed_impl<int>::get_saturation(left)
                                                        : right;
}
#endif
} // namespace compile_time
} // namespace sub_sat
} // namespace details
UTL_NAMESPACE_END

#if UTL_ARCH_x86

#  if UTL_SUPPORTS_GNU_ASM

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace runtime {

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
            "subq    %[right], %[left]\n\t"
            "cmovnoq %[left],  %[sat]"
            : [sat] "+r"(sat), [left] "+r"(l)
            : [right] "r"(r), [max] "r"(max), [bits] "Ji"(shift)
            : "cc");
    return sat;
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
            "subl    %[right], %[left]\n\t"
            "cmovnol %[left], %[sat]"
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
            "subw    %[right], %[left]\n\t"
            "cmovnow %[left], %[sat]"
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

#  elif UTL_COMPILER_MSVC // UTL_SUPPORTS_GNU_ASM

#    include <intrin.h>

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace runtime {

#    if UTL_ARCH_x86_64
template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<8>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<8>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_max(T);
    UTL_SCOPE make_unsigned_t<T> tmp;
    return !_subborrow_u64(0, l, r, &tmp) ? (T)tmp : sat;
}
#    endif // UTL_ARCH_x86_64

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<4>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(4, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<4>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(4, T))>
T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_max(T);
    UTL_SCOPE make_unsigned_t<T> tmp;
    return !_subborrow_u32(0, l, r, &tmp) ? (T)tmp : sat;
}

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<2>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<2>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_max(T);
    UTL_SCOPE make_unsigned_t<T> tmp;
    return !_subborrow_u16(0, l, r, &tmp) ? (T)tmp : sat;
}

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<1>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(1, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<1>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(1, T))>
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

#  endif // UTL_SUPPORTS_GNU_ASM

#elif UTL_ARCH_ARM

#  if UTL_SIMD_ARM_NEON
#    include <arm_neon.h>
#  endif

#  if UTL_SUPPORTS_GNU_ASM

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace runtime {

#    if UTL_ARCH_AARCH64

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<8>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
auto has_overload_impl(int) noexcept -> UTL_SCOPE true_type;

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<8>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
T impl(T l, T r) noexcept {
    static constexpr auto min = UTL_NUMERIC_min(T);
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

#    endif

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
    static constexpr auto min = UTL_NUMERIC_min(T);
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

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<2>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
T impl(T l, T r) noexcept {
    using w_type = int32_t;
    static constexpr auto min = UTL_NUMERIC_min(T);
    static constexpr int ls = 2 * CHAR_BIT;
    static constexpr int rs = sizeof(l) * CHAR_BIT - 1;
    __asm("lsl     %w[left], %w[left], %w[ls]\n\t"
          "lsl     %w[right], %w[right], %w[ls]\n\t"
          "subs    %w[left], %w[left], %w[right]\n\t"
          "asr     %w[right], %w[right], %w[bits]\n\t"
          "eor     %w[right], %w[right], %w[min]\n\t"
          "csel    %w[left], %w[right], %w[left], vs\n\t"
          "lsr     %w[left], %w[left], %w[ls]"
          : [left] "+r"(l), [right] "+r"(r)
          : [min] "i"(min), [ls] "Ji"(ls), [bits] "Ji"(rs)
          : "cc");
    return l;
}

template <UTL_CONCEPT_CXX20(UTL_SCOPE sized_signed_integral<1>) T
        UTL_REQUIRES_CXX11( UTL_TRAIT_is_sized_signed_integral(1, T))>
T impl(T l, T r) noexcept {
    using w_type = int32_t;
    static constexpr auto min = UTL_NUMERIC_min(T);
    static constexpr int ls = 3 * CHAR_BIT;
    static constexpr int rs = sizeof(l) * CHAR_BIT - 1;
    __asm("lsl     %w[left], %w[left], %w[ls]\n\t"
          "lsl     %w[right], %w[right], %w[ls]\n\t"
          "subs    %w[left], %w[left], %w[right]\n\t"
          "asr     %w[right], %w[right], %w[bits]\n\t"
          "eor     %w[right], %w[right], %w[min]\n\t"
          "csel    %w[left], %w[right], %w[left], vs\n\t"
          "lsr     %w[left], %w[left], %w[ls]"
          : [left] "+r"(l), [right] "+r"(r)
          : [min] "i"(min), [ls] "Ji"(ls), [bits] "Ji"(rs)
          : "cc");
    return l;
}

#    if UTL_SIMD_ARM_NEON
int16_t impl(int16_t l, int16_t r) noexcept {
    return vqsubh_s16(l, r);
}

int8_t impl(int8_t l, int8_t r) noexcept {
    return vqsubb_s8(l, r);
}
#    endif

} // namespace runtime
} // namespace sub_sat
} // namespace details
UTL_NAMESPACE_END

#  elif UTL_SIMD_ARM_NEON

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace runtime {
int64_t impl(int64_t l, int64_t r) noexcept {
    return vqsubd_s64(l, r);
}

int32_t impl(int32_t l, int32_t r) noexcept {
    return vqsubs_s32(l, r);
}

int16_t impl(int16_t l, int16_t r) noexcept {
    return vqsubh_s16(l, r);
}

int8_t impl(int8_t l, int8_t r) noexcept {
    return vqsubb_s8(l, r);
}
} // namespace runtime
} // namespace sub_sat
} // namespace details
UTL_NAMESPACE_END

#  endif //  UTL_SUPPORTS_GNU_ASM

#endif

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace runtime {
template <typename T>
auto has_overload_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename T>
using has_overload = decltype(UTL_SCOPE details::sub_sat::runtime::has_overload_impl<T>(0));

template <UTL_CONCEPT_CXX20(signed_integral) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_signed(T) && UTL_TRAIT_is_integral(T) && !has_overload<T>::value)>
T impl(T left, T right) noexcept {
    return UTL_SCOPE details::sub_sat::compile_time::impl(left, right);
}

} // namespace runtime

template <UTL_CONCEPT_CXX20(signed_integral) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_signed(T) && UTL_TRAIT_is_integral(T) && !has_overload<T>::value)>
constexpr T impl(T left, T right) noexcept {
    return UTL_CONSTANT_P(left == right)
        ? UTL_SCOPE details::sub_sat::compile_time::impl(left, right)
        : UTL_SCOPE details::sub_sat::runtime::impl(left, right);
}

} // namespace sub_sat
} // namespace details

template <UTL_CONCEPT_CXX20(saturatable) T UTL_REQUIRES_CXX11(UTL_TRAIT_is_saturatable(T))>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T sub_sat(T left, T right) noexcept {
    return UTL_SCOPE details::sub_sat::impl(left, right);
}

UTL_NAMESPACE_END
