// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_countl_zero.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr UTL_ENABLE_IF_CXX11(bool, UTL_TRAIT_VALUE(is_bit_readable, T)) bit_width(T x) noexcept {
    return CHAR_BIT * sizeof(T) - countl_zero(x);
}

UTL_NAMESPACE_END
