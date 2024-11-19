// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if !defined(UTL_EXPECTED_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

UTL_NAMESPACE_BEGIN

template <typename T, typename E>
class __UTL_PUBLIC_TEMPLATE expected;

namespace details {
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_expected_type : false_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_expected_type<T const> : is_expected_type<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_expected_type<T volatile> : is_expected_type<T> {};
template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_expected_type<__UTL expected<T, U>> : true_type {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_expected_type_v = false;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_expected_type_v<T const> = is_expected_type_v<T>;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_expected_type_v<T volatile> = is_expected_type_v<T>;
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_expected_type_v<__UTL expected<T, U>> = true;

#  define __UTL_TRAIT_is_expected(...) __UTL details::is_expected_type_v<__VA_ARGS__>
#else
#  define __UTL_TRAIT_is_expected(...) __UTL details::is_expected_type<__VA_ARGS__>::value
#endif

namespace expected {
struct transforming_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr transforming_t() noexcept = default;
};
UTL_INLINE_CXX17 constexpr transforming_t transforming{};
struct transforming_error_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr transforming_error_t() noexcept = default;
};
UTL_INLINE_CXX17 constexpr transforming_error_t transforming_error{};

struct converting_t {
    __UTL_HIDE_FROM_ABI explicit constexpr converting_t() noexcept = default;
};
UTL_INLINE_CXX17 constexpr converting_t converting{};
struct empty_t {
    __UTL_HIDE_FROM_ABI inline constexpr empty_t() noexcept = default;
    template <typename... Ts>
    __UTL_HIDE_FROM_ABI inline constexpr empty_t(Ts&&...) noexcept {}
};
UTL_INLINE_CXX17 constexpr empty_t empty{};
} // namespace expected
} // namespace details

UTL_NAMESPACE_END
