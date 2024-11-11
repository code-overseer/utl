// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_is_destructible.h"

UTL_STD_NAMESPACE_BEGIN
template <typename T>
struct unexpect_t;
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_ABI_PUBLIC unexpect_t {
    __UTL_HIDE_FROM_ABI explicit constexpr unexpect_t() noexcept = default;
};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr unexpect_t unexpect{};
#endif

template <typename E>
class unexpected {
    static_assert(UTL_TRAIT_is_destructible(E), "Invalid type");
};

namespace details {
namespace unexpected {

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_type : false_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_type<T const> : is_type<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_type<T volatile> : is_type<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_type<unexpected<T>> : true_type {};

#if UTL_CXX14

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_type_v = false;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_type_v<T const> = is_type_v<T>;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_type_v<T volatile> = is_type_v<T>;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_type_v<unexpected<T>> = true;

#  define __UTL_TRAIT_is_unexpected(...) __UTL details::unexpected::is_type_v<__VA_ARGS__>
#else
#  define __UTL_TRAIT_is_unexpected(...) __UTL details::unexpected::is_type<__VA_ARGS__>::value
#endif
} // namespace unexpected
} // namespace details

UTL_NAMESPACE_END
