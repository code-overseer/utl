// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_TARGET_MICROSOFT

#  include "utl/numeric/utl_sub_sat.h"

UTL_NAMESPACE_BEGIN

namespace platform {
template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<system_clock_t> {
public:
    using clock = system_clock_t;
    using value_type = long long;
    using duration = time_duration;
    using epoch_conversion = integral_constant<value_type, 11644473600LL>;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        // Epoch is 1601-01-01 00:00:00 UTC
        static_assert(sizeof(size_t) >= 8, "Unsupported architecture");
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration difference(
        value_type l, value_type r) noexcept {
        auto const diff = l - r;
        // 100 nanoseconds to seconds
        auto const seconds = diff / 10000000;
        auto const nanoseconds = diff * 100 - seconds * 1000000000;
        return duration{seconds, nanoseconds};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr int compare(
        value_type l, value_type r) noexcept {
        return l < r ? -1 : l > r ? 1 : 0;
    }

    friend __UTL_HIDE_FROM_ABI time_point<system_clock_t> get_time(system_clock_t) noexcept {
        return time_point<system_clock_t>{get_time()};
    }

private:
    __UTL_ABI_PUBLIC static value_type get_time() noexcept;
};

constexpr ::time_t to_posix_time(time_point<system_clock_t> t) noexcept {
    using epoch_shift = typename clock_traits<system_clock_t>::epoch_conversion;
    // 100 nanoseconds to seconds
    return t.value() / 10000000ULL - epoch_shift::value;
}

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<steady_clock_t> {
public:
    using clock = steady_clock_t;
    using value_type = long long;
    using duration = time_duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        static_assert(sizeof(size_t) >= 8, "Unsupported architecture");
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration difference(
        value_type l, value_type r) noexcept {
        auto const diff = l - r;
        auto const seconds = diff / 1000;
        return duration{seconds, diff * 1000000 - seconds * 1000000000};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr int compare(
        value_type l, value_type r) noexcept {
        return l < r ? -1 : l > r ? 1 : 0;
    }

    friend __UTL_HIDE_FROM_ABI time_point<steady_clock_t> get_time(steady_clock_t) noexcept {
        return time_point<steady_clock_t>{get_time()};
    }

private:
    __UTL_ABI_PUBLIC static value_type get_time() noexcept;
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<high_resolution_clock_t> {
public:
    using clock = high_resolution_clock_t;
    using value_type = long long;
    using duration = time_duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST) static duration difference(value_type l, value_type r) noexcept;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr int compare(
        value_type l, value_type r) noexcept {
        return l < r ? -1 : l > r ? 1 : 0;
    }

    friend __UTL_HIDE_FROM_ABI time_point<high_resolution_clock_t> get_time(
        high_resolution_clock_t) noexcept {
        return time_point<high_resolution_clock_t>{get_time()};
    }

private:
    __UTL_ABI_PUBLIC static value_type get_time() noexcept;
};

} // namespace platform

UTL_NAMESPACE_END

#endif //  UTL_TARGET_MICROSOFT
