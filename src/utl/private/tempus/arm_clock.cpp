// Copyright 2023-2024 Bryan Wong

#include "utl/assert/utl_assert.h"
#include "utl/atomic/utl_atomic.h"
#include "utl/tempus/utl_clock.h"
#include "utl/type_traits/utl_is_same.h"

// TODO Investigate using PMU counter
#if UTL_ARCH_AARCH64

// TODO push macro
#  ifdef __UTL_SUPPORTS_REQUIRED_BUILTINS
#    undef __UTL_SUPPORTS_REQUIRED_BUILTINS
#  endif

#  define __UTL_SUPPORTS_REQUIRED_BUILTINS                                        \
      UTL_HAS_BUILTIN(__builtin_arm_isb) && UTL_HAS_BUILTIN(__builtin_arm_dmb) && \
          UTL_HAS_BUILTIN(__builtin_arm_rsr64)

#  if __UTL_SUPPORTS_REQUIRED_BUILTINS
UTL_NAMESPACE_BEGIN
namespace tempus {

static unsigned int clock_frequency() noexcept {
    static unsigned int const value =
        (unsigned int)(__builtin_arm_rsr64("CNTFRQ_EL0") & 0xffffffff);
    return value;
}

static_assert(is_same<typename clock_traits<hardware_clock_t>::value_type,
                  decltype(__builtin_arm_rsr64("CNTVCT_EL0"))>::value,
    "Invalid implementation");

#    define __UTL_BARRIER_SY 0xF
auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_relaxed)) noexcept
    -> value_type {
    return __builtin_arm_rsr64("CNTVCT_EL0");
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_acquire)) noexcept
    -> value_type {
    value_type const res = __builtin_arm_rsr64("CNTVCT_EL0");
    __builtin_arm_isb(__UTL_BARRIER_SY);
    return res;
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_release)) noexcept
    -> value_type {
    __builtin_arm_isb(__UTL_BARRIER_SY);
    return __builtin_arm_rsr64("CNTVCT_EL0");
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_acq_rel)) noexcept
    -> value_type {
    __builtin_arm_isb(__UTL_BARRIER_SY);
    value_type const res = __builtin_arm_rsr64("CNTVCT_EL0");
    __builtin_arm_isb(__UTL_BARRIER_SY);
    return res;
}

#    undef __UTL_BARRIER_SY

bool hardware_ticks::invariant_frequency() noexcept {
    return true;
}

} // namespace tempus

UTL_NAMESPACE_END

#  elif UTL_SUPPORTS_GNU_ASM

UTL_NAMESPACE_BEGIN
namespace tempus {
static unsigned int clock_frequency() noexcept {
    static unsigned int const value = []() {
        unsigned long long res;
        __asm__ volatile("mrs %0, CNTFRQ_EL0\n\t" : "=r"(res) : : "memory");
        return (unsigned int)(res & 0xffffffff);
    }();

    return value;
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_relaxed)) noexcept
    -> value_type {
    value_type res;
    __asm__ volatile("mrs %0, CNTVCT_EL0\n\t" : "=r"(res) : : "memory");
    return res;
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_acquire)) noexcept
    -> value_type {
    value_type res;
    __asm__ volatile("mrs %0, CNTVCT_EL0\n\t"
                     "isb sy"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_release)) noexcept
    -> value_type {
    value_type res;
    __asm__ volatile("isb sy\n\t"
                     "mrs %0, CNTVCT_EL0"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_acq_rel)) noexcept
    -> value_type {
    value_type res;
    __asm__ volatile("isb sy\n\t"
                     "mrs %0, CNTVCT_EL0\n\t"
                     "isb sy\n\t"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

bool hardware_ticks::invariant_frequency() noexcept {
    return true;
}
} // namespace tempus
UTL_NAMESPACE_END

#  elif UTL_TARGET_MICROSOFT

// From winnt.h
#    define __UTL_ARM64_SYSREG(op0, op1, crn, crm, op2)                                  \
        (((op0 & 1) << 14) | ((op1 & 7) << 11) | ((crn & 15) << 7) | ((crm & 15) << 3) | \
            ((op2 & 7) << 0))

extern "C" __int64 _ReadStatusReg(int);
extern "C" void __isb(unsigned int);
#    pragma intrinsic(_ReadStatusReg)
#    pragma intrinsic(__isb)

#    define __UTL_ARM64_CNTFRQ __UTL_ARM64_SYSREG(3, 3, 14, 0, 0)
#    define __UTL_ARM64_CNTVCT __UTL_ARM64_SYSREG(3, 3, 14, 0, 2)
#    define __UTL_BARRIER_SY 0xF

static unsigned int clock_frequency() noexcept {
    static unsigned int const value =
        (unsigned int)(_ReadStatusReg(__UTL_ARM64_CNTFRQ) & 0xffffffff);
    return value;
}

UTL_NAMESPACE_BEGIN
namespace tempus {
auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_relaxed)) noexcept
    -> value_type {
    return _ReadStatusReg(ARM64_CNTVCT);
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_acquire)) noexcept
    -> value_type {
    value_type const res = _ReadStatusReg(ARM64_CNTVCT);
    __isb(__UTL_BARRIER_SY);
    return res;
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_release)) noexcept
    -> value_type {
    __isb(__UTL_BARRIER_SY);
    return _ReadStatusReg(ARM64_CNTVCT);
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_acq_rel)) noexcept
    -> value_type {
    __isb(__UTL_BARRIER_SY);
    value_type const res = _ReadStatusReg(ARM64_CNTVCT);
    __isb(__UTL_BARRIER_SY);
    return res;
}

#    undef __UTL_BARRIER_SY
#    undef __UTL_ARM64_CNTVCT
#    undef __UTL_ARM64_CNTFRQ
#    undef __UTL_ARM64_SYSREG

bool hardware_ticks::invariant_frequency() noexcept {
    return true;
}
} // namespace tempus
UTL_NAMESPACE_END

#  else

UTL_PRAGMA_WARN("Unrecognized target/compiler");

UTL_NAMESPACE_BEGIN
namespace tempus {
auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_relaxed)) noexcept
    -> value_type {
    UTL_ASSERT(false);
    UTL_BUILTIN_unreachable();
}
auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_acquire)) noexcept
    -> value_type {
    UTL_ASSERT(false);
    UTL_BUILTIN_unreachable();
}
auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_release)) noexcept
    -> value_type {
    UTL_ASSERT(false);
    UTL_BUILTIN_unreachable();
}
auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_acq_rel)) noexcept
    -> value_type {
    UTL_ASSERT(false);
    UTL_BUILTIN_unreachable();
}

bool hardware_ticks::invariant_frequency() noexcept {
    return false;
}
static unsigned int clock_frequency() noexcept {
    return 1;
}
} // namespace tempus
UTL_NAMESPACE_END
#  endif

UTL_NAMESPACE_BEGIN
namespace tempus {

duration to_duration(hardware_ticks t) noexcept {
    if (!hardware_ticks::invariant_frequency()) {
        return duration::invalid();
    }

    if (t.value() < 0) {
        return duration::invalid();
    }

    static constexpr unsigned int nano = 1000000000;
    return duration{0, t.value() * nano / clock_frequency()};
}

auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_seq_cst)) noexcept
    -> value_type {
    return get_time(instr_order_acq_rel);
}

} // namespace tempus
UTL_NAMESPACE_END

#endif // UTL_ARCH_AARCH64
