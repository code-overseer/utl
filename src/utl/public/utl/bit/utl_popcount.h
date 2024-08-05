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

UTL_NODISCARD constexpr int builtin_popcount(unsigned long long x) noexcept {
    return __builtin_popcountll(x);
}

UTL_NODISCARD constexpr int builtin_popcount(unsigned long x) noexcept {
    return __builtin_popcountl(x);
}

UTL_NODISCARD constexpr int builtin_popcount(unsigned int x) noexcept {
    return __builtin_popcount(x);
}

UTL_NODISCARD constexpr int builtin_popcount(unsigned short x) noexcept {
    return __builtin_popcount(x);
}

UTL_NODISCARD constexpr int builtin_popcount(unsigned char x) noexcept {
    return __builtin_popcount(x);
}

#  if UTL_SUPPORTS_INT128
struct builtin_popcount128 {
    constexpr builtin_popcount128(__uint128_t x) : result(compute(x)) {}
    constexpr operator int() const noexcept { return result; }

private:
    static constexpr __uint128_t mask() noexcept { return (__uint128_t(1) << 64) - 1; }

    static constexpr int compute(__uint128_t x) noexcept {
        return compute((unsigned long long)(x & mask()), (unsigned long long)(x >> 64));
    }

    static constexpr int compute(unsigned long long hi, unsigned long long low) noexcept {
        return builtin_popcount(hi) + builtin_popcount(low);
    }

    int result;
};

UTL_NODISCARD inline constexpr int builtin_popcount(__uint128_t x) noexcept {
    return builtin_popcount128(x);
}
#  endif

#elif UTL_COMPILER_MSVC

namespace compile_time {
template <typename T>
constexpr int popcount(T x, int c = 0) noexcept {
    return !x ? c : popcount(x & (x - 1), c + 1);
}
} // namespace compile_time

UTL_NODISCARD constexpr int builtin_popcount(unsigned long long x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::popcount(x) : __popcnt64(x);
}

UTL_NODISCARD constexpr int builtin_popcount(unsigned long x) noexcept {
    static_assert(sizeof(x) == 4, "MSVC");
    return UTL_CONSTANT_P(x) ? compile_time::popcount(x) : __popcnt(x);
}

UTL_NODISCARD constexpr int builtin_popcount(unsigned int x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::popcount(x) : __popcnt(x);
}

UTL_NODISCARD constexpr int builtin_popcount(unsigned short x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::popcount(x) : __popcnt(x);
}

UTL_NODISCARD constexpr int builtin_popcount(unsigned char x) noexcept {
    return UTL_CONSTANT_P(x) ? compile_time::popcount(x) : __popcnt(x);
}
#endif

} // namespace details
} // namespace bit

template <UTL_CONCEPT_CXX20(bit_readable) T>
UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) constexpr UTL_ENABLE_IF_CXX11(int, UTL_TRAIT_is_bit_readable(T)) popcount(T x) noexcept {
    return bit::details::builtin_popcount(x);
}

UTL_NAMESPACE_END
