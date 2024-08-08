// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_bit_width.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) inline constexpr auto bit_floor(T x) noexcept -> UTL_ENABLE_IF_CXX11(T, UTL_TRAIT_is_bit_readable(T)) {
    return x ? (T(1) << (bit_width(x) - 1)) : 0;
}

UTL_NAMESPACE_END
