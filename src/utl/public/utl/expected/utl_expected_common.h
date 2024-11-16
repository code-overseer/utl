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
namespace expected {

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_type : false_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_type<T const> : is_type<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_type<T volatile> : is_type<T> {};
template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_type<expected<T, U>> : true_type {};

#if UTL_CXX14

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_type_v = false;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_type_v<T const> = is_type_v<T>;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_type_v<T volatile> = is_type_v<T>;
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_type_v<expected<T, U>> = true;

#  define __UTL_TRAIT_is_expected(...) __UTL details::expected::is_type_v<__VA_ARGS__>
#else
#  define __UTL_TRAIT_is_expected(...) __UTL details::expected::is_type<__VA_ARGS__>::value
#endif

} // namespace expected
} // namespace details

template <typename E>
class __UTL_PUBLIC_TEMPLATE expected<void const, E> : expected<void, E> {};
template <typename E>
class __UTL_PUBLIC_TEMPLATE expected<void volatile, E> : expected<void, E> {};
template <typename E>
class __UTL_PUBLIC_TEMPLATE expected<void const volatile, E> : expected<void, E> {};

struct transforming_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr transforming_t() noexcept = default;
};
static constexpr transforming_t transforming{};
struct transforming_error_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr transforming_t() noexcept = default;
};
static constexpr transforming_error_t transforming_error{};

UTL_NAMESPACE_END
