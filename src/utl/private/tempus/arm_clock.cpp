// Copyright 2023-2024 Bryan Wong

#include "utl/assert/utl_assert.h"
#include "utl/atomic/utl_atomic.h"
#include "utl/tempus/utl_clock.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_same.h"

// TODO Investigate using PMU counter (which is not invariant)
#if UTL_ARCH_AARCH64

#  define __UTL_SUPPORTS_ARM_STATUS_REGISTER_BUILTINS \
      (UTL_HAS_BUILTIN(__builtin_arm_isb) && UTL_HAS_BUILTIN(__builtin_arm_rsr64))
#  define __UTL_BARRIER_SY 0xF

#  if __UTL_SUPPORTS_ARM_STATUS_REGISTER_BUILTINS
UTL_NAMESPACE_BEGIN
namespace {
UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntfrq_el0() noexcept {
    return __builtin_arm_rsr64("CNTFRQ_EL0");
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_relaxed)) noexcept {
    return __builtin_arm_rsr64("CNTPCT_EL0");
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_acquire)) noexcept {
    uint64_t const res = __builtin_arm_rsr64("CNTPCT_EL0");
    __builtin_arm_isb(__UTL_BARRIER_SY);
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_release)) noexcept {
    __builtin_arm_isb(__UTL_BARRIER_SY);
    return __builtin_arm_rsr64("CNTPCT_EL0");
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_acq_rel)) noexcept {
    __builtin_arm_isb(__UTL_BARRIER_SY);
    uint64_t const res = __builtin_arm_rsr64("CNTPCT_EL0");
    __builtin_arm_isb(__UTL_BARRIER_SY);
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_relaxed)) noexcept {
    return __builtin_arm_rsr64("PMCCTR_EL0");
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_acquire)) noexcept {
    uint64_t const res = __builtin_arm_rsr64("PMCCTR_EL0");
    __builtin_arm_isb(__UTL_BARRIER_SY);
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_release)) noexcept {
    __builtin_arm_isb(__UTL_BARRIER_SY);
    return __builtin_arm_rsr64("PMCCTR_EL0");
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_acq_rel)) noexcept {
    __builtin_arm_isb(__UTL_BARRIER_SY);
    uint64_t const res = __builtin_arm_rsr64("PMCCTR_EL0");
    __builtin_arm_isb(__UTL_BARRIER_SY);
    return res;
}
} // namespace
UTL_NAMESPACE_END

#    undef __UTL_BARRIER_SY

#  elif UTL_SUPPORTS_GNU_ASM // __UTL_SUPPORTS_ARM_STATUS_REGISTER_BUILTINS

UTL_NAMESPACE_BEGIN
namespace {
UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntfrq_el0() noexcept {
    uint64_t res;
    __asm__ volatile("mrs %0, CNTFRQ_EL0\n\t" : "=r"(res) : : "memory");
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_relaxed)) noexcept {
    uint64_t res;
    __asm__ volatile("mrs %0, CNTPCT_EL0\n\t" : "=r"(res) : : "memory");
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_acquire)) noexcept {
    uint64_t res;
    __asm__ volatile("mrs %0, CNTPCT_EL0\n\t"
                     "isb sy"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_release)) noexcept {
    uint64_t res;
    __asm__ volatile("isb sy\n\t"
                     "mrs %0, CNTPCT_EL0"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_acq_rel)) noexcept {
    uint64_t res;
    __asm__ volatile("isb sy\n\t"
                     "mrs %0, CNTPCT_EL0\n\t"
                     "isb sy\n\t"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_relaxed)) noexcept {
    uint64_t res;
    __asm__ volatile("mrs %0, PMCCNTR_EL0\n\t" : "=r"(res) : : "memory");
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_acquire)) noexcept {
    uint64_t res;
    __asm__ volatile("mrs %0, PMCCNTR_EL0\n\t"
                     "isb sy"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_release)) noexcept {
    uint64_t res;
    __asm__ volatile("isb sy\n\t"
                     "mrs %0, PMCCNTR_EL0"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_acq_rel)) noexcept {
    uint64_t res;
    __asm__ volatile("isb sy\n\t"
                     "mrs %0, PMCCNTR_EL0\n\t"
                     "isb sy\n\t"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}
} // namespace
UTL_NAMESPACE_END

#  elif UTL_TARGET_MICROSOFT // __UTL_SUPPORTS_ARM_STATUS_REGISTER_BUILTINS

extern "C" __int64 _ReadStatusReg(int);
extern "C" void __isb(unsigned int);
#    pragma intrinsic(_ReadStatusReg)
#    pragma intrinsic(__isb)

// From winnt.h
#    define __UTL_ARM64_SYSREG(op0, op1, crn, crm, op2)                                  \
        (((op0 & 1) << 14) | ((op1 & 7) << 11) | ((crn & 15) << 7) | ((crm & 15) << 3) | \
            ((op2 & 7) << 0))

#    define __UTL_ARM64_CNTFRQ __UTL_ARM64_SYSREG(3, 3, 14, 0, 0)
#    define __UTL_ARM64_CNTPCT __UTL_ARM64_SYSREG(3, 3, 14, 0, 2)
#    define __UTL_ARM64_PMCCNTR __UTL_ARM64_SYSREG(3, 3, 9, 13, 0)
#    define __UTL_BARRIER_SY 0xF

