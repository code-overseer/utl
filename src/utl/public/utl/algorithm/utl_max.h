// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/utility/utl_move.h"

namespace std {
/* UTL_UNDEFINED_BEHAVIOUR */
template <typename>
class initializer_list;
} // namespace std

UTL_NAMESPACE_BEGIN

template <typename T>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr T const& max(T const& l, T const& r) noexcept(
    noexcept(l < r)) {
    return l < r ? r : l;
}

template <typename T, typename F>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr T const& max(
    T const& l, T const& r, F compare) noexcept(noexcept(compare(l, r))) {
    return compare(l, r) ? r : l;
}

namespace details {
namespace algorithm {
template <typename T>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD) constexpr T const& max(T* output, T* current, T* end) {
    return current == end ? *output : max(UTL_SCOPE max(*output, *current), current + 1, end);
}
template <typename T, typename F>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD) constexpr T const& max(T* output, T* current, F&& compare, T* end) {
    return current == end
        ? *output
        : max(compare(*output, *current) ? *current : *output, current + 1, compare, end);
}
} // namespace algorithm
} // namespace details

template <typename T>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD) constexpr T max(std::initializer_list<T> list) noexcept(
    noexcept(*list.begin() < *list.begin() && UTL_TRAIT_is_nothrow_copy_constructible(T))) {
    return max(list.begin(), list.begin() + 1, list.end());
}

template <typename T, typename F>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD) constexpr T max(std::initializer_list<T> list, F comp) noexcept(
    noexcept(comp(*list.begin(), *list.begin()))) {
    return max(list.begin(), list.begin() + 1, UTL_SCOPE move(comp), list.end());
}

UTL_NAMESPACE_END
