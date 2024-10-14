// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_TARGET_APPLE
#  include <mach/clock.h>
#  include <mach/mach.h>
#  include <mach/mach_time.h>
#  include <sys/time.h>

UTL_NAMESPACE_BEGIN

namespace tempus {
template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<system_clock_t> {
public:
    using clock = system_clock_t;
    using value_type = long long;
    using duration = time_duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        static_assert(sizeof(size_t) >= 8, "Unsupported architecture");
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration difference(
        value_type l, value_type r) noexcept {
        auto const diff = l - r;
        auto const seconds = diff / 1000000;
        return duration{seconds, diff * 1000 - seconds * 1000000000};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr clock_order compare(
        value_type l, value_type r) noexcept {
        return static_cast<clock_order>((l > r) - (l < r));
    }

    __UTL_HIDE_FROM_ABI friend time_point<system_clock_t> get_time(system_clock_t) noexcept {
        timeval t;
        gettimeofday(&t, nullptr);
        return time_point<system_clock_t>{value_type(t.tv_sec * 1000000) + value_type(t.tv_usec)};
    }
};

__UTL_HIDE_FROM_ABI constexpr ::time_t to_posix_time(time_point<system_clock_t> t) noexcept {
    // microseconds to seconds
    return t.value() / 1000000LL;
}

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<steady_clock_t> {
public:
    using clock = steady_clock_t;
    using value_type = unsigned long long;
    using duration = time_duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        static_assert(sizeof(size_t) >= 8, "Unsupported architecture");
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline duration difference(
        value_type l, value_type r) noexcept {
        static mach_timebase_info_data_t const timebase = []() {
            mach_timebase_info_data_t t;
            mach_timebase_info(&t);
            return t;
        }();

        auto const diff = (l - r) * timebase.numer / timebase.denom;
        auto const seconds = diff / 1000000000;
        auto const nanoseconds = diff - seconds * 1000000000;
        using diff_type = long long;
        return duration{diff_type(seconds), diff_type(nanoseconds)};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr clock_order compare(
        value_type l, value_type r) noexcept {
        return static_cast<clock_order>((l > r) - (l < r));
    }

    __UTL_HIDE_FROM_ABI friend time_point<steady_clock_t> get_time(steady_clock_t) noexcept {
        return time_point<steady_clock_t>{mach_continuous_time()};
    }
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<high_resolution_clock_t> :
    private clock_traits<steady_clock_t> {
    using base_type = clock_traits<steady_clock_t>;

public:
    using clock = high_resolution_clock_t;
    using typename base_type::duration;
    using typename base_type::value_type;

    using base_type::compare;
    using base_type::difference;
    using base_type::equal;
    using base_type::time_since_epoch;

    __UTL_HIDE_FROM_ABI friend time_point<high_resolution_clock_t> get_time(
        high_resolution_clock_t) noexcept {
        return time_point<high_resolution_clock_t>{get_time(steady_clock).value()};
    }
};

} // namespace tempus

UTL_NAMESPACE_END

#endif //  UTL_TARGET_APPLE
