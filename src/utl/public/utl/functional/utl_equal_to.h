// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"

#include "utl/type_traits/utl_declval.h"

UTL_NAMESPACE_BEGIN

template <typename T = void>
struct equal_to {
    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr bool operator()(
        T const& lhs, T const& rhs) const
        noexcept(noexcept(UTL_SCOPE declval<T const&>() == UTL_SCOPE declval<T const&>())) {
        return lhs == rhs;
    }
};

template <>
struct equal_to<void> {
    template <typename T, typename U>
    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto operator()(T&& lhs, U&& rhs) const
        noexcept(noexcept(UTL_SCOPE declval<T>() == UTL_SCOPE declval<U>()))
            -> decltype(UTL_SCOPE declval<T>() == UTL_SCOPE declval<U>()) {
        return UTL_SCOPE forward<T>(lhs) == UTL_SCOPE forward<U>(rhs);
    }
};

UTL_NAMESPACE_END
