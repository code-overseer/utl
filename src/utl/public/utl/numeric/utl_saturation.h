// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/concepts/utl_integral.h"
#include "utl/string/utl_is_string_char.h"
#include "utl/type_traits/utl_is_boolean.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename T>
concept saturatable = integral<T> && !string_char<T> && !is_boolean_v<T>;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_saturatable : bool_constant<saturatable<T>> {};

template <typename T>
inline constexpr bool is_saturatable_v = saturatable<T>;

UTL_NAMESPACE_END

#  define UTL_TRAIT_is_saturatable(...) UTL_SCOPE saturatable<T>

#else

UTL_NAMESPACE_BEGIN

namespace details {
namespace saturation {

template <typename T>
UTL_HIDE_FROM_ABI auto trait_impl(float) noexcept -> UTL_SCOPE false_type;

template <typename T>
UTL_HIDE_FROM_ABI auto trait_impl(int) noexcept
    -> UTL_SCOPE bool_constant<UTL_TRAIT_is_integral(T) &&
        !UTL_SCOPE disjunction<UTL_SCOPE is_string_char<T>, UTL_SCOPE is_boolean<T>>::value>;

template <typename T>
using trait UTL_NODEBUG = decltype(UTL_SCOPE details::saturation::trait_impl<T>(0));

} // namespace saturation
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_saturatable : details::saturation::trait<T> {};

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_saturatable_v = details::saturation::trait<T>::value;

UTL_NAMESPACE_END

#  define UTL_TRAIT_is_saturatable(...) UTL_SCOPE details::saturation::trait<T>::value

#endif
