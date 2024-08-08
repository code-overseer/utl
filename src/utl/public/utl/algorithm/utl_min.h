// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/utility/utl_move.h"

namespace std {
/* UTL_UNDEFINED_BEHAVIOUR */
template <typename>
class UTL_PUBLIC_TEMPLATE initializer_list;
} // namespace std

UTL_NAMESPACE_BEGIN

template <typename T>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr T const& min(T const& l, T const& r) noexcept(
    noexcept(l < r)) {
    return l < r ? l : r;
}

template <typename T, typename F>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr T const& min(
    T const& l, T const& r, F compare) noexcept(noexcept(compare(l, r))) {
    return compare(l, r) ? l : r;
}

namespace details {
namespace algorithm {

template <typename T>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD) constexpr T const& min(T* output, T* current, T* end) {
    return current == end ? *output : min(UTL_SCOPE min(*output, *current), current + 1, end);
}
template <typename T, typename F>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD) constexpr T const& min(T* output, T* current, F&& compare, T* end) {
    return current == end
        ? *output
        : min(compare(*output, *current) ? *output : *current, current + 1, compare, end);
}

} // namespace algorithm
} // namespace details

template <typename T>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD) constexpr T min(std::initializer_list<T> list) noexcept(
    noexcept(*list.begin() < *list.begin())) {
    return min(list.begin(), list.begin() + 1, list.end());
}

template <typename T, typename F>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD) constexpr T min(std::initializer_list<T> list, F comp) noexcept(
    noexcept(comp(*list.begin(), *list.begin()))) {
    return min(list.begin(), list.begin() + 1, UTL_SCOPE move(comp), list.end());
}

UTL_NAMESPACE_END
