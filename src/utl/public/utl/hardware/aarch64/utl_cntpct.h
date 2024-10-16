// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_ARCH_AARCH64

#  include "utl/configuration/utl_pragma.h"

#  include "utl/hardware/aarch64/utl_system_register.h"
#  include "utl/hardware/arm/utl_barrier.h"
#  include "utl/hardware/utl_instruction_order.h"
#  include "utl/type_traits/utl_constants.h"

#  include <stdint.h>

UTL_NAMESPACE_BEGIN
namespace aarch64 {
inline namespace el0 {
namespace {
#  if UTL_HAS_BUILTIN(__builtin_arm_isb) && UTL_HAS_BUILTIN(__builtin_arm_rsr64)

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_relaxed)) noexcept {
    return __builtin_arm_rsr64("CNTPCT_EL0");
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_acquire)) noexcept {
    uint64_t const res = __builtin_arm_rsr64("CNTPCT_EL0");
    __builtin_arm_isb(arm::barrier::SY);
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_release)) noexcept {
    __builtin_arm_isb(arm::barrier::SY);
    return __builtin_arm_rsr64("CNTPCT_EL0");
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_acq_rel)) noexcept {
    __builtin_arm_isb(arm::barrier::SY);
    uint64_t const res = __builtin_arm_rsr64("CNTPCT_EL0");
    __builtin_arm_isb(arm::barrier::SY);
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_seq_cst)) noexcept {
    return cntpct(instruction_order_acq_rel);
}

#  elif UTL_SUPPORTS_GNU_ASM // UTL_HAS_BUILTIN(__builtin_arm_isb) &&
                             // UTL_HAS_BUILTIN(__builtin_arm_rsr64)

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_relaxed)) noexcept {
    uint64_t res;
    __asm__ volatile("mrs %0, CNTPCT_EL0\n\t" : "=r"(res) : : "memory");
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_acquire)) noexcept {
    uint64_t res;
    __asm__ volatile("mrs %0, CNTPCT_EL0\n\t"
                     "isb sy"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_release)) noexcept {
    uint64_t res;
    __asm__ volatile("isb sy\n\t"
                     "mrs %0, CNTPCT_EL0"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_acq_rel)) noexcept {
    uint64_t res;
    __asm__ volatile("isb sy\n\t"
                     "mrs %0, CNTPCT_EL0\n\t"
                     "isb sy\n\t"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_seq_cst)) noexcept {
    return cntpct(instruction_order_acq_rel);
}

#  elif UTL_COMPILER_MSVC // UTL_HAS_BUILTIN(__builtin_arm_isb) &&
                          // UTL_HAS_BUILTIN(__builtin_arm_rsr64)

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_relaxed)) noexcept {
    return _ReadStatusReg(system_register::CNTPCT);
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_acquire)) noexcept {
    value_type const res = _ReadStatusReg(system_register::CNTPCT);
    __isb(arm::barrier::SY);
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_release)) noexcept {
    __isb(arm::barrier::SY);
    return _ReadStatusReg(system_register::CNTPCT);
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_acq_rel)) noexcept {
    __isb(arm::barrier::SY);
    value_type const res = _ReadStatusReg(system_register::CNTPCT);
    __isb(arm::barrier::SY);
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_order_seq_cst)) noexcept {
    return cntpct(instruction_order_acq_rel);
}

#  else // UTL_HAS_BUILTIN(__builtin_arm_isb) && UTL_HAS_BUILTIN(__builtin_arm_rsr64)

UTL_PRAGMA_WARN("Unrecognized target/compiler");

template <instruction_order N>
UTL_ATTRIBUTE(NORETURN) uint64_t cntpct(instruction_order_type<N>) noexcept {
    static_assert(__UTL always_false<instruction_order_type<N>>(), "Unrecognized target/compiler");
    UTL_BUILTIN_unreachable();
}

#  endif // UTL_HAS_BUILTIN(__builtin_arm_isb) && UTL_HAS_BUILTIN(__builtin_arm_rsr64)

} // namespace
} // namespace el0
} // namespace aarch64
UTL_NAMESPACE_END

#endif // UTL_ARCH_AARCH64
