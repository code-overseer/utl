// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/type_traits/utl_declval.h"

UTL_NAMESPACE_BEGIN

template <typename T = void>
struct UTL_PUBLIC_TEMPLATE less {
    UTL_HIDE_FROM_ABI constexpr bool operator()(T const& lhs, T const& rhs) const
        noexcept(declval<T const&>() < declval<T const&>()) {
        return lhs < rhs;
    }
};

template <>
struct UTL_PUBLIC_TEMPLATE less<void> {
    template <typename T, typename U>
    UTL_HIDE_FROM_ABI constexpr auto operator()(T&& lhs, U&& rhs) const
        noexcept(declval<T>() < declval<U>()) -> decltype(declval<T>() < declval<U>()) {
        return forward<T>(lhs) < forward<U>(rhs);
    }
};

UTL_NAMESPACE_END
