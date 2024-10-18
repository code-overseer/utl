// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/chrono/utl_chrono_fwd.h"
#include "utl/compare/utl_compare_fwd.h"
#include "utl/tempus/utl_clock_fwd.h"

#include "utl/concepts/utl_boolean_testable.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/concepts/utl_semiregular.h"
#include "utl/hardware/utl_instruction_barrier.h"
#include "utl/tempus/utl_duration.h"
#include "utl/tempus/utl_hardware_ticks.h"
#include "utl/tempus/utl_tempus_literals.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_default_constructible.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"

#include <time.h>

/**
 * A simple multi-platform clock library
 * Essentially a simplified version of chrono using only a single duration unit
 *
 * Although publically exposed, API stability is not guaranteed
 */

UTL_NAMESPACE_BEGIN

namespace tempus {

using namespace __UTL literals::tempus_literals;

enum class clock_order : signed char {
    less = -1,
    equal = 0,
    greater = 1,
    unordered = 2
};

#if UTL_CXX20
template <typename Clock>
concept clock_type =
    requires {
        typename clock_traits<Clock>::clock;
        typename clock_traits<Clock>::duration_type;
        typename clock_traits<Clock>::value_type;
    } && semiregular<typename clock_traits<Clock>::value_type> &&
    requires(clock_traits<Clock>::value_type l, clock_traits<Clock>::value_type r) {
        {
            clock_traits<Clock>::difference(l, r)
        } noexcept -> same_as<typename clock_traits<Clock>::duration_type>;
        {
            clock_traits<Clock>::time_since_epoch(l)
        } noexcept -> same_as<typename clock_traits<Clock>::duration_type>;
        { clock_traits<Clock>::equal(l, r) } noexcept -> boolean_testable;
        { clock_traits<Clock>::compare(l, r) } noexcept -> same_as<clock_order>;
    };
template <typename T>
inline constexpr bool is_clock_v = clock_type<T>;
template <typename T>
struct is_clock : bool_constant<is_clock_v<T>> {};
#  define UTL_TRAIT_is_tempus_clock(...) __UTL tempus::clock_type<__VA_ARGS__>

#else // UTL_CXX20

namespace details {
template <typename T>
auto is_clock_impl(float) noexcept -> false_type;
template <typename Clock>
auto is_clock_impl(int) noexcept -> bool_constant<
    always_true<typename clock_traits<Clock>::clock, typename clock_traits<Clock>::duration_type,
        typename clock_traits<Clock>::value_type>() &&
    UTL_TRAIT_is_default_constructible(typename clock_traits<Clock>::value_type) &&
    UTL_TRAIT_is_copy_constructible(typename clock_traits<Clock>::value_type) &&
    noexcept(clock_traits<Clock>::time_since_epoch(
        __UTL declval<typename clock_traits<Clock>::value_type>())) &&
    noexcept(
        clock_traits<Clock>::difference(__UTL declval<typename clock_traits<Clock>::value_type>(),
            __UTL declval<typename clock_traits<Clock>::value_type>())) &&
    noexcept(clock_traits<Clock>::equal(__UTL declval<typename clock_traits<Clock>::value_type>(),
        __UTL declval<typename clock_traits<Clock>::value_type>())) &&
    noexcept(clock_traits<Clock>::compare(__UTL declval<typename clock_traits<Clock>::value_type>(),
        __UTL declval<typename clock_traits<Clock>::value_type>())) &&
    UTL_TRAIT_is_boolean_testable(decltype(clock_traits<Clock>::equal(
        __UTL declval<typename clock_traits<Clock>::value_type>(),
        __UTL declval<typename clock_traits<Clock>::value_type>()))) &&
    UTL_TRAIT_is_same(clock_order,
        decltype(clock_traits<Clock>::compare(
            __UTL declval<typename clock_traits<Clock>::value_type>(),
            __UTL declval<typename clock_traits<Clock>::value_type>())))>;
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_clock : decltype(details::is_clock_impl<T>(0)) {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_clock_v = decltype(details::is_clock_impl<T>(0))::value;
#    define UTL_TRAIT_is_tempus_clock(...) __UTL tempus::is_clock_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_tempus_clock(...) __UTL tempus::is_clock<__VA_ARGS__>::value
#  endif

#endif // UTL_CXX20

#if UTL_CXX20
template <clock_type Clock>
class __UTL_PUBLIC_TEMPLATE time_point<Clock> {
#else
template <typename Clock>
class __UTL_PUBLIC_TEMPLATE time_point {
#endif
    static_assert(UTL_TRAIT_is_tempus_clock(Clock), "Invalid implementation");
    friend clock_traits<Clock>;
    using traits = clock_traits<Clock>;

#if UTL_CXX20
    template <same_as<::std::partial_ordering> R>
    struct order_table {
        static constexpr R values[] = {R::less, R::equal, R::greater, R::unordered};
    };
#endif

public:
    using clock = typename traits::clock;
    using value_type = typename traits::value_type;
    using duration_type = typename traits::duration_type;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr duration_type time_since_epoch() const noexcept {
        return traits::time_since_epoch(value_);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr duration_type operator-(
        time_point const& other) const noexcept {
        return traits::difference(value_, other.value_);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator==(
        time_point const& other) const noexcept {
        return traits::equal(value_, other.value_);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator<(time_point const& other) const noexcept {
        return traits::compare(value_, other.value_) == clock_order::less;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator!=(
        time_point const& other) const noexcept {
        auto const result = traits::compare(value_, other.value_);
        return result != clock_order::equal && result != clock_order::unordered;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator>=(
        time_point const& other) const noexcept {
        auto const result = traits::compare(value_, other.value_);
        return result != clock_order::less && result != clock_order::unordered;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator>(time_point const& other) const noexcept {
        return traits::compare(value_, other.value_) == clock_order::greater;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator<=(
        time_point const& other) const noexcept {
        auto const result = traits::compare(value_, other.value_);
        return result != clock_order::greater && result != clock_order::unordered;
    }

    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) constexpr value_type const& value() const noexcept {
        return value_;
    }

#if UTL_CXX20
    template <same_as<time_point> T, same_as<::std::partial_ordering> R = ::std::partial_ordering>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr R operator<=>(T const& other) const noexcept {
        auto const idx = static_cast<int>(traits::compare(value_, other.value_));
        return order_table<R>::values[idx + 1];
    }
#endif

    __UTL_HIDE_FROM_ABI constexpr explicit time_point() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(value_type)) = default;

private:
    __UTL_HIDE_FROM_ABI explicit time_point(value_type const& v) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(value_type))
        : value_(v){};

    value_type value_;
};

struct __UTL_ABI_PUBLIC system_clock_t {
    explicit constexpr system_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend time_point<system_clock_t> get_time(system_clock_t) noexcept;
};

struct __UTL_ABI_PUBLIC steady_clock_t {
    explicit constexpr steady_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend time_point<steady_clock_t> get_time(steady_clock_t) noexcept;
};

struct __UTL_ABI_PUBLIC high_resolution_clock_t {
public:
    explicit constexpr high_resolution_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend time_point<high_resolution_clock_t> get_time(
        high_resolution_clock_t) noexcept;
};

struct __UTL_ABI_PUBLIC process_clock_t {
    explicit constexpr process_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend time_point<process_clock_t> get_time(process_clock_t) noexcept;
};

struct __UTL_ABI_PUBLIC thread_clock_t {
    explicit constexpr thread_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend time_point<thread_clock_t> get_time(thread_clock_t) noexcept;
};

struct __UTL_ABI_PUBLIC hardware_clock_t {
    explicit constexpr hardware_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_barrier_none)) noexcept;
    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_barrier_after)) noexcept;
    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_barrier_before)) noexcept;
    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_barrier_enclose)) noexcept;

    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(hardware_clock_t) noexcept;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) static bool invariant_frequency() noexcept {
        return hardware_ticks::invariant_frequency();
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) static uint64_t frequency() noexcept {
        return hardware_ticks::frequency();
    }
};

UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr ::time_t to_posix_time(time_point<system_clock_t>) noexcept;

UTL_INLINE_CXX17 constexpr system_clock_t system_clock{};

UTL_INLINE_CXX17 constexpr steady_clock_t steady_clock{};

UTL_INLINE_CXX17 constexpr high_resolution_clock_t high_resolution_clock{};

UTL_INLINE_CXX17 constexpr process_clock_t process_clock{};

UTL_INLINE_CXX17 constexpr thread_clock_t thread_clock{};

UTL_INLINE_CXX17 constexpr hardware_clock_t hardware_clock{};

} // namespace tempus

UTL_NAMESPACE_END

#define UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD
#include "utl/tempus/apple/utl_clock.h"
#include "utl/tempus/arm/utl_hardware_clock.h"
#include "utl/tempus/posix/utl_clock.h"
#include "utl/tempus/winapi/utl_clock.h"
#include "utl/tempus/x86/utl_hardware_clock.h"
#undef UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD
