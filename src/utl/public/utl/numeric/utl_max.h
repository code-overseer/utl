// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/concepts/utl_arithmetic_type.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_arithmetic.h"

UTL_NAMESPACE_BEGIN

namespace numeric {
template <UTL_CONCEPT_CXX20(arithmetic_type) T>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr UTL_ENABLE_IF_CXX11(T, is_arithmetic<T>::value) max(T l, T r) noexcept {
    return l > r ? l : r;
}
} // namespace numeric

UTL_NAMESPACE_END
