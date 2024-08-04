// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/bit/utl_common.h"

UTL_NAMESPACE_BEGIN

namespace bit {
namespace details {

#if UTL_HAS_BUILTIN(__builtin_popcount)

#  if !UTL_HAS_BUILTIN(__builtin_popcountll)
#    error Unexpected configuration
#  endif

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_popcount(unsigned long long x) noexcept {
    return __builtin_popcountll(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_popcount(unsigned long x) noexcept {
    return __builtin_popcountl(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_popcount(unsigned int x) noexcept {
    return __builtin_popcount(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_popcount(unsigned short x) noexcept {
    return __builtin_popcount(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_popcount(unsigned char x) noexcept {
    return __builtin_popcount(x);
}

#elif UTL_COMPILER_MSVC

namespace compile_time {
template <typename T>
constexpr int popcount(T x, int c = 0) noexcept {
    return !x ? c : popcount(x & (x - 1), c + 1);
}
} // namespace compile_time

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_popcount(unsigned long long x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::popcount(x) : __popcnt64(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_popcount(unsigned long x) noexcept {
    static_assert(sizeof(x) == 4, "MSVC");
    return UTL_CONSTANT_P(x) ? compile_time::popcount(x) : __popcnt(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_popcount(unsigned int x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::popcount(x) : __popcnt(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_popcount(unsigned short x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::popcount(x) : __popcnt(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_popcount(unsigned char x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::popcount(x) : __popcnt(x);
}
#endif

} // namespace details
} // namespace bit

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST) constexpr UTL_ENABLE_IF_CXX11(bool, is_bit_readable<T>::value) popcount(T x) noexcept {
    return bit::details::builtin_popcount(x);
}

UTL_NAMESPACE_END
