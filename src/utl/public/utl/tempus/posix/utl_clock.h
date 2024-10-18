// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#include <stdint.h>

#if UTL_TARGET_UNIX

#  include <sys/resource.h>
#  include <sys/time.h>
#  include <time.h>

UTL_NAMESPACE_BEGIN

namespace tempus {
namespace details {
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr int64_t timeval_to_usec(::timeval const& val) noexcept {
    using value_type = int64_t;
    using sec_to_usec = integral_constant<value_type, 1000000>;
    return value_type(val.tv_sec * sec_to_usec::value) + value_type(val.tv_usec);
}
} // namespace details

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<system_clock_t> {
public:
    using clock = system_clock_t;
    using value_type = int64_t;
    using duration_type = duration;

public:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr duration_type
    time_since_epoch(value_type t) noexcept {
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr duration_type difference(
        value_type l, value_type r) noexcept {
        return duration_type{0, (l - r) * 1000};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr clock_order compare(
        value_type l, value_type r) noexcept {
        return static_cast<clock_order>((l > r) - (l < r));
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend time_point<system_clock_t> get_time(
        system_clock_t) noexcept {
        timeval t;
        gettimeofday(&t, nullptr);
        return time_point<system_clock_t>{details::timeval_to_usec(t)};
    }
};

UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr ::time_t to_posix_time(
    time_point<system_clock_t> t) noexcept {
    using value_type = typename clock_traits<system_clock_t>::value_type;
    using conversion = integral_constant<value_type, 1000000>;
    // microseconds to seconds
    return t.value() / conversion::value;
}

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<process_clock_t> : private clock_traits<system_clock_t> {
private:
    using base_type = clock_traits<system_clock_t>;

public:
    using clock = process_clock_t;
    using typename base_type::duration_type;
    using typename base_type::value_type;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr duration_type
    time_since_epoch(value_type t) noexcept {
        return duration_type::invalid();
    }

    using base_type::compare;
    using base_type::difference;
    using base_type::equal;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend time_point<process_clock_t> get_time(
        process_clock_t) noexcept {
        rusage usage;
        ::getrusage(RUSAGE_SELF, &usage);
        auto const user_time = details::timeval_to_usec(usage.ru_utime);
        auto const kernel_time = details::timeval_to_usec(usage.ru_stime);
        return time_point<process_clock_t>{user_time + kernel_time};
    }
};

#  if !UTL_TARGET_APPLE
template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<steady_clock_t> {
public:
    using clock = steady_clock_t;
    using value_type = ::timespec;
    using duration_type = duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr duration_type
    time_since_epoch(value_type t) noexcept {
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline duration_type difference(
        value_type l, value_type r) noexcept {
        using diff_type = long long;
        auto const dsec = diff_type(l.tv_sec) - diff_type(r.tv_sec);
        auto const dnsec = diff_type(l.tv_nsec) - diff_type(r.tv_nsec);
        return duration_type{0, dsec * 1000000000ll + dnsec};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr clock_order compare(
        value_type l, value_type r) noexcept {
        auto const lt = l.tv_sec < r.tv_sec || (l.tv_sec == r.tv_sec && l.tv_nsec < r.tv_nsec);
        auto const gt = r.tv_sec < l.tv_sec || (l.tv_sec == r.tv_sec && r.tv_nsec < l.tv_nsec);
        return static_cast<clock_order>(static_cast<int>(gt) - static_cast<int>(lt));
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend time_point<steady_clock_t> get_time(
        steady_clock_t) noexcept {
        value_type tp;
        clock_gettime(CLOCK_MONOTONIC, &tp);
        return time_point<steady_clock_t>{tp};
    }
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<high_resolution_clock_t> :
    private clock_traits<steady_clock_t> {
private:
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
struct __UTL_PUBLIC_TEMPLATE clock_traits<thread_clock_t> : private clock_traits<system_clock_t> {
private:
    using base_type = clock_traits<system_clock_t>;

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
        rusage usage;
        ::getrusage(RUSAGE_THREAD, &usage);
        auto const user_time = details::timeval_to_usec(usage.ru_utime);
        auto const kernel_time = details::timeval_to_usec(usage.ru_stime);
        return time_point<thread_clock_t>{user_time + kernel_time};
    }
};

#  endif // !UTL_TARGET_APPLE

} // namespace tempus

UTL_NAMESPACE_END

#endif
