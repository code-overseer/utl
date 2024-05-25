// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX17

#  include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

using size_t = decltype(sizeof(0));

template <auto V>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr integral_constant<size_t, V> as_index_constant() noexcept {
    return {};
}

template <auto V>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr integral_constant<decltype(V), V> as_integral_constant() noexcept {
    return {};
}

template <auto V>
using index_constant_t = decltype(as_index_constant<V>());
template <auto V>
using integral_constant_t = decltype(as_integral_constant<V>());

UTL_NAMESPACE_END

#endif // if UTL_CXX17
