// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_popcount.h"

UTL_NAMESPACE_BEGIN

namespace bit {
namespace details {

#if UTL_HAS_BUILTIN(__builtin_clz)

#  if !UTL_HAS_BUILTIN(__builtin_clzl)
#    error Unexpected configuration
#  endif

#  if !UTL_HAS_BUILTIN(__builtin_clzll)
#    error Unexpected configuration
#  endif

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_clz(unsigned long long x) noexcept {
    return x ? __builtin_clzll(x) : CHAR_BIT * sizeof(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_clz(unsigned long x) noexcept {
    return x ? __builtin_clzl(x) : CHAR_BIT * sizeof(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_clz(unsigned x) noexcept {
    return x ? __builtin_clz(x) : CHAR_BIT * sizeof(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_clz(unsigned short x) noexcept {
    return x ? __builtin_clz(x) - unsigned_width_diff<unsigned short>() : CHAR_BIT * sizeof(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr int builtin_clz(unsigned char x) noexcept {
    return x ? __builtin_clz(x) - unsigned_width_diff<unsigned char>() : CHAR_BIT * sizeof(x);
}

#elif UTL_COMPILER_MSVC

namespace compile_time {
template <typename T, unsigned int N = sizeof(T) * CHAR_BIT>
constexpr int builtin_clz(T x, unsigned int i = 1) noexcept {
    return i == N ? N - popcount(x) : builtin_clz(T(x | (x >> i)), i << 1);
}
} // namespace compile_time

namespace runtime {
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) int builtin_clz(
    unsigned long long x, unsigned long index = 0) noexcept {
    return _BitScanReverse64(&index, x) ? index : CHAR_BIT * sizeof(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) int builtin_clz(
    unsigned long x, unsigned long index = 0) noexcept {
    return _BitScanReverse(&index, x) ? index : CHAR_BIT * sizeof(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) int builtin_clz(
    unsigned int x, unsigned long index = 0) noexcept {
    return _BitScanReverse(&index, x) ? index : CHAR_BIT * sizeof(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) int builtin_clz(
    unsigned short x, unsigned long index = 0) noexcept {
    return _BitScanReverse(&index, x) ? index - unsigned_width_diff<unsigned short>()
                                      : CHAR_BIT * sizeof(x);
}

UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) int builtin_clz(
    unsigned char x, unsigned long index = 0) noexcept {
    return _BitScanReverse(&index, x) ? index - unsigned_width_diff<unsigned char>()
                                      : CHAR_BIT * sizeof(x);
}
} // namespace runtime

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST) constexpr int builtin_clz(T x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::builtin_clz(x) : runtime::builtin_clz(x);
}

#endif

} // namespace details
} // namespace bit

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE) constexpr UTL_ENABLE_IF_CXX11(int, UTL_TRAIT_VALUE(is_bit_readable, T)) countl_zero(T x) noexcept {
    return bit::details::builtin_clz(x);
}

UTL_NAMESPACE_END
