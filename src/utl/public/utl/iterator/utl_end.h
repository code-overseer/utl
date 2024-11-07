// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN
template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto end(T& t) noexcept(noexcept(t.end()))
    -> decltype(t.end()) {
    return t.end();
}

template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto end(T const& t) noexcept(noexcept(t.end()))
    -> decltype(t.end()) {
    return t.end();
}

template <typename T, size_t N>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T* end(T (&a)[N]) noexcept {
    return a + N;
}

template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto cend(T const& t) noexcept(
    noexcept(__UTL end(t))) -> decltype(__UTL end(t)) {
    return __UTL end(t);
}

UTL_NAMESPACE_END
