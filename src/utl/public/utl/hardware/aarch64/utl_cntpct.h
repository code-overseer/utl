// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_ARCH_AARCH64

#  include "utl/configuration/utl_pragma.h"

#  include "utl/hardware/aarch64/utl_system_register.h"
#  include "utl/hardware/arm/utl_barrier.h"
#  include "utl/hardware/utl_instruction_barrier.h"
#  include "utl/type_traits/utl_constants.h"

#  include <stdint.h>

UTL_NAMESPACE_BEGIN
namespace aarch64 {
inline namespace el0 {
namespace {
#  if UTL_HAS_BUILTIN(__builtin_arm_isb) && UTL_HAS_BUILTIN(__builtin_arm_rsr64)

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_none)) noexcept {
    return __builtin_arm_rsr64("CNTPCT_EL0");
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_after)) noexcept {
    uint64_t const res = __builtin_arm_rsr64("CNTPCT_EL0");
    __builtin_arm_isb(arm::barrier::SY);
    UTL_COMPILER_BARRIER();
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_before)) noexcept {
    UTL_COMPILER_BARRIER();
    __builtin_arm_isb(arm::barrier::SY);
    return __builtin_arm_rsr64("CNTPCT_EL0");
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_enclose)) noexcept {
    UTL_COMPILER_BARRIER();
    __builtin_arm_isb(arm::barrier::SY);
    uint64_t const res = __builtin_arm_rsr64("CNTPCT_EL0");
    __builtin_arm_isb(arm::barrier::SY);
    UTL_COMPILER_BARRIER();
    return res;
}

#  elif UTL_SUPPORTS_GNU_ASM // UTL_HAS_BUILTIN(__builtin_arm_isb) &&
                             // UTL_HAS_BUILTIN(__builtin_arm_rsr64)

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_none)) noexcept {
    uint64_t res;
    __asm__("mrs %0, CNTPCT_EL0\n\t" : "=r"(res) : :);
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_after)) noexcept {
    uint64_t res;
    __asm__ volatile("mrs %0, CNTPCT_EL0\n\t"
                     "isb sy"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_before)) noexcept {
    uint64_t res;
    __asm__ volatile("isb sy\n\t"
                     "mrs %0, CNTPCT_EL0"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_enclose)) noexcept {
    uint64_t res;
    __asm__ volatile("isb sy\n\t"
                     "mrs %0, CNTPCT_EL0\n\t"
                     "isb sy\n\t"
                     : "=r"(res)
                     :
                     : "memory");
    return res;
}

#  elif UTL_COMPILER_MSVC // UTL_HAS_BUILTIN(__builtin_arm_isb) &&
                          // UTL_HAS_BUILTIN(__builtin_arm_rsr64)

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_none)) noexcept {
    return _ReadStatusReg(system_register::CNTPCT);
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_after)) noexcept {
    value_type const res = _ReadStatusReg(system_register::CNTPCT);
    __isb(arm::barrier::SY);
    UTL_COMPILER_BARRIER();
    return res;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_before)) noexcept {
    UTL_COMPILER_BARRIER();
    __isb(arm::barrier::SY);
    return _ReadStatusReg(system_register::CNTPCT);
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t cntpct(decltype(instruction_barrier_enclose)) noexcept {
    UTL_COMPILER_BARRIER();
    __isb(arm::barrier::SY);
    value_type const res = _ReadStatusReg(system_register::CNTPCT);
    __isb(arm::barrier::SY);
    UTL_COMPILER_BARRIER();
    return res;
}

#  else // UTL_HAS_BUILTIN(__builtin_arm_isb) && UTL_HAS_BUILTIN(__builtin_arm_rsr64)

UTL_PRAGMA_WARN("Unrecognized target/compiler");

template <instruction_barrier N>
UTL_ATTRIBUTE(NORETURN) uint64_t cntpct(instruction_barrier_type<N>) noexcept {
    static_assert(
        __UTL always_false<instruction_barrier_type<N>>(), "Unrecognized target/compiler");
    UTL_BUILTIN_unreachable();
}

#  endif // UTL_HAS_BUILTIN(__builtin_arm_isb) && UTL_HAS_BUILTIN(__builtin_arm_rsr64)

} // namespace
} // namespace el0
} // namespace aarch64
UTL_NAMESPACE_END

#endif // UTL_ARCH_AARCH64
