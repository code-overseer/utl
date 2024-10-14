// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_TARGET_UNIX

#  include <sys/resource.h>
#  include <sys/time.h>
#  include <time.h>

UTL_NAMESPACE_BEGIN

namespace tempus {
namespace details {
constexpr long long timeval_to_usec(::timeval const& val) noexcept {
    using value_type = long long;
    using sec_to_usec = integral_constant<value_type, 1000000LL>;
    return value_type(val.tv_sec * sec_to_usec::value) + value_type(val.tv_usec);
}
} // namespace details

#  if !UTL_TARGET_APPLE
template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<steady_clock_t> {
public:
    using clock = steady_clock_t;
    using value_type = ::timespec;
    using duration = duration;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline duration difference(
        value_type l, value_type r) noexcept {
        using diff_type = long long;
        auto const dsec = diff_type(l.tv_sec) - diff_type(r.tv_sec);
        auto const dnsec = diff_type(l.tv_nsec) - diff_type(r.tv_nsec);
        return duration{0, dsec * 1000000000ll + dnsec};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr clock_order compare(
        value_type l, value_type r) noexcept {
        auto const lt =
            static_cast<int>(l.tv_sec < r.tv_sec) | static_cast<int>(l.tv_nsec < r.tv_nsec);
        auto const gt =
            static_cast<int>(l.tv_sec > r.tv_sec) | static_cast<int>(l.tv_nsec > r.tv_nsec);
        return static_cast<clock_order>(gt - lt);
    }

    __UTL_HIDE_FROM_ABI friend time_point<steady_clock_t> get_time(steady_clock_t) noexcept {
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
#  endif // !UTL_TARGET_APPLE

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<system_clock_t> {
public:
    using clock = system_clock_t;
    using value_type = long long;
    using duration = duration;

public:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        return difference(t, 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr duration difference(
        value_type l, value_type r) noexcept {
        return duration{0, (l - r) * 1000};
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
        return time_point<system_clock_t>{details::timeval_to_usec(t)};
    }
};

__UTL_HIDE_FROM_ABI constexpr ::time_t to_posix_time(time_point<system_clock_t> t) noexcept {
    using value_type = typename clock_traits<system_clock_t>::value_type;
    using conversion = integral_constant<value_type, 1000000>;
    // microseconds to seconds
    return t.value() / conversion::value;
}

struct usage_time_t {
    long long kernel;
    long long user;
};

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
        rusage usage;
        ::getrusage(RUSAGE_SELF, &usage);
        auto const user_time = details::timeval_to_usec(usage.ru_utime);
        auto const kernel_time = details::timeval_to_usec(usage.ru_stime);
        return time_point<process_clock_t>{user_time + kernel_time};
    }
};

#  if !UTL_TARGET_APPLE
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
