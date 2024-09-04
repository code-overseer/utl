// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_ARCH_x86_64

#  include "utl/numeric/utl_sub_sat.h"

UTL_NAMESPACE_BEGIN

namespace platform {

struct timestamp_counter_t {
    long long tick;
    int aux;
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<hardware_clock_t> {
public:
    using clock = hardware_clock_t;
    using value_type = timestamp_counter_t;
    using duration = hardware_ticks;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        return duration(t.tick - 0ll);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, PURE, ALWAYS_INLINE) static inline constexpr duration difference(
        value_type l, value_type r) noexcept {
        return duration(__UTL sub_sat(l.tick, r.tick));
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l.aux == r.aux && l.tick == r.tick;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr int compare(
        value_type const& l, value_type const& r) noexcept {

        if (l.aux == r.aux) {
            return l.tick < r.tick ? -1 : l.tick > r.tick ? 1 : 0;
        }

        return l.aux < r.aux ? -1 : 1;
    }

    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(hardware_clock_t c) noexcept {
        return get_time(c, __UTL memory_order_seq_cst);
    }

    friend __UTL_HIDE_FROM_ABI time_point<hardware_clock_t> get_time(
        hardware_clock_t, __UTL memory_order) noexcept {
        return time_point<hardware_clock_t>{get_time(o)};
    }

private:
    __UTL_ABI_PUBLIC static value_type get_time(__UTL memory_order o) noexcept;
};

} // namespace platform

UTL_NAMESPACE_END

#endif //  UTL_ARCH_x86_64
