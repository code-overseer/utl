// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_floating_point.h"
#include "utl/type_traits/utl_is_integral.h"
#include "utl/type_traits/utl_is_signed.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_undefined_trait.h"
#include "utl/utl_float.h"

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
struct __UTL_PUBLIC_TEMPLATE min_impl<T, true, true> {
    static constexpr T value = T(T(1) << (CHAR_BIT * sizeof(T) - 1));
};
template <typename T>
constexpr T min_impl<T, true, true>::value;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE min_impl<T, true, false> {
    static constexpr T value = 0;
};
template <typename T>
constexpr T min_impl<T, true, false>::value;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE max_impl<T, true, true> {
    static constexpr T value = ~min_impl<T>::value;
};
template <typename T>
constexpr T max_impl<T, true, true>::value;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE max_impl<T, true, false> {
    static constexpr T value = T(-1);
};
template <typename T>
constexpr T max_impl<T, true, false>::value;

template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE max_impl<bool, A, B> {
    static constexpr bool value = true;
};
template <bool A, bool B>
constexpr bool max_impl<bool, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE min_impl<bool, A, B> {
    static constexpr bool value = false;
};
template <bool A, bool B>
constexpr bool min_impl<bool, A, B>::value;

template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE max_impl<float, A, B> {
    static constexpr float value = FLT_MAX;
};
template <bool A, bool B>
constexpr float max_impl<float, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE min_impl<float, A, B> {
    static constexpr float value = FLT_MIN;
};
template <bool A, bool B>
constexpr float min_impl<float, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE max_impl<double, A, B> {
    static constexpr double value = DBL_MAX;
};
template <bool A, bool B>
constexpr double max_impl<double, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE min_impl<double, A, B> {
    static constexpr double value = DBL_MIN;
};
template <bool A, bool B>
constexpr double min_impl<double, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE max_impl<long double, A, B> {
    static constexpr long double value = LDBL_MAX;
};
template <bool A, bool B>
constexpr long double max_impl<long double, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE min_impl<long double, A, B> {
    static constexpr long double value = LDBL_MIN;
};
template <bool A, bool B>
constexpr long double min_impl<long double, A, B>::value;

#if UTL_SUPPORTS_FLOAT16
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE max_impl<float16, A, B> {
    static constexpr float16 value = 6.5504e+4f16;
};
template <bool A, bool B>
constexpr float16 max_impl<float16, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE min_impl<float16, A, B> {
    static constexpr float16 value = 6.103515625e-5f16;
};
template <bool A, bool B>
constexpr float16 min_impl<float16, A, B>::value;
#endif
#if UTL_SUPPORTS_FLOAT32
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE max_impl<float32, A, B> {
    static constexpr float32 value = 3.40282347e+38f32;
};
template <bool A, bool B>
constexpr float32 max_impl<float32, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE min_impl<float32, A, B> {
    static constexpr float32 value = 1.17549435e-38f32;
};
template <bool A, bool B>
constexpr float32 min_impl<float32, A, B>::value;
#endif
#if UTL_SUPPORTS_FLOAT64
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE max_impl<float64, A, B> {
    static constexpr float64 value = 1.7976931348623157e+308f64;
};
template <bool A, bool B>
constexpr float64 max_impl<float64, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE min_impl<float64, A, B> {
    static constexpr float64 value = 2.2250738585072014e-308f64;
};
template <bool A, bool B>
constexpr float64 min_impl<float64, A, B>::value;
#endif
#if UTL_SUPPORTS_FLOAT128
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE max_impl<float128, A, B> {
    static constexpr float128 value = 1.1897314953572317650857593266280070162e+4932f128;
};
template <bool A, bool B>
constexpr float128 max_impl<float128, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE min_impl<float128, A, B> {
    static constexpr float128 value = 3.3621031431120935062626778173217526026e-4932f128;
};
template <bool A, bool B>
constexpr float128 min_impl<float128, A, B>::value;
#endif
#if UTL_SUPPORTS_BFLOAT16
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE max_impl<bfloat16, A, B> {
    static constexpr bfloat16 value = 3.38953139e+38bf16;
};
template <bool A, bool B>
constexpr bfloat16 max_impl<bfloat16, A, B>::value;
template <bool A, bool B>
struct __UTL_PUBLIC_TEMPLATE min_impl<bfloat16, A, B> {
    static constexpr bfloat16 value = 1.175494351e-38bf16;
};
template <bool A, bool B>
constexpr bfloat16 min_impl<bfloat16, A, B>::value;
#endif

template <typename T, bool = UTL_TRAIT_is_floating_point(T)>
struct fp_lowest;
template <typename T>
struct __UTL_PUBLIC_TEMPLATE fp_lowest<T, true> {
    static constexpr T value = -max_impl<T>::value;
};
template <typename T>
constexpr T fp_lowest<T, true>::value;

} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE maximum : details::max_impl<remove_cv_t<T>> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE minimum : details::min_impl<remove_cv_t<T>> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE lowest : details::min_impl<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE lowest<T const> : lowest<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE lowest<T volatile> : lowest<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE lowest<T const volatile> : lowest<T> {};
template <>
struct __UTL_PUBLIC_TEMPLATE lowest<float> : details::fp_lowest<float> {};
template <>
struct __UTL_PUBLIC_TEMPLATE lowest<double> : details::fp_lowest<double> {};
template <>
struct __UTL_PUBLIC_TEMPLATE lowest<long double> : details::fp_lowest<long double> {};
#if UTL_SUPPORTS_FLOAT16
template <>
struct __UTL_PUBLIC_TEMPLATE lowest<float16> : details::fp_lowest<float16> {};
#endif
#if UTL_SUPPORTS_FLOAT32
template <>
struct __UTL_PUBLIC_TEMPLATE lowest<float32> : details::fp_lowest<float32> {};
#endif
#if UTL_SUPPORTS_FLOAT64
template <>
struct __UTL_PUBLIC_TEMPLATE lowest<float64> : details::fp_lowest<float64> {};
#endif
#if UTL_SUPPORTS_FLOAT128
template <>
struct __UTL_PUBLIC_TEMPLATE lowest<float128> : details::fp_lowest<float128> {};
#endif
#if UTL_SUPPORTS_BFLOAT16
template <>
struct __UTL_PUBLIC_TEMPLATE lowest<bfloat16> : details::fp_lowest<bfloat16> {};
#endif
#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr auto maximum_v = maximum<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr auto minimum_v = minimum<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr auto lowest_v = lowest<T>::value;
#  define UTL_NUMERIC_maximum(TYPE) UTL_SCOPE numeric::maximum_v<TYPE>
#  define UTL_NUMERIC_minimum(TYPE) UTL_SCOPE numeric::minimum_v<TYPE>
#  define UTL_NUMERIC_lowest(TYPE) UTL_SCOPE numeric::lowest_v<TYPE>
#else
#  define UTL_NUMERIC_maximum(TYPE) UTL_SCOPE numeric::maximum<TYPE>::value;
#  define UTL_NUMERIC_minimum(TYPE) UTL_SCOPE numeric::minimum<TYPE>::value;
#  define UTL_NUMERIC_lowest(TYPE) UTL_SCOPE numeric::lowest<TYPE>::value;
#endif
} // namespace numeric

UTL_NAMESPACE_END
