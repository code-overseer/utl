// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/initializer_list/utl_initializer_list_fwd.h"

UTL_NAMESPACE_BEGIN
template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto data(T& t) noexcept(noexcept(t.data()))
    -> decltype(t.data()) {
    return t.data();
}

template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto data(T const& t) noexcept(
    noexcept(t.data())) -> decltype(t.data()) {
    return t.data();
}

template <typename T, size_t N>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T* data(T (&a)[N]) noexcept {
    return a;
}

template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const* data(
    ::std::initializer_list<T> list) noexcept {
    return list.data();
}

UTL_NAMESPACE_END
