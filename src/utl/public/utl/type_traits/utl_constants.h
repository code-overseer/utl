// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"

UTL_NAMESPACE_BEGIN

using size_t = decltype(sizeof(0));

template <typename T, T N>
struct __UTL_PUBLIC_TEMPLATE integral_constant {
    static constexpr T value = N;
    using value_type = T;
    using type = integral_constant;
    UTL_HIDE_FROM_ABI constexpr operator value_type() const noexcept { return N; }
    UTL_HIDE_FROM_ABI constexpr value_type operator()() const noexcept { return N; }
};

template <typename T, T N>
constexpr T integral_constant<T, N>::value;

template <typename T, T N>
using value_constant = integral_constant<T, N>;
template <size_t N>
using size_constant = integral_constant<size_t, N>;
using npos_type = size_constant<(size_t)-1>;
template <bool B>
using bool_constant = integral_constant<bool, B>;
using true_type = bool_constant<true>;
using false_type = bool_constant<false>;
template <typename...>
using always_true_type = true_type;
template <typename...>
using always_false_type = false_type;

template <typename...>
UTL_CONSTEVAL bool always_true() noexcept {
    return true;
}
template <typename...>
UTL_CONSTEVAL bool always_false() noexcept {
    return false;
}

#if UTL_CXX14
template <typename T, T N>
UTL_INLINE_CXX17 constexpr T integral_constant_v = N;
template <typename T, T N>
UTL_INLINE_CXX17 constexpr T value_constant_v = N;
template <typename...>
UTL_INLINE_CXX17 constexpr bool always_true_v = true;
template <typename...>
UTL_INLINE_CXX17 constexpr bool always_false_v = false;
#endif

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_integral_constant 1
#define UTL_TRAIT_SUPPORTED_bool_constant 1
#define UTL_TRAIT_SUPPORTED_true_type 1
#define UTL_TRAIT_SUPPORTED_false_type 1
