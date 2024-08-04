// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_bit_width.h"
#include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace bit {

template <typename T>
UTL_REQUIRES_CXX20(requires(T x) {
    { +x } -> same_as<T>;
})
UTL_ATTRIBUTE(ALWAYS_INLINE) inline constexpr auto bit_ceil(T x) noexcept
    -> UTL_ENABLE_IF_CXX11(T, UTL_SCOPE is_same<T, decltype(+x)>::value) {
    return T(1) << bit_width(T(x - 1));
}

template <typename T>
UTL_ATTRIBUTE(ALWAYS_INLINE) inline constexpr auto bit_ceil(T x) noexcept
    -> UTL_ENABLE_IF_CXX11(T, !UTL_SCOPE is_same<T, decltype(+x)>::value) {
    return T(1u << (bit_width(T(x - 1)) + details::bit::unsigned_width_diff<T>()) >>
        details::bit::unsigned_width_diff<T>());
}

} // namespace bit
} // namespace details

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) inline constexpr UTL_ENABLE_IF_CXX11(T, UTL_TRAIT_is_bit_readable(T)) bit_ceil(T x) noexcept {
    return x <= 1u ? T(1) : details::bit::bit_ceil(x);
}

UTL_NAMESPACE_END
