// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN
template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto begin(T& t) noexcept(noexcept(t.begin()))
    -> decltype(t.begin()) {
    return t.begin();
}

template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto begin(T const& t) noexcept(
    noexcept(t.begin())) -> decltype(t.begin()) {
    return t.begin();
}

template <typename T, size_t N>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T* begin(T (&a)[N]) noexcept {
    return a;
}

template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto cbegin(T const& t) noexcept(
    noexcept(__UTL begin(t))) -> decltype(__UTL begin(t)) {
    return __UTL begin(t);
}

UTL_NAMESPACE_END
