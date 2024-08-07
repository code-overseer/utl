// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_countl_zero.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) constexpr auto countl_one(T x) noexcept -> UTL_ENABLE_IF_CXX11(int, UTL_TRAIT_is_bit_readable(T)) {
    return details::bit::builtin_clz(T(~x));
}

UTL_NAMESPACE_END
