// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_bit_width.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST) constexpr UTL_ENABLE_IF_CXX11(T, UTL_TRAIT_VALUE(is_bit_readable, T)) bit_ceil(T x) noexcept {
    return x ? (T(1) << (bit_width(x) - 1)) : 0;
}

UTL_NAMESPACE_END
