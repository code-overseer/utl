// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_TARGET_MICROSOFT

#  include "utl/numeric/utl_sub_sat.h"

UTL_NAMESPACE_BEGIN

namespace tempus {
template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<system_clock_t> {
public:
    using clock = system_clock_t;
    using value_type = long long;
    using duration = duration;
    using epoch_conversion = integral_constant<value_type, 11644473600LL>;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        // Epoch is 1601-01-01 00:00:00 UTC
        static_assert(sizeof(value_type) >= 8, "Unsupported architecture");
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration difference(
        value_type l, value_type r) noexcept {
        auto const diff = l - r;
        // 100 nanoseconds to seconds
        auto const seconds = diff / 10000000;
        auto const nanoseconds = diff * 100 - seconds * 1000000000;
        return duration{seconds, nanoseconds};
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
struct __UTL_PUBLIC_TEMPLATE clock_traits<process_clock_t> : private clock_traits<system_clock_t> {
private:
    using base_type = clock_traits<system_clock_t>;

public:
    using clock = process_clock_t;
    using typename base_type::duration;
    using typename base_type::value_type;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        return duration::invalid();
    }

    using base_type::compare;
    using base_type::difference;
    using base_type::equal;

    __UTL_HIDE_FROM_ABI friend time_point<process_clock_t> get_time(process_clock_t) noexcept {
        return time_point<system_clock_t>{get_time()};
    }

private:
    __UTL_ABI_PUBLIC static value_type get_time() noexcept;
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<thread_clock_t> : private clock_traits<system_clock_t> {
private:
    using base_type = clock_traits<system_clock_t>;

public:
    using clock = thread_clock_t;
    using typename base_type::duration;
    using typename base_type::value_type;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        return duration::invalid();
    }

    using base_type::compare;
    using base_type::difference;
    using base_type::equal;

    __UTL_HIDE_FROM_ABI friend time_point<thread_clock_t> get_time(thread_clock_t) noexcept {
        return time_point<system_clock_t>{get_time()};
    }

private:
    __UTL_ABI_PUBLIC static value_type get_time() noexcept;
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<steady_clock_t> {
public:
    using clock = steady_clock_t;
    using value_type = long long;
    using duration = duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        static_assert(sizeof(size_t) >= 8, "Unsupported architecture");
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration difference(
        value_type l, value_type r) noexcept {
        auto const diff = l - r;
        auto const seconds = diff / 1000;
        return duration{seconds, diff * 1000000 - seconds * 1000000000};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr clock_order compare(
        value_type l, value_type r) noexcept {
        return static_cast<clock_order>((l > r) - (l < r));
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
    using duration = duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr clock_order compare(
        value_type l, value_type r) noexcept {
        return static_cast<clock_order>((l > r) - (l < r));
    }

    friend __UTL_HIDE_FROM_ABI time_point<high_resolution_clock_t> get_time(
        high_resolution_clock_t) noexcept {
        return time_point<high_resolution_clock_t>{get_time()};
    }

    UTL_ATTRIBUTES(__UTL_ABI_PUBLIC, CONST) static duration difference(value_type l, value_type r) noexcept;

private:
    __UTL_ABI_PUBLIC static value_type get_time() noexcept;
};

} // namespace tempus

UTL_NAMESPACE_END

#endif //  UTL_TARGET_MICROSOFT
