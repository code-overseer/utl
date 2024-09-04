// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/chrono/utl_chrono_fwd.h"
#include "utl/platform/utl_clock_fwd.h"

#include "utl/atomic.h"
#include "utl/compare/utl_strong_ordering.h"
#include "utl/platform/utl_hardware_ticks.h"
#include "utl/platform/utl_time_duration.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_default_constructible.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"

#include <time.h>

/**
 * Multi-platform clock library used internally by UTL
 * Essentially a simplified version of chrono using only a single duration unit
 *
 * Although publically exposed, API stability is not guaranteed
 */

UTL_NAMESPACE_BEGIN

namespace platform {

template <typename>
struct __UTL_PUBLIC_TEMPLATE is_clock : false_type {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_clock<system_clock_t> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_clock<steady_clock_t> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_clock<high_resolution_clock_t> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_clock<hardware_clock_t> : true_type {};

#if UTL_CXX20
template <typename T>
inline constexpr bool is_clock_v = is_clock<T>::value;
template <typename T>
concept clock_type = is_clock_v<T>;
#  define UTL_TRAIT_is_platform_clock(...) __UTL platform::clock_type<__VA_ARGS__>
#elif UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_clock_v = is_clock<T>::value;
#  define UTL_TRAIT_is_platform_clock(...) __UTL platform::is_clock_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_platform_clock(...) __UTL platform::is_clock<__VA_ARGS__>::value
#endif

template <UTL_CONCEPT_CXX20(clock_type) Clock>
class __UTL_PUBLIC_TEMPLATE time_point<Clock> {
    static_assert(UTL_TRAIT_is_platform_clock(Clock));
    friend clock_traits<Clock>;
    using traits = clock_traits<Clock>;

public:
    using clock = typename traits::clock;
    using value_type = typename traits::value_type;
    using duration = typename traits::duration;

    static_assert(UTL_TRAIT_is_default_constructible(value_type), "Invalid implementation");
    static_assert(UTL_TRAIT_is_copy_constructible(value_type), "Invalid implementation");

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) duration time_since_epoch() const noexcept {
        static_assert(noexcept(traits::time_since_epoch(value_)), "Invalid clock");
        return traits::time_since_epoch(value_);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) duration operator-(time_point const& other) const noexcept {
        static_assert(noexcept(traits::difference(value_, other.value_)), "Invalid clock");
        return traits::difference(value_, other.value_);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) bool operator==(time_point const& other) const noexcept {
        static_assert(noexcept(traits::equal(value_, other.value_)), "Invalid clock");
        return traits::equal(value_, other.value_);
    }

#if UTL_CXX20
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) bool operator<=>(time_point const& other) const noexcept {
        static_assert(noexcept(traits::compare(value_, other.value_)), "Invalid clock");
        return traits::compare(value_, other.value_) < 0;
    }
#else

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) bool operator<(time_point const& other) const noexcept {
        static_assert(noexcept(traits::compare(value_, other.value_)), "Invalid clock");
        return static_cast<__UTL strong_ordering>(traits::compare(value_, other.value_) <=> 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) bool operator!=(time_point const& other) const noexcept {
        return !(*this == other);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) bool operator>=(time_point const& other) const noexcept {
        return !(*this < other);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) bool operator>(time_point const& other) const noexcept {
        return other < *this;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) bool operator<=(time_point const& other) const noexcept {
        return !(other < *this);
    }

#endif

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) constexpr value_type const& value() const noexcept {
        return value_;
    }

    constexpr explicit time_point() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(value_type)) = default;

private:
    explicit time_point(value_type const& v) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(value_type))
        : value_(v){};

    value_type value_;
};

struct system_clock_t {
    explicit constexpr system_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend time_point<system_clock_t> get_time(system_clock_t) noexcept;
};

struct steady_clock_t {
    explicit constexpr steady_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend time_point<steady_clock_t> get_time(steady_clock_t) noexcept;
};

struct high_resolution_clock_t {
public:
    explicit constexpr high_resolution_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend time_point<high_resolution_clock_t> get_time(
        high_resolution_clock_t) noexcept;
};

struct hardware_clock_t {
    explicit constexpr hardware_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, __UTL memory_order) noexcept;
    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(hardware_clock_t c) noexcept;
};

__UTL_HIDE_FROM_ABI constexpr ::time_t to_posix_time(time_point<system_clock_t>) noexcept;

UTL_INLINE_CXX17 constexpr system_clock_t system_clock{};

UTL_INLINE_CXX17 constexpr steady_clock_t steady_clock{};

UTL_INLINE_CXX17 constexpr high_resolution_clock_t high_resolution_clock{};

UTL_INLINE_CXX17 constexpr hardware_clock_t hardware_clock{};

} // namespace platform

UTL_NAMESPACE_END

#define UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD
#include "utl/platform/apple/utl_clock.h"
#include "utl/platform/arm/utl_hardware_clock.h"
#include "utl/platform/posix/utl_clock.h"
#include "utl/platform/winapi/utl_clock.h"
#include "utl/platform/x86/utl_hardware_clock.h"
#undef UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD
