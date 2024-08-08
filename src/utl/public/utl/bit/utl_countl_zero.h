// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_popcount.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace bit {

#if UTL_HAS_BUILTIN(__builtin_clz)

#  if !UTL_HAS_BUILTIN(__builtin_clzl)
#    error Unexpected configuration
#  endif

#  if !UTL_HAS_BUILTIN(__builtin_clzll)
#    error Unexpected configuration
#  endif

UTL_NODISCARD inline constexpr int builtin_clz(unsigned long long x) noexcept {
    return x ? __builtin_clzll(x) : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD inline constexpr int builtin_clz(unsigned long x) noexcept {
    return x ? __builtin_clzl(x) : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD inline constexpr int builtin_clz(unsigned x) noexcept {
    return x ? __builtin_clz(x) : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD inline constexpr int builtin_clz(unsigned short x) noexcept {
    return x ? __builtin_clz(x) - unsigned_width_diff<unsigned short>() : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD inline constexpr int builtin_clz(unsigned char x) noexcept {
    return x ? __builtin_clz(x) - unsigned_width_diff<unsigned char>() : CHAR_BIT * sizeof(x);
}

#  if UTL_SUPPORTS_INT128
struct builtin_clz128 {
    constexpr builtin_clz128(__uint128_t x) : result(compute(x)) {}
    constexpr operator int() const noexcept { return result; }

private:
    static constexpr __uint128_t mask() noexcept { return (__uint128_t(1) << 64) - 1; }

    static constexpr int compute(__uint128_t x) noexcept {
        return compute((unsigned long long)(x & mask()), (unsigned long long)(x >> 64));
    }

    static constexpr int compute(unsigned long long hi, unsigned long long low) noexcept {
        return hi == 0 ? 64 + builtin_clz(low) : builtin_clz(hi);
    }

    int result;
};

UTL_NODISCARD inline constexpr int builtin_clz(__uint128_t x) noexcept {
    return builtin_clz128(x);
}
#  endif

#elif UTL_COMPILER_MSVC

namespace compile_time {
template <typename T, unsigned int N = sizeof(T) * CHAR_BIT>
inline constexpr int builtin_clz(T x, unsigned int i = 1) noexcept {
    return i == N ? N - popcount(x) : builtin_clz(T(x | (x >> i)), i << 1);
}
} // namespace compile_time

namespace runtime {
UTL_NODISCARD inline int builtin_clz(unsigned long long x, unsigned long index = 0) noexcept {
    return _BitScanReverse64(&index, x) ? index : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD inline int builtin_clz(unsigned long x, unsigned long index = 0) noexcept {
    return _BitScanReverse(&index, x) ? index : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD inline int builtin_clz(unsigned int x, unsigned long index = 0) noexcept {
    return _BitScanReverse(&index, x) ? index : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD inline int builtin_clz(unsigned short x, unsigned long index = 0) noexcept {
    return _BitScanReverse(&index, x) ? index - unsigned_width_diff<unsigned short>()
                                      : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD inline int builtin_clz(unsigned char x, unsigned long index = 0) noexcept {
    return _BitScanReverse(&index, x) ? index - unsigned_width_diff<unsigned char>()
                                      : CHAR_BIT * sizeof(x);
}
} // namespace runtime

template <typename T>
UTL_NODISCARD inline constexpr int builtin_clz(T x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::builtin_clz(x) : runtime::builtin_clz(x);
}

#endif

} // namespace bit
} // namespace details

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) inline constexpr auto countl_zero(T x) noexcept -> UTL_ENABLE_IF_CXX11(int, UTL_TRAIT_is_bit_readable(T)) {
    return details::bit::builtin_clz(x);
}

UTL_NAMESPACE_END
