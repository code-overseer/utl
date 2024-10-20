// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_ARCH_x86

#  include "utl/configuration/utl_pragma.h"

#  include "utl/hardware/utl_instruction_barrier.h"
#  include "utl/type_traits/utl_constants.h"

#  include <stdint.h>

#  if UTL_COMPILER_MSVC

UTL_EXTERN_C_BEGIN
unsigned __int64 __rdtsc();
void _mm_sfence(void);
void _mm_lfence(void);
void _mm_mfence(void);
UTL_EXTERN_C_END

#    pragma intrinsic(__rdtsc)
#    pragma intrinsic(_mm_sfence)
#    pragma intrinsic(_mm_lfence)
#    pragma intrinsic(_mm_mfence)

#  endif

UTL_NAMESPACE_BEGIN

namespace x86 {
namespace {

#  if UTL_SUPPORTS_GNU_ASM

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t rdtsc(decltype(instruction_barrier_none)) noexcept {
    uint64_t high;
    uint64_t low;
    __asm__("rdtsc" : "=a"(low), "=d"(high) : :);
    return (high << 32) | low;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t rdtsc(decltype(instruction_barrier_after)) noexcept {
    uint64_t high;
    uint64_t low;
    uint32_t aux;
    __asm__("rdtsc\n\t"
            "lfence"
            : "=a"(low), "=d"(high)
            :
            : "memory");

    return (high << 32) | low;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t rdtsc(decltype(instruction_barrier_before)) noexcept {
    uint64_t high;
    uint64_t low;
    uint32_t aux;
    __asm__("mfence\n\t"
            "lfence\n\t"
            "rdtsc"
            : "=a"(low), "=d"(high)
            :
            : "memory");
    return (high << 32) | low;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t rdtsc(decltype(instruction_barrier_enclose)) noexcept {
    uint64_t high;
    uint64_t low;
    uint32_t aux;
    __asm__("mfence\n\t"
            "lfence\n\t"
            "rdtsc\n\t"
            "lfence"
            : "=a"(low), "=d"(high)
            :
            : "memory");
    return (high << 32) | low;
}

#  elif UTL_COMPILER_MSVC // UTL_SUPPORTS_GNU_ASM

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t rdtsc(decltype(instruction_barrier_none)) noexcept {
    return __rdtsc();
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t rdtsc(decltype(instruction_barrier_after)) noexcept {
    uint64_t const result = __rdtsc();
    _mm_lfence();
    UTL_COMPILER_BARRIER();
    return result;
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t rdtsc(decltype(instruction_barrier_before)) noexcept {
    UTL_COMPILER_BARRIER();
    _mm_mfence();
    _mm_lfence();
    return __rdtsc();
}

UTL_ATTRIBUTE(ALWAYS_INLINE) inline uint64_t rdtsc(decltype(instruction_barrier_enclose)) noexcept {
    UTL_COMPILER_BARRIER();
    _mm_mfence();
    _mm_lfence();
    uint64_t const result = __rdtsc();
    _mm_lfence();
    UTL_COMPILER_BARRIER();
    return result;
}

#  else

UTL_PRAGMA_WARN("Unrecognized target/compiler");

template <instruction_barrier N>
UTL_ATTRIBUTE(NORETURN) inline uint64_t rdtsc(instruction_barrier_type<N>) noexcept {
    static_assert(
        __UTL always_false<instruction_barrier_type<N>>(), "Unrecognized target/compiler");
    UTL_BUILTIN_unreachable();
}

#  endif // UTL_SUPPORTS_GNU_ASM

} // namespace
} // namespace x86

UTL_NAMESPACE_END

#endif // UTL_ARCH_x86
