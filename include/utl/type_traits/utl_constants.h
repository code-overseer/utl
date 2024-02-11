// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"

UTL_NAMESPACE_BEGIN

using size_t = decltype(sizeof(0));

template<typename T, T N>
struct integral_constant {
    static constexpr T value = N;
    using value_type = T;
    using type = integral_constant;
    constexpr operator value_type() const noexcept { return N; }
    constexpr value_type operator()() const noexcept { return N; }
};

template<typename T, T N>
using value_constant = integral_constant<T, N>;

template<size_t N>
using size_constant = integral_constant<size_t, N>;

using npos_type = size_constant<(size_t)-1>;

template<bool B>
using bool_constant = integral_constant<bool, B>;
using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

#ifdef UTL_CXX14
template<typename T, T N>
UTL_INLINE_CXX17 constexpr T integral_constant_v = N;

template<typename T, T N>
UTL_INLINE_CXX17 constexpr T value_constant_v = N;
#endif

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_integral_constant 1
#define UTL_TRAIT_SUPPORTED_bool_constant 1
#define UTL_TRAIT_SUPPORTED_true_type 1
#define UTL_TRAIT_SUPPORTED_false_type 1
