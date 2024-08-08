// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_popcount.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace bit {

#if UTL_HAS_BUILTIN(__builtin_ctz)

#  if !UTL_HAS_BUILTIN(__builtin_ctzl)
#    error Unexpected configuration
#  endif

#  if !UTL_HAS_BUILTIN(__builtin_ctzll)
#    error Unexpected configuration
#  endif

UTL_NODISCARD constexpr int builtin_ctz(unsigned long long x) noexcept {
    return x ? __builtin_ctzll(x) : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD constexpr int builtin_ctz(unsigned long x) noexcept {
    return x ? __builtin_ctzl(x) : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD constexpr int builtin_ctz(unsigned x) noexcept {
    return x ? __builtin_ctz(x) : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD constexpr int builtin_ctz(unsigned short x) noexcept {
    return x ? __builtin_ctz(x) - unsigned_width_diff<unsigned short>() : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD constexpr int builtin_ctz(unsigned char x) noexcept {
    return x ? __builtin_ctz(x) - unsigned_width_diff<unsigned char>() : CHAR_BIT * sizeof(x);
}

#  if UTL_SUPPORTS_INT128
struct builtin_ctz128 {
    constexpr builtin_ctz128(__uint128_t x) : result(compute(x)) {}
    constexpr operator int() const noexcept { return result; }

private:
    static constexpr __uint128_t mask() noexcept { return (__uint128_t(1) << 64) - 1; }

    static constexpr int compute(__uint128_t x) noexcept {
        return compute((unsigned long long)(x & mask()), (unsigned long long)(x >> 64));
    }

    static constexpr int compute(unsigned long long hi, unsigned long long low) noexcept {
        return low == 0 ? 64 + builtin_clz(hi) : builtin_clz(low);
    }

    int result;
};

UTL_NODISCARD inline constexpr int builtin_ctz(__uint128_t x) noexcept {
    return builtin_ctz128(x);
}
#  endif

#elif UTL_COMPILER_MSVC

namespace compile_time {
template <typename T>
UTL_NODISCARD constexpr int builtin_ctz(T x, unsigned int i = 1) noexcept {
    return x ? (x &= -(int64_t)x),
           ((!(x & 0xFFFFFFFFULL) << 5 | !(x & 0xFFFF0000FFFFULL) << 4 |
                !(x & 0xFF00FF00FF00FFULL) << 3 | !(x & 0xF0F0F0F0F0F0F0FULL) << 2 |
                !(x & 0x3333333333333333ULL) << 1 | !(x & 0x5555555555555555ULL)) +
               !x)
             : CHAR_BIT * sizeof(x);
}
} // namespace compile_time

namespace runtime {
UTL_NODISCARD int builtin_ctz(unsigned long long x, unsigned long index = 0) noexcept {
    return _BitScanForward64(&index, x) ? index : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD int builtin_ctz(unsigned long x, unsigned long index = 0) noexcept {
    return _BitScanForward(&index, x) ? index : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD int builtin_ctz(unsigned int x, unsigned long index = 0) noexcept {
    return _BitScanForward(&index, x) ? index : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD int builtin_ctz(unsigned short x, unsigned long index = 0) noexcept {
    return _BitScanForward(&index, x) ? index : CHAR_BIT * sizeof(x);
}

UTL_NODISCARD int builtin_ctz(unsigned char x, unsigned long index = 0) noexcept {
    return _BitScanForward(&index, x) ? index : CHAR_BIT * sizeof(x);
}
} // namespace runtime

template <typename T>
UTL_NODISCARD constexpr int builtin_ctz(T x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::builtin_ctz(x) : runtime::builtin_ctz(x);
}

#endif

} // namespace bit
} // namespace details

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) constexpr auto countr_zero(T x) noexcept -> UTL_ENABLE_IF_CXX11(int, UTL_TRAIT_is_bit_readable(T)) {
    return details::bit::builtin_ctz(x);
}

UTL_NAMESPACE_END
