// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

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
namespace compile_time {
template <typename T>
class signed_impl {
    using value_type = UTL_SCOPE remove_cv_t<T>;
    static constexpr value_type max = UTL_NUMERIC_maximum(value_type);
    static constexpr value_type shift = CHAR_BIT * sizeof(value_type) - 1;

public:
    __UTL_HIDE_FROM_ABI
    constexpr signed_impl(T left, T right)
        : signed_impl(left >> shift, right, evaluate(left, right)) {}

    __UTL_HIDE_FROM_ABI
    constexpr value_type result() const noexcept { return result_; }

    __UTL_HIDE_FROM_ABI
    static constexpr value_type get_saturation(value_type left) noexcept {
        return saturation(left >> shift);
    }

private:
    __UTL_HIDE_FROM_ABI
    static constexpr value_type saturation(value_type left_sign) noexcept {
        return left_sign ^ max;
    }

    __UTL_HIDE_FROM_ABI
    static constexpr value_type evaluate(value_type left, value_type right) noexcept {
        return UTL_SCOPE to_signed(UTL_SCOPE to_unsigned(left) + UTL_SCOPE to_unsigned(right));
    }

    __UTL_HIDE_FROM_ABI
    static constexpr bool is_saturated(
        value_type left_sign, value_type right_sign, value_type result_sign) noexcept {
        return left_sign == right_sign && (left_sign ^ result_sign);
    }

    __UTL_HIDE_FROM_ABI
    constexpr signed_impl(value_type left_sign, value_type right, value_type intermediate)
        : result_(is_saturated(left_sign, right >> shift, intermediate >> shift)
                  ? saturation(left_sign)
                  : intermediate) {}

    value_type result_;
};

template <UTL_CONCEPT_CXX20(signed_integral) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_signed(T) && UTL_TRAIT_is_integral(T))>
__UTL_HIDE_FROM_ABI constexpr T impl(T left, T right) noexcept {
    return signed_impl<T>(left, right).result();
}

#if UTL_HAS_BUILTIN(__builtin_saddll_overflow)
__UTL_HIDE_FROM_ABI
constexpr long long impl(long long left, long long right) noexcept {
    return __builtin_saddll_overflow(left, right, &right)
        ? signed_impl<long long>::get_saturation(left)
        : right;
}
#endif

#if UTL_HAS_BUILTIN(__builtin_saddl_overflow)
__UTL_HIDE_FROM_ABI
constexpr long impl(long left, long right) noexcept {
    return __builtin_saddl_overflow(left, right, &right) ? signed_impl<long>::get_saturation(left)
                                                         : right;
}
#endif

#if UTL_HAS_BUILTIN(__builtin_sadd_overflow)
__UTL_HIDE_FROM_ABI
constexpr int impl(int left, int right) noexcept {
    return __builtin_sadd_overflow(left, right, &right) ? signed_impl<int>::get_saturation(left)
                                                        : right;
}
#endif
} // namespace compile_time
} // namespace add_sat
} // namespace details
UTL_NAMESPACE_END

#define UTL_NUMERIC_PRIVATE_HEADER_GUARD
#if UTL_ARCH_x86
#  include "utl/numeric/x86/utl_add_sat.h"
#elif UTL_ARCH_ARM
#  include "utl/numeric/arm/utl_add_sat.h"
#endif
#undef UTL_NUMERIC_PRIVATE_HEADER_GUARD

UTL_NAMESPACE_BEGIN
namespace details {
namespace add_sat {
namespace runtime {
template <typename T>
__UTL_HIDE_FROM_ABI auto has_overload_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename T>
using has_overload = decltype(UTL_SCOPE details::add_sat::runtime::has_overload_impl<T>(0));

template <UTL_CONCEPT_CXX20(signed_integral) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_signed(T) && UTL_TRAIT_is_integral(T) && !has_overload<T>::value)>
__UTL_HIDE_FROM_ABI T impl(T left, T right) noexcept {
    return UTL_SCOPE details::add_sat::compile_time::impl(left, right);
}

} // namespace runtime

template <UTL_CONCEPT_CXX20(signed_integral) T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_signed(T) && UTL_TRAIT_is_integral(T) && !runtime::has_overload<T>::value)>
__UTL_HIDE_FROM_ABI constexpr T impl(T left, T right) noexcept {
    return UTL_CONSTANT_P(left == right)
        ? UTL_SCOPE details::add_sat::compile_time::impl(left, right)
        : UTL_SCOPE details::add_sat::runtime::impl(left, right);
}

template <typename T>
__UTL_HIDE_FROM_ABI constexpr T saturate(T result, T left) noexcept {
    return result | -(result < left);
}

template <UTL_CONCEPT_CXX20(unsigned_integral) T UTL_REQUIRES_CXX11(UTL_TRAIT_is_unsigned(T))>
__UTL_HIDE_FROM_ABI constexpr T impl(T left, T right) noexcept {
    return UTL_SCOPE details::add_sat::saturate(left + right, left);
}

} // namespace add_sat
} // namespace details

template <UTL_CONCEPT_CXX20(saturatable) T UTL_REQUIRES_CXX11(UTL_TRAIT_is_saturatable(T))>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, FLATTEN) constexpr T add_sat(T left, T right) noexcept {
    return UTL_SCOPE details::add_sat::impl(left, right);
}

UTL_NAMESPACE_END
