// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_bit_width.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr UTL_ENABLE_IF_CXX11(T, UTL_TRAIT_VALUE(is_bit_readable, T)) bit_ceil(T x) noexcept {
    return x <= 1u ? T(1)
        : UTL_TRAIT_VALUE(is_same, T, decltype(+x))
        ? T(1) << bit_width(T(x - 1))
        : T(1u << (bit_width(T(x - 1)) + unsigned_width_diff<T>()) >> unsigned_width_diff<T>());
}

UTL_NAMESPACE_END
