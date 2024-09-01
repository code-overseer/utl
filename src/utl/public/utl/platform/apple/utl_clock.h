// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_TARGET_APPLE

#  include <mach/clock.h>
#  include <mach/mach.h>
#  include <sys/time.h>

extern "C" uint64_t mach_continuous_time(void);

UTL_NAMESPACE_BEGIN

namespace platform {
template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<system_clock_t> {
public:
    using clock = system_clock_t;
    using value_type = unsigned long long;
    using duration = time_duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        static_assert(sizeof(size_t) >= 8, "Unsupported architecture");
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration difference(
        value_type l, value_type r) noexcept {
        auto const diff = l - r;
        auto const seconds = diff / 1000000;
        return duration{seconds, diff * 1000 - seconds * 1000000000};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr int compare(
        value_type l, value_type r) noexcept {
        return l < r ? -1 : l > r ? 1 : 0;
    }

    friend time_point<system_clock_t> get_time(system_clock_t) noexcept {
        timeval t;
        gettimeofday(&t, nullptr);
        return time_point<system_clock_t>{value_type(t.tv_sec * 1000000) + value_type(t.tv_usec)};
    }
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<steady_clock_t> {
public:
    using clock = steady_clock_t;
    using value_type = unsigned long long;
    using duration = time_duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        static_assert(sizeof(size_t) >= 8, "Unsupported architecture");
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration difference(
        value_type l, value_type r) noexcept {
        auto const diff = l - r;
        auto const seconds = diff / 10000000;
        return duration{seconds, diff * 100 - seconds * 1000000000};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr int compare(
        value_type l, value_type r) noexcept {
        return l < r ? -1 : l > r ? 1 : 0;
    }

    friend time_point<steady_clock_t> get_time(steady_clock_t) noexcept {
        return time_point<steady_clock_t>{mach_continuous_time()};
    }
};

// template <>
// class __UTL_PUBLIC_TEMPLATE  clock_traits<high_resolution_clock_t> {
// public:
//     using clock = high_resolution_clock_t;
//     using value_type = unsigned long long;
//     using duration = time_duration;

//     UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration
//     time_since_epoch(
//         value_type t) noexcept {
//         static_assert(sizeof(size_t) >= 8, "Unsupported architecture");
//         return difference(t, 0);
//     }

//     UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST) static duration difference(value_type l, value_type r)
//     noexcept;

//     UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
//         value_type l, value_type r) noexcept {
//         return l == r;
//     }

//     UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr int compare(
//         value_type l, value_type r) noexcept {
//         return l < r ? -1 : l > r ? 1 : 0;
//     }

//     friend __UTL_HIDE_FROM_ABI time_point<high_resolution_clock_t> get_time(
//         high_resolution_clock_t) noexcept {
//         return time_point<high_resolution_clock_t>{get_time()};
//     }

// private:
//     UTL_ABI_PUBLIC static value_type get_time() noexcept;
// };

// // TODO move this
// struct rdtscp_t {
//     unsigned long long tick;
//     int aux;
// };

// template <>
// class __UTL_PUBLIC_TEMPLATE  clock_traits<hardware_clock_t> {
// public:
//     using clock = hardware_clock_t;
//     using value_type = rdtscp_t;
//     using duration = hardware_ticks;

//     UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration
//     time_since_epoch(
//         value_type t) noexcept {
//         return duration(t.tick - 0);
//     }

//     UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration
//     difference(
//         value_type l, value_type r) noexcept {
//         return duration(__UTL sub_sat(l.tick, r.tick));
//     }

//     UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
//         value_type l, value_type r) noexcept {
//         return l.aux == r.aux && l.tick == r.tick;
//     }

//     UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr int compare(
//         value_type const& l, value_type const& r) noexcept {

//         if (l.aux == r.aux) {
//             return l.tick < r.tick ? -1 : l.tick > r.tick ? 1 : 0;
//         }

//         return l.aux - r.aux;
//     }

//     friend __UTL_HIDE_FROM_ABI time_point<high_resolution_clock_t> get_time(
//         hardware_ticks) noexcept {
//         return time_point<hardware_clock_t>{get_time()};
//     }

// private:
//     UTL_ABI_PUBLIC static value_type get_time() noexcept;
// };

} // namespace platform

UTL_NAMESPACE_END

#endif //  UTL_TARGET_MICROSOFT
