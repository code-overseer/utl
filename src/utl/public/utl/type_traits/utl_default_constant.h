
// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE default_constant {
    static constexpr T value{};
    using value_type = T;
    using type = default_constant;
    __UTL_HIDE_FROM_ABI constexpr operator value_type() const noexcept { return value; }
    __UTL_HIDE_FROM_ABI constexpr value_type operator()() const noexcept { return value; }
};

template <typename T>
constexpr T default_constant<T>::value;

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr T default_constant_v{};

#  define UTL_TRAIT_default_constant(...) __UTL default_constant_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_default_constant(...) __UTL default_constant<__VA_ARGS__>::value
#endif

UTL_NAMESPACE_END
