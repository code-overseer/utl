// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_common_type.h"
#include "utl/type_traits/utl_make_signed.h"

UTL_NAMESPACE_BEGIN
template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto size(T& t) noexcept(noexcept(t.size()))
    -> decltype(t.size()) {
    return t.size();
}

template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto size(T const& t) noexcept(
    noexcept(t.size())) -> decltype(t.size()) {
    return t.size();
}

template <typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto ssize(T const& t)
    -> common_type_t<ptrdiff_t, make_signed_t<decltype(t.size())>>;

template <typename T, size_t N>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr size_t size(T (&)[N]) noexcept {
    return N;
}

template <typename T, ptrdiff_t N>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr ptrdiff_t ssize(T const (&)[N]) noexcept {
    return N;
}
UTL_NAMESPACE_END
