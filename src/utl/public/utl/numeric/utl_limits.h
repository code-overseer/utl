// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_integral.h"
#include "utl/type_traits/utl_is_signed.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_undefined_trait.h"
#include "utl/utlfloat.h"

#include <cfloat>
#include <climits>
#include <cstdint>
/**
 * Non-standard limits for internal use
 */

static_assert((-1 & 3) == 3, "Invalid signed representation detected");

UTL_NAMESPACE_BEGIN
namespace numeric {
namespace details {

template <typename T, bool Int = UTL_TRAIT_is_integral(T), bool Signed = UTL_TRAIT_is_signed(T)>
struct max_impl;
template <typename T, bool Int = UTL_TRAIT_is_integral(T), bool Signed = UTL_TRAIT_is_signed(T)>
struct min_impl;

template <typename T>
struct min_impl<T, true, true> {
    static constexpr T value = T(T(1) << (CHAR_BIT * sizeof(T) - 1));
};
template <typename T>
struct min_impl<T, true, false> {
    static constexpr T value = 0;
};
template <typename T>
struct max_impl<T, true, true> {
    static constexpr T value = ~min_impl<T>::value;
};
template <typename T>
struct max_impl<T, true, false> {
    static constexpr T value = T(-1);
};

template <>
struct max_impl<bool> {
    static constexpr bool value = true;
};
template <>
struct min_impl<bool> {
    static constexpr bool value = false;
};

template <>
struct max_impl<float> {
    static constexpr float value = FLT_MAX;
};
template <>
struct min_impl<float> {
    static constexpr float value = FLT_MIN;
};
template <>
struct max_impl<double> {
    static constexpr double value = DBL_MAX;
};
template <>
struct min_impl<double> {
    static constexpr double value = DBL_MIN;
};
template <>
struct max_impl<long double> {
    static constexpr long double value = LDBL_MAX;
};
template <>
struct min_impl<long double> {
    static constexpr long double value = LDBL_MIN;
};

#if UTL_SUPPORTS_FLOAT16
template <>
struct max_impl<float16> {
    static constexpr float16 value = 6.5504e+4f16;
};
template <>
struct min_impl<float16> {
    static constexpr float16 value = 6.103515625e-5f16;
};
#endif
#if UTL_SUPPORTS_FLOAT32
template <>
struct max_impl<float32> {
    static constexpr float32 value = 3.40282347e+38f32;
};
template <>
struct min_impl<float32> {
    static constexpr float32 value = 1.17549435e-38f32;
};
#endif
#if UTL_SUPPORTS_FLOAT64
template <>
struct max_impl<float64> {
    static constexpr float64 value = 1.7976931348623157e+308f64;
};
template <>
struct min_impl<float64> {
    static constexpr float64 value = 2.2250738585072014e-308f64;
};
#endif
#if UTL_SUPPORTS_FLOAT128
template <>
struct max_impl<float128> {
    static constexpr float128 value = 1.1897314953572317650857593266280070162e+4932f128;
};
template <>
struct min_impl<float128> {
    static constexpr float128 value = 3.3621031431120935062626778173217526026e-4932f128;
};
#endif
#if UTL_SUPPORTS_BFLOAT16
template <>
struct max_impl<bfloat16> {
    static constexpr bfloat16 value = 3.38953139e+38bf16;
};
template <>
struct min_impl<bfloat16> {
    static constexpr bfloat16 value = 1.175494351e-38bf16;
};
#endif

} // namespace details

template <typename T>
struct max : details::max_impl<remove_cv_t<T>> {};
template <typename T>
struct min : details::min_impl<remove_cv_t<T>> {};
template <typename T>
struct lowest : details::min_impl<T> {};
template <typename T>
struct lowest<T const> : lowest<T> {};
template <typename T>
struct lowest<T volatile> : lowest<T> {};
template <typename T>
struct lowest<T const volatile> : lowest<T> {};
template <>
struct lowest<float> {
    static constexpr float value = -max<float>::value;
};
template <>
struct lowest<double> {
    static constexpr double value = -max<double>::value;
};
template <>
struct lowest<long double> {
    static constexpr long double value = -max<long double>::value;
};
#if UTL_SUPPORTS_FLOAT16
template <>
struct lowest<float16> {
    static constexpr float16 value = -max<float16>::value;
};
#endif
#if UTL_SUPPORTS_FLOAT32
template <>
struct lowest<float32> {
    static constexpr float32 value = -max<float32>::value;
};
#endif
#if UTL_SUPPORTS_FLOAT64
template <>
struct lowest<float64> {
    static constexpr float64 value = -max<float64>::value;
};
#endif
#if UTL_SUPPORTS_FLOAT128
template <>
struct lowest<float128> {
    static constexpr float128 value = -max<float128>::value;
};
#endif
#if UTL_SUPPORTS_BFLOAT16
template <>
struct lowest<bfloat16> {
    static constexpr bfloat16 value = -max<bfloat16>::value;
};
#endif
#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr auto max_v = max<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr auto min_v = min<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr auto lowest_v = lowest<T>::value;
#  define UTL_NUMERIC_max(TYPE) UTL_SCOPE numeric::max_v<TYPE>
#  define UTL_NUMERIC_min(TYPE) UTL_SCOPE numeric::min_v<TYPE>
#  define UTL_NUMERIC_lowest(TYPE) UTL_SCOPE numeric::lowest_v<TYPE>
#else
#  define UTL_NUMERIC_max(TYPE) UTL_SCOPE numeric::max<TYPE>::value;
#  define UTL_NUMERIC_min(TYPE) UTL_SCOPE numeric::min<TYPE>::value;
#  define UTL_NUMERIC_lowest(TYPE) UTL_SCOPE numeric::lowest<TYPE>::value;
#endif
} // namespace numeric

UTL_NAMESPACE_END
