// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_declval.h"

UTL_NAMESPACE_BEGIN

template <typename T = void>
struct equal_to {
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr bool operator()(
        T const& lhs, T const& rhs) const
        noexcept(noexcept(__UTL declval<T const&>() == __UTL declval<T const&>())) {
        return lhs == rhs;
    }
};

template <>
struct equal_to<void> {
    template <typename T, typename U>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto operator()(T&& lhs, U&& rhs) const
        noexcept(noexcept(__UTL declval<T>() == __UTL declval<U>()))
            -> decltype(__UTL declval<T>() == __UTL declval<U>()) {
        return __UTL forward<T>(lhs) == __UTL forward<U>(rhs);
    }
};

UTL_NAMESPACE_END
