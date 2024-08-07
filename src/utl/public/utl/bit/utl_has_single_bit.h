// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_popcount.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr auto has_single_bit(T x) noexcept -> UTL_ENABLE_IF_CXX11(bool, UTL_TRAIT_is_bit_readable(T)) {
    return x && (x & (x - 1)) == 0;
}

UTL_NAMESPACE_END
