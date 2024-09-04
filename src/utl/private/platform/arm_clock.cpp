// Copyright 2023-2024 Bryan Wong

#include "utl/assert/utl_assert.h"
#include "utl/platform/utl_clock.h"
#include "utl/type_traits/utl_is_same.h"

#if UTL_ARCH_AARCH64

// TODO push macro
#  ifdef __UTL_SUPPORTS_REQUIRED_BUILTINS
#    undef __UTL_SUPPORTS_REQUIRED_BUILTINS
#  endif

#  define __UTL_SUPPORTS_REQUIRED_BUILTINS                                        \
      UTL_HAS_BUILTIN(__builtin_arm_isb) && UTL_HAS_BUILTIN(__builtin_arm_dmb) && \
          UTL_HAS_BUILTIN(__builtin_arm_rsr64)

#  if __UTL_SUPPORTS_REQUIRED_BUILTINS
// Parameter for __builtin_arm_dmb
#    define DMB_NSH 0x7
#    define DMB_ISHLD 0x9
#    define DMB_ISHST 0xa
#    define DMB_ISH 0xb
#    define DMB_SY 0xf

// Parameter for __builtin_arm_isb
#    define ISB_SY 0xf

UTL_NAMESPACE_BEGIN
namespace platform {

static unsigned int clock_frequency() noexcept {
    static unsigned int const value =
        (unsigned int)(__builtin_arm_rsr64("CNTFRQ_EL0") & 0xffffffff);
    return value;
}

auto clock_traits<hardware_clock_t>::get_time(__UTL memory_order o) noexcept -> value_type {
    static_assert(is_same<value_type, unsigned long long>::value, "Invalid implementation");
    value_type res;
    switch (o) {
    case __UTL memory_order_relaxed:
        __builtin_arm_isb(ISB_SY);
        res = __builtin_arm_rsr64("CNTVCT_EL0");
        __builtin_arm_isb(ISB_SY);
        break;
    case __UTL memory_order_consume:
        UTL_FALLTHROUGH;
    case __UTL memory_order_acquire:
        __builtin_arm_isb(ISB_SY);
        res = __builtin_arm_rsr64("CNTVCT_EL0");
        __builtin_arm_dmb(DMB_SY);
        __builtin_arm_isb(ISB_SY);
        break;
    case __UTL memory_order_release:
        __builtin_arm_isb(ISB_SY);
        __builtin_arm_dmb(DMB_SY);
        res = __builtin_arm_rsr64("CNTVCT_EL0");
        __builtin_arm_isb(ISB_SY);
        break;
    default:
        __builtin_arm_isb(ISB_SY);
        __builtin_arm_dmb(DMB_SY);
        res = __builtin_arm_rsr64("CNTVCT_EL0");
        __builtin_arm_dmb(DMB_SY);
        __builtin_arm_isb(ISB_SY);
        break;
    }

    return res;
}

bool hardware_ticks::invariant_frequency() noexcept {
    return true;
}

} // namespace platform

UTL_NAMESPACE_END
#    undef DMB_NSH
#    undef DMB_ISHLD
#    undef DMB_ISHST
#    undef DMB_ISH
#    undef DMB_SY
#    undef ISB_SY

#  elif UTL_SUPPORTS_GNU_ASM

UTL_NAMESPACE_BEGIN
namespace platform {
static unsigned int clock_frequency() noexcept {
    static unsigned int const value = []() {
        unsigned long long res;
        __asm__ volatile("mrs %0, CNTFRQ_EL0\n\t" : "=r"(res) : : "memory");
        return (unsigned int)(res & 0xffffffff);
    }();

    return value;
}

auto clock_traits<hardware_clock_t>::get_time(__UTL memory_order o) noexcept -> value_type {
    unsigned long long res;
    switch (o) {
    case __UTL memory_order_relaxed:
        __asm__ volatile("isb sy\n\t"
                         "mrs %0, CNTVCT_EL0\n\t"
                         "isb sy"
                         : "=r"(res)
                         :
                         : "memory");
        break;

    case __UTL memory_order_consume:
        [[fallthrough]];
    case __UTL memory_order_acquire:
        __asm__ volatile("isb sy\n\t"
                         "mrs %0, CNTVCT_EL0\n\t"
                         "dmb sy\n\t"
                         "isb sy"
                         : "=r"(res)
                         :
                         : "memory");
        break;

    case __UTL memory_order_release:
        __asm__ volatile("isb sy\n\t"
                         "dmb sy\n\t"
                         "mrs %0, CNTVCT_EL0\n\t"
                         "isb sy"
                         : "=r"(res)
                         :
                         : "memory");
        break;

    default:
        __asm__ volatile("isb sy\n\t"
                         "dmb sy\n\t"
                         "mrs %0, CNTVCT_EL0\n\t"
                         "dmb sy\n\t"
                         "isb sy"
                         : "=r"(res)
                         :
                         : "memory");
        break;
    }

    return res;
}

bool hardware_ticks::invariant_frequency() noexcept {
    return true;
}

} // namespace platform
UTL_NAMESPACE_END

