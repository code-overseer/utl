// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/utility/utl_declval.h"
#include "utl/utility/utl_signs.h"
#include "utl_sequence.h"

#include <cstdint>

UTL_NAMESPACE_BEGIN

namespace seq {
namespace details {
template <typename T>
UTL_ATTRIBUTES(NODISCARD)
UTL_CONSTEVAL T pow(T n, size_t p) noexcept {
    return !p ? 1 : ((p & 1) ? n : 1) * pow(n * n, p >> 1);
}
} // namespace details

template <typename T, T... Is>
using array_t = T const[sizeof...(Is)];

template <typename T, T... Is>
UTL_INLINE_CXX17 constexpr array_t<T, Is...> array{Is...};

template <typename T, T N>
using scalar_t = integral_constant<T, N>;

template <uint8_t N>
using uscalar8_t = scalar_t<decltype(N), N>;
template <uint16_t N>
using uscalar16_t = scalar_t<decltype(N), N>;
template <uint32_t N>
using uscalar32_t = scalar_t<decltype(N), N>;
template <uint64_t N>
using uscalar64_t = scalar_t<decltype(N), N>;

template <int8_t N>
using scalar8_t = scalar_t<decltype(N), N>;
template <int16_t N>
using scalar16_t = scalar_t<decltype(N), N>;
template <int32_t N>
using scalar32_t = scalar_t<decltype(N), N>;
template <int64_t N>
using scalar64_t = scalar_t<decltype(N), N>;

template <make_signed_t<size_t> N>
using sindex_t = scalar_t<decltype(N), N>;
template <size_t N>
using index_t = scalar_t<decltype(N), N>;

template <typename T, size_t N>
using range_t = make_integer_sequence<T, N>;
template <typename T, T... Is>
using sequence_t = integer_sequence<T, Is...>;

template <typename T, size_t N>
UTL_INLINE_CXX17 constexpr range_t<T, N> range = {};
template <typename T, T... Is>
UTL_INLINE_CXX17 constexpr sequence_t<T, Is...> sequence = {};
template <typename T, T N>
UTL_INLINE_CXX17 constexpr scalar_t<T, N> scalar = {};
template <size_t N>
UTL_INLINE_CXX17 constexpr index_t<N> index = {};

template <typename T, T I>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto to_sequence(scalar_t<T, I>) noexcept
    -> sequence_t<T, I> {
    return {};
}
template <typename T, T... Is>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto to_array(sequence_t<T, Is...>) noexcept
    -> array_t<T, Is...> const& {
    return array<T, Is...>;
}

#define UTL_BINOP_RESULT(OP, L, R) decltype(declval<L>() OP declval<R>())
#define UTL_DEFINE_OP(NAME, SYM)                                                     \
    template <typename T, T... Is, typename U, U J>                                  \
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)                                  \
    constexpr auto NAME(sequence_t<T, Is...>, scalar_t<U, J>) noexcept               \
        -> sequence_t<UTL_BINOP_RESULT(SYM, T, U), (Is SYM J)...> {                  \
        return {};                                                                   \
    }                                                                                \
    template <typename T, T... Is, typename U, U... Js>                              \
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)                                  \
    constexpr auto NAME(sequence_t<T, Is...>, sequence_t<U, Js...>) noexcept         \
        -> sequence_t<UTL_BINOP_RESULT(SYM, T, U), (Is SYM Js)...> {                 \
        return {};                                                                   \
    }                                                                                \
    template <typename T, T... Is, typename U, U J>                                  \
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)                                  \
    constexpr auto operator SYM(sequence_t<T, Is...>, scalar_t<U, J>) noexcept       \
        -> sequence_t<UTL_BINOP_RESULT(SYM, T, U), (Is SYM J)...> {                  \
        return {};                                                                   \
    }                                                                                \
    template <typename T, T... Is, typename U, U... Js>                              \
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)                                  \
    constexpr auto operator SYM(sequence_t<T, Is...>, sequence_t<U, Js...>) noexcept \
        -> sequence_t<UTL_BINOP_RESULT(SYM, T, U), (Is SYM Js)...> {                 \
        return {};                                                                   \
    }

UTL_DEFINE_OP(add, +)
UTL_DEFINE_OP(sub, -)
UTL_DEFINE_OP(mul, *)
UTL_DEFINE_OP(div, /)
UTL_DEFINE_OP(mod, %)
UTL_DEFINE_OP(bw_ls, <<)
UTL_DEFINE_OP(bw_rs, >>)
UTL_DEFINE_OP(bw_or, |)
UTL_DEFINE_OP(bw_and, &)
UTL_DEFINE_OP(bw_xor, ^)
#undef UTL_DEFINE_OP
#undef UTL_BINOP_RESULT

template <typename T, T I>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto bw_neg(scalar_t<T, I>) noexcept
    -> scalar_t<decltype(~I), ~I> {
    return {};
}
template <typename T, T... Is>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto bw_neg(sequence_t<T, Is...>) noexcept
    -> sequence_t<decltype(~declval<T>()), (~Is)...> {
    return {};
}
template <typename T, T I>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto operator~(scalar_t<T, I>) noexcept
    -> scalar_t<decltype(~I), ~I> {
    return {};
}
template <typename T, T... Is>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto operator~(sequence_t<T, Is...>) noexcept
    -> sequence_t<decltype(~declval<T>()), (~Is)...> {
    return {};
}

UTL_INLINE_CXX17 constexpr size_t npos = (size_t)-1;

template <typename T, size_t N>
UTL_INLINE_CXX17 constexpr auto zero = bw_xor(range<T, N>, range<T, N>);

template <size_t N, typename T, T... Is>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr T access(sequence_t<T, Is...>) noexcept {
    return array<T, Is...>[N];
}

template <typename T, T... Is, size_t... Js>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto shuffle(sequence_t<T, Is...>,
    sequence_t<size_t, Js...>) noexcept -> sequence_t<T, array<T, Is...>[Js]...> {
    return {};
}
template <typename T, T... Is, size_t J>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto pow(sequence_t<T, Is...>, index_t<J>) noexcept
    -> sequence_t<T, details::pow(Is, J)...> {
    return {};
}
template <typename T, T... Is, size_t... Js>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto pow(sequence_t<T, Is...>,
    sequence_t<size_t, Js...>) noexcept -> sequence_t<T, details::pow(Is, Js)...> {
    return {};
}

template <size_t N, typename T, T I>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto repeat(scalar_t<T, I>) noexcept
    -> decltype(bw_or(zero<T, N>, scalar<T, I>)) {
    return {};
}
template <size_t N, typename T, T... Is>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto repeat(sequence_t<T, Is...>) noexcept
    -> decltype(shuffle(
        sequence<T, Is...>, div(range<size_t, N * sizeof...(Is)>, index<sizeof...(Is)>))) {
    return {};
}
template <size_t N, typename T, T... Is>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto tile(sequence_t<T, Is...>) noexcept
    -> decltype(shuffle(
        sequence<T, Is...>, mod(range<size_t, N * sizeof...(Is)>, index<sizeof...(Is)>))) {
    return {};
}
template <typename T, T... Is>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto flip(sequence_t<T, Is...>) noexcept
    -> decltype(shuffle(
        sequence<T, Is...>, sub(index<sizeof...(Is)>, range<size_t, sizeof...(Is)>))) {
    return {};
}

template <typename T, T... Is, T... Js>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto concat(
    sequence_t<T, Is...>, sequence_t<T, Js...>) noexcept -> sequence_t<T, Is..., Js...> {
    return {};
}
} // namespace seq
UTL_NAMESPACE_END