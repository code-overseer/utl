// Copyright 2023-2024 Bryan Wong

#include "utl/assert/utl_assert.h"
#include "utl/platform/utl_clock.h"

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

auto clock_traits<hardware_clock_t>::get_time(__UTL memory_order o) noexcept -> value_type {
    unsigned long long res;
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

} // namespace platform
UTL_NAMESPACE_END

#  elif UTL_TARGET_MICROSOFT

#    include <intrin.h>

// From winnt.h
#    define ARM64_SYSREG(op0, op1, crn, crm, op2)                                        \
        (((op0 & 1) << 14) | ((op1 & 7) << 11) | ((crn & 15) << 7) | ((crm & 15) << 3) | \
            ((op2 & 7) << 0))

#    define ARM64_CNTVCT ARM64_SYSREG(3, 3, 14, 0, 2)      // Generic Timer counter register
#    define ARM64_PMCCNTR_EL0 ARM64_SYSREG(3, 3, 9, 13, 0) // Cycle Count Register [CP15_PMCCNTR]
#    define ARM64_PMSELR_EL0 \
        ARM64_SYSREG(3, 3, 9, 12, 5) // Event Counter Selection Register [CP15_PMSELR]
#    define ARM64_PMXEVCNTR_EL0 \
        ARM64_SYSREG(3, 3, 9, 13, 2) // Event Count Register [CP15_PMXEVCNTR]
#    define ARM64_PMXEVCNTRn_EL0(n) \
        ARM64_SYSREG(3, 3, 14, 8 + ((n) / 8), (n) % 8) // Direct Event Count Register [n/a]
#    define ARM64_TPIDR_EL0 \
        ARM64_SYSREG(3, 3, 13, 0, 2) // Thread ID Register, User Read/Write [CP15_TPIDRURW]
#    define ARM64_TPIDRRO_EL0 \
        ARM64_SYSREG(3, 3, 13, 0, 3) // Thread ID Register, User Read Only [CP15_TPIDRURO]
#    define ARM64_TPIDR_EL1 \
        ARM64_SYSREG(3, 0, 13, 0, 4) // Thread ID Register, Privileged Only [CP15_TPIDRPRW]

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
} // namespace platform
UTL_NAMESPACE_END

#  else

UTL_NAMESPACE_BEGIN
namespace platform {
auto time_point<hardware_clock_t>::get_time(__UTL memory_order o) noexcept -> value_type {
    UTL_ASSERT(false);
    UTL_BUILTIN_unreachable();
}
} // namespace platform
UTL_NAMESPACE_END
#  endif
#endif