#  elif UTL_TARGET_MICROSOFT

#    define NOMINMAX
#    define NODRAWTEXT
#    define NOGDI
#    define NOBITMAP
#    define NOMCX
#    define NOSERVICE
#    define NOHELP
#    ifndef WIN32_LEAN_AND_MEAN
#      define WIN32_LEAN_AND_MEAN
#    endif

#    include <Windows.h>
#    include <intrin.h>
#    include <winnt.h>

#    define ARM64_CNTFRQ ARM64_SYSREG(3, 3, 14, 0, 0)

static unsigned int clock_frequency() noexcept {
    static unsigned int const value = (unsigned int)(_ReadStatusReg(ARM64_CNTFRQ) & 0xffffffff);
    return value;
}

UTL_NAMESPACE_BEGIN
namespace platform {
auto time_point<hardware_clock_t>::get_time(__UTL memory_order o) noexcept -> value_type {
    unsigned long long res;
    switch (o) {
    case __UTL memory_order_relaxed:
        __isb(_ARM64_BARRIER_SY);
        res = _ReadStatusReg(ARM64_CNTVCT);
        __isb(_ARM64_BARRIER_SY);
        break;
    case __UTL memory_order_consume:
        UTL_FALLTHROUGH;
    case __UTL memory_order_acquire:
        __isb(_ARM64_BARRIER_SY);
        res = _ReadStatusReg(ARM64_CNTVCT);
        __dmb(_ARM64_BARRIER_SY);
        __isb(_ARM64_BARRIER_SY);
        break;
    case __UTL memory_order_release:
        __isb(_ARM64_BARRIER_SY);
        __dmb(_ARM64_BARRIER_SY);
        res = _ReadStatusReg(ARM64_CNTVCT);
        __isb(_ARM64_BARRIER_SY);
        break;
    default:
        __isb(_ARM64_BARRIER_SY);
        __dmb(_ARM64_BARRIER_SY);
        res = _ReadStatusReg(ARM64_CNTVCT);
        __dmb(_ARM64_BARRIER_SY);
        __isb(_ARM64_BARRIER_SY);
    }

    return res;
}

bool hardware_ticks::invariant_frequency() noexcept {
    return true;
}
} // namespace platform
UTL_NAMESPACE_END

#  else

UTL_PRAGMA_WARN("Unrecognized target/compiler");

UTL_NAMESPACE_BEGIN
namespace platform {
auto time_point<hardware_clock_t>::get_time(__UTL memory_order o) noexcept -> value_type {
    UTL_ASSERT(false);
    UTL_BUILTIN_unreachable();
}
bool hardware_ticks::invariant_frequency() noexcept {
    return false;
}
static unsigned int clock_frequency() noexcept {
    return 1;
}
} // namespace platform
UTL_NAMESPACE_END
#  endif

UTL_NAMESPACE_BEGIN
namespace platform {

time_duration to_time_duration(hardware_ticks t) noexcept {
    if (!hardware_ticks::invariant_frequency()) {
        return time_duration::invalid();
    }

    if (t.value() < 0) {
        return time_duration::invalid();
    }

    static constexpr unsigned int nano = 1000000000;
    return time_duration{0, t.value() * nano / clock_frequency()};
}

} // namespace platform
UTL_NAMESPACE_END
#endif
