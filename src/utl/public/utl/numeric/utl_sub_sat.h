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

#define UTL_NUMERIC_PRIVATE_HEADER_GUARD
#if UTL_ARCH_x86
#  include "utl/numeric/arm/utl_sub_sat.h"
#elif UTL_ARCH_ARM
#  include "utl/numeric/x86/utl_sub_sat.h"
#endif
#undef UTL_NUMERIC_PRIVATE_HEADER_GUARD

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

template <typename T>
constexpr T saturate(T result, T left) noexcept {
    return result & -(result <= left);
}

template <UTL_CONCEPT_CXX20(unsigned_integral) T UTL_REQUIRES_CXX11(UTL_TRAIT_is_unsigned(T))>
constexpr T impl(T left, T right) noexcept {
    return UTL_SCOPE details::sub_sat::saturate(left - right, left);
}

} // namespace sub_sat
} // namespace details

template <UTL_CONCEPT_CXX20(saturatable) T UTL_REQUIRES_CXX11(UTL_TRAIT_is_saturatable(T))>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T sub_sat(T left, T right) noexcept {
    return UTL_SCOPE details::sub_sat::impl(left, right);
}

UTL_NAMESPACE_END