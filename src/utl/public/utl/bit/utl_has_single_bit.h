// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_popcount.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr UTL_ENABLE_IF_CXX11(bool, is_bit_readable<T>::value) has_single_bit(T x) noexcept {
    return x && (x & (x - 1)) == 0;
}

UTL_NAMESPACE_END