#    undef __UTL_ARM64_SYSREG

UTL_NAMESPACE_BEGIN
namespace {
UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntfrq_el0() noexcept {
    return _ReadStatusReg(__UTL_ARM64_CNTFRQ);
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_relaxed)) noexcept {
    return _ReadStatusReg(__UTL_ARM64_CNTPCT);
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_acquire)) noexcept {
    value_type const res = _ReadStatusReg(__UTL_ARM64_CNTPCT);
    __isb(__UTL_BARRIER_SY);
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_release)) noexcept {
    __isb(__UTL_BARRIER_SY);
    return _ReadStatusReg(__UTL_ARM64_CNTPCT);
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(decltype(instr_order_acq_rel)) noexcept {
    __isb(__UTL_BARRIER_SY);
    value_type const res = _ReadStatusReg(__UTL_ARM64_CNTPCT);
    __isb(__UTL_BARRIER_SY);
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_relaxed)) noexcept {
    return _ReadStatusReg(__UTL_ARM64_PMCCNTR);
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_acquire)) noexcept {
    value_type const res = _ReadStatusReg(__UTL_ARM64_PMCCNTR);
    __isb(__UTL_BARRIER_SY);
    return res;
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_release)) noexcept {
    __isb(__UTL_BARRIER_SY);
    return _ReadStatusReg(__UTL_ARM64_PMCCNTR);
}

UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(decltype(instr_order_acq_rel)) noexcept {
    __isb(__UTL_BARRIER_SY);
    value_type const res = _ReadStatusReg(__UTL_ARM64_PMCCNTR);
    __isb(__UTL_BARRIER_SY);
    return res;
}
} // namespace
UTL_NAMESPACE_END

#    undef __UTL_BARRIER_SY
#    undef __UTL_ARM64_CNTPCT
#    undef __UTL_ARM64_CNTFRQ
#    undef __UTL_ARM64_SYSREG

#  else // __UTL_SUPPORTS_ARM_STATUS_REGISTER_BUILTINS

UTL_PRAGMA_WARN("Unrecognized target/compiler");

#    define __UTL_NO_INVARIANT_CNT_FREQ

UTL_NAMESPACE_BEGIN
namespace {
template <instruction_order O>
UTL_ATTRIBUTES(NORETURN, MAYBE_UNUSED) uint64_t arm64_cntpct_el0(instr_order_t<O>) noexcept {
    UTL_ASSERT(false);
    UTL_BUILTIN_unreachable();
}
template <instruction_order O>
UTL_ATTRIBUTES(NORETURN, MAYBE_UNUSED) uint64_t arm64_pmccntr_el0(instr_order_t<O>) noexcept {
    UTL_ASSERT(false);
    UTL_BUILTIN_unreachable();
}
UTL_ATTRIBUTES(NORETURN, MAYBE_UNUSED) uint64_t arm64_cntfrq_el0() noexcept {
    UTL_ASSERT(false);
    UTL_BUILTIN_unreachable();
}
} // namespace
UTL_NAMESPACE_END
#  endif // __UTL_SUPPORTS_ARM_STATUS_REGISTER_BUILTINS

UTL_NAMESPACE_BEGIN
namespace {

uint32_t clock_frequency() noexcept {
// Provide escape hatch for systems with broken frequencies
#  ifndef UTL_ARM_CNTFRQ
    // TODO Prescaler?
    static uint32_t value = []() { return (arm64_cntfrq_el0() & 0xffffffffu); }();
    return value;
#  else
    static_assert(
        is_convertible<decltype(UTL_ARM_CNTFRQ), uint32_t>::value, "Invalid `UTL_ARM_CNTFRQ`");
    return UTL_ARM_CNTFRQ;
#  endif
}

template <instruction_order O>
uint64_t get_timestamp(instr_order_t<O> o) noexcept {
#  ifndef UTL_USE_PMU_HARDWARE_CLOCK
    return arm64_cntpct_el0(o);
#  else
    return arm64_pmccntr_el0(o);
#  endif
}

template <>
uint64_t get_timestamp(decltype(instr_order_seq_cst)) noexcept {
    return get_timestamp(instr_order_acq_rel);
}

} // namespace

namespace tempus {
bool hardware_ticks::invariant_frequency() noexcept {
#  if !defined(__UTL_NO_INVARIANT_CNT_FREQ) && !defined(UTL_USE_PMU_HARDWARE_CLOCK)
    return clock_frequency() != 0;
#  else
    return false;
#  endif
}

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

#  define __UTL_DEFINE_GET_TIME(ORDER)                                                        \
      auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_##ORDER) o) noexcept \
          -> value_type {                                                                     \
          return get_timestamp(o);                                                            \
      }
__UTL_DEFINE_GET_TIME(relaxed);
__UTL_DEFINE_GET_TIME(acquire);
__UTL_DEFINE_GET_TIME(release);
__UTL_DEFINE_GET_TIME(acq_rel);
__UTL_DEFINE_GET_TIME(seq_cst);

#  undef __UTL_DEFINE_GET_TIME

} // namespace tempus
UTL_NAMESPACE_END

#endif // UTL_ARCH_AARCH64
