// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_TARGET_APPLE
#  include "utl/tempus/posix/utl_clock.h"

#  include <stdint.h>

extern "C" uint64_t mach_continuous_time(void);

UTL_NAMESPACE_BEGIN

namespace tempus {

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<steady_clock_t> {
public:
    using clock = steady_clock_t;
    using value_type = uint64_t;
    using duration_type = duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, PURE, NODISCARD) static duration_type difference(
        value_type l, value_type r) noexcept;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr duration_type
    time_since_epoch(value_type t) noexcept {
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr clock_order compare(
        value_type l, value_type r) noexcept {
        return static_cast<clock_order>((l > r) - (l < r));
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend time_point<steady_clock_t> get_time(
        steady_clock_t) noexcept {
        return time_point<steady_clock_t>{mach_continuous_time()};
    }
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<high_resolution_clock_t> :
    private clock_traits<steady_clock_t> {
    using base_type = clock_traits<steady_clock_t>;

public:
    using clock = high_resolution_clock_t;
    using typename base_type::duration_type;
    using typename base_type::value_type;

    using base_type::compare;
    using base_type::difference;
    using base_type::equal;
    using base_type::time_since_epoch;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend time_point<high_resolution_clock_t> get_time(
        high_resolution_clock_t) noexcept {
        return time_point<high_resolution_clock_t>{get_time(steady_clock).value()};
    }
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<thread_clock_t> : private details::timeval_traits {
private:
    using base_type = details::timeval_traits;

public:
    using clock = thread_clock_t;
    using typename base_type::duration_type;
    using typename base_type::value_type;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr duration_type
    time_since_epoch(value_type t) noexcept {
        return duration_type::invalid();
    }

    using base_type::compare;
    using base_type::difference;
    using base_type::equal;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend time_point<thread_clock_t> get_time(
        thread_clock_t) noexcept {
        return time_point<thread_clock_t>{get_time()};
    }

private:
    __UTL_ABI_PUBLIC static value_type get_time() noexcept;
};
} // namespace tempus

UTL_NAMESPACE_END

#endif //  UTL_TARGET_APPLE
