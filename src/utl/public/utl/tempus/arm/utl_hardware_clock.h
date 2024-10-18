
// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_ARCH_AARCH64

#  include "utl/numeric/utl_sub_sat.h"

UTL_NAMESPACE_BEGIN

namespace tempus {

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<hardware_clock_t> {
public:
    using clock = hardware_clock_t;
    using value_type = uint64_t;
    using duration_type = hardware_ticks;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr duration_type
    time_since_epoch(value_type t) noexcept {
        return duration_type(t - 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr duration_type difference(
        value_type l, value_type r) noexcept {
        auto const val = __UTL sub_sat(l, r);
        if (!val && l == r) {
            return duration_type::invalid();
        }

        return duration_type((int64_t)val);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline constexpr clock_order compare(
        value_type const& l, value_type const& r) noexcept {
        return static_cast<clock_order>((l > r) - (l < r));
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_barrier_none) o) noexcept {
        return time_point<hardware_clock_t>{get_time(o)};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_barrier_after) o) noexcept {
        return time_point<hardware_clock_t>{get_time(o)};
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_barrier_before) o) noexcept {
        return time_point<hardware_clock_t>{get_time(o)};
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, decltype(instruction_barrier_enclose) o) noexcept {
        return time_point<hardware_clock_t>{get_time(o)};
    }

private:
    __UTL_ABI_PUBLIC static value_type get_time(decltype(instruction_barrier_none)) noexcept;
    __UTL_ABI_PUBLIC static value_type get_time(decltype(instruction_barrier_after)) noexcept;
    __UTL_ABI_PUBLIC static value_type get_time(decltype(instruction_barrier_before)) noexcept;
    __UTL_ABI_PUBLIC static value_type get_time(decltype(instruction_barrier_enclose)) noexcept;
};

} // namespace tempus

UTL_NAMESPACE_END

#endif //  UTL_ARCH_AARCH64
