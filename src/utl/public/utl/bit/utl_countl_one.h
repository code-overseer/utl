// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_countl_zero.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) constexpr UTL_ENABLE_IF_CXX11(int, UTL_TRAIT_is_bit_readable(T)) countl_one(T x) noexcept {
    return bit::details::builtin_clz(T(~x));
}

UTL_NAMESPACE_END
