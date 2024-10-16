
// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_ARCH_AARCH64

#  include "utl/atomic/utl_atomic.h"
#  include "utl/numeric/utl_sub_sat.h"

UTL_NAMESPACE_BEGIN

namespace tempus {

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<hardware_clock_t> {
public:
    using clock = hardware_clock_t;
    using value_type = unsigned long long;
    using duration = hardware_ticks;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        return duration(t - 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration difference(
        value_type l, value_type r) noexcept {
        auto const diff = l - r;
        return duration{(long long)diff};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr clock_order compare(
        value_type const& l, value_type const& r) noexcept {
        return static_cast<clock_order>((l > r) - (l < r));
    }

    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_order_relaxed) o) noexcept {
        return time_point<hardware_clock_t>{get_time(o)};
    }

    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_order_acquire) o) noexcept {
        return time_point<hardware_clock_t>{get_time(o)};
    }
    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_order_release) o) noexcept {
        return time_point<hardware_clock_t>{get_time(o)};
    }
    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_order_acq_rel) o) noexcept {
        return time_point<hardware_clock_t>{get_time(o)};
    }

    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_order_seq_cst) o) noexcept {
        return time_point<hardware_clock_t>{get_time(o)};
    }

    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t clock) noexcept {
        return time_point<hardware_clock_t>{get_time(instruction_order_seq_cst)};
    }

private:
    __UTL_ABI_PUBLIC static value_type get_time(decltype(instruction_order_relaxed)) noexcept;
    __UTL_ABI_PUBLIC static value_type get_time(decltype(instruction_order_acquire)) noexcept;
    __UTL_ABI_PUBLIC static value_type get_time(decltype(instruction_order_release)) noexcept;
    __UTL_ABI_PUBLIC static value_type get_time(decltype(instruction_order_acq_rel)) noexcept;
    __UTL_ABI_PUBLIC static value_type get_time(decltype(instruction_order_seq_cst)) noexcept;
};

} // namespace tempus

UTL_NAMESPACE_END

#endif //  UTL_ARCH_AARCH64
