// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_countl_zero.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace bit {
template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) inline constexpr T bit_log2(T x) noexcept {
    return CHAR_BIT * sizeof(T) - 1 - UTL_SCOPE countl_zero(x);
}
} // namespace bit
} // namespace details

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) inline constexpr UTL_ENABLE_IF_CXX11(int, UTL_TRAIT_is_bit_readable(T)) bit_width(T x) noexcept {
    return x == 0 ? 0 : details::bit::bit_log2(x) + 1;
}

UTL_NAMESPACE_END
