// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_CLOCK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_TARGET_APPLE
#  if UTL_ARCH_x86_64
#    include <immintrin.h>
#  endif

#  include "utl/numeric/utl_sub_sat.h"

#  include <mach/clock.h>
#  include <mach/mach.h>
#  include <mach/mach_time.h>
#  include <sys/time.h>

UTL_NAMESPACE_BEGIN

namespace platform {
template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<system_clock_t> {
public:
    using clock = system_clock_t;
    using value_type = long long;
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

    __UTL_HIDE_FROM_ABI friend time_point<system_clock_t> get_time(system_clock_t) noexcept {
        timeval t;
        gettimeofday(&t, nullptr);
        return time_point<system_clock_t>{value_type(t.tv_sec * 1000000) + value_type(t.tv_usec)};
    }
};

__UTL_HIDE_FROM_ABI constexpr ::time_t to_posix_time(time_point<system_clock_t> t) noexcept {
    // microseconds to seconds
    return t.value() / 1000000LL;
}

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

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline duration difference(
        value_type l, value_type r) noexcept {
        static mach_timebase_info_data_t const timebase = []() {
            mach_timebase_info_data_t t;
            mach_timebase_info(&t);
            return t;
        }();

        auto const diff = (l - r) * timebase.numer / timebase.denom;
        auto const seconds = diff / 1000000000;
        auto const nanoseconds = diff - seconds * 1000000000;
        using diff_type = long long;
        return duration{diff_type(seconds), diff_type(nanoseconds)};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr int compare(
        value_type l, value_type r) noexcept {
        return l < r ? -1 : l > r ? 1 : 0;
    }

    __UTL_HIDE_FROM_ABI friend time_point<steady_clock_t> get_time(steady_clock_t) noexcept {
        return time_point<steady_clock_t>{mach_continuous_time()};
    }
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<high_resolution_clock_t> :
    private clock_traits<steady_clock_t> {
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

#  if UTL_ARCH_ARM

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
        // static auto const freq = (unsigned int)__builtin_arm_rsr64("CNTFRQ_EL0");
        // static auto const ns_per_tick = 1000000000u / frequency;
        auto const diff = l - r;
        return duration{(long long)diff};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr bool equal(
        value_type l, value_type r) noexcept {
        return l == r;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr int compare(
        value_type const& l, value_type const& r) noexcept {
        return l < r ? -1 : l > r ? 1 : 0;
    }

    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, __UTL memory_order o) noexcept {
        unsigned long long res;
// Parameter for __builtin_arm_dmb
#    define DMB_NSH 0x7
#    define DMB_ISHLD 0x9
#    define DMB_ISHST 0xa
#    define DMB_ISH 0xb
#    define DMB_SY 0xf

// Parameter for __builtin_arm_dsb
#    define DSB_NSH 0x7
#    define DSB_ISHLD 0x9
#    define DSB_ISHST 0xa
#    define DSB_ISH 0xb
#    define DSB_SY 0xf

// Parameter for __builtin_arm_isb
#    define ISB_SY 0xf
/**
 * Ensure that reads of the counter are treated the same as memory reads
 * for the purposes of ordering by subsequent memory barriers.
 */
#    define arch_counter_enforce_ordering(val)   \
        do {                                     \
            decltype(val) tmp;                   \
            __asm volatile("   eor %0, %1, %1\n" \
                           "   add %0, sp, %0\n" \
                           "   ldr xzr, [%0]"    \
                           : "=r"(tmp)           \
                           : "r"(val));          \
        } while (0)

        __builtin_arm_isb(ISB_SY);
        switch (o) {
        case __UTL memory_order_relaxed:
            res = __builtin_arm_rsr64("CNTVCT_EL0");
            arch_counter_enforce_ordering(res);
            break;
        case __UTL memory_order_consume:
            UTL_FALLTHROUGH;
        case __UTL memory_order_acquire:
            res = __builtin_arm_rsr64("CNTVCT_EL0");
            __builtin_arm_dsb(DSB_SY);
            break;
        case __UTL memory_order_release:
            __builtin_arm_dmb(DMB_ISHST);
            res = __builtin_arm_rsr64("CNTVCT_EL0");
            arch_counter_enforce_ordering(res);
            break;
        case __UTL memory_order_acq_rel:
            __builtin_arm_dmb(DMB_ISHST);
            res = __builtin_arm_rsr64("CNTVCT_EL0");
            __builtin_arm_dsb(DSB_SY);
            break;
        default:
            __builtin_arm_dmb(DMB_SY);
            res = __builtin_arm_rsr64("CNTVCT_EL0");
            __builtin_arm_dsb(DSB_SY);
        }

        return time_point<hardware_clock_t>{res};

#    undef DMB_NSH
#    undef DMB_ISHLD
#    undef DMB_ISHST
#    undef DMB_ISH
#    undef DMB_SY
#    undef DSB_NSH
#    undef DSB_ISHLD
#    undef DSB_ISHST
#    undef DSB_ISH
#    undef DSB_SY
#    undef ISB_SY
    }
};

#  elif UTL_ARCH_x86_64
// // TODO move this
struct rdtscp_t {
    unsigned long long tick;
    int aux;
};

template <>
struct __UTL_PUBLIC_TEMPLATE clock_traits<hardware_clock_t> {
public:
    using clock = hardware_clock_t;
    using value_type = rdtscp_t;
    using duration = hardware_ticks;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration time_since_epoch(
        value_type t) noexcept {
        return duration(t.tick - 0);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, ALWAYS_INLINE) static inline constexpr duration difference(
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

        return l.aux - r.aux;
    }

    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(hardware_clock_t c) noexcept {
        return get_time(c, __UTL memory_order_seq_cst);
    }

    __UTL_HIDE_FROM_ABI friend time_point<hardware_clock_t> get_time(
        hardware_clock_t, __UTL memory_order o) noexcept {
        unsigned int high;
        unsigned int low;
        unsigned int aux;
        switch (o) {
        case __UTL memory_order_relaxed:
            __asm__ volatile("rdtscp" : "=a"(low), "=d"(high), "=c"(aux));
            break;
        case __UTL memory_order_consume:
            UTL_FALLTHROUGH;
        case __UTL memory_order_acquire:
            __asm__ volatile("rdtscp\n\t"
                             "lfence"
                             : "=a"(low), "=d"(high), "=c"(aux)
                             :
                             : "memory");
            break;
        case __UTL memory_order_release:
            __asm__ volatile("mfence\n\t"
                             "rdtscp"
                             : "=a"(low), "=d"(high), "=c"(aux)
                             :
                             : "memory");
            break;
        default:
            __asm__ volatile("mfence\n\t"
                             "rdtscp\n\t"
                             "lfence"
                             : "=a"(low), "=d"(high), "=c"(aux)
                             :
                             : "memory");
        }

        auto const tick = ((((unsigned long long)high) << 32) | ((unsigned long long)low));
        return time_point<hardware_clock_t>{tick, aux};
    }
};

#  endif

} // namespace platform

UTL_NAMESPACE_END

#endif //  UTL_TARGET_APPLE
