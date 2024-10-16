// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_ARCH_x86

#  include "utl/configuration/utl_pragma.h"

#  include "utl/hardware/utl_instruction_order.h"
#  include "utl/type_traits/utl_constants.h"

#  include <stdint.h>

#  if UTL_COMPILER_MSVC

UTL_EXTERN_C_BEGIN
unsigned __int64 __rdtscp(unsigned int*);
void _mm_sfence(void);
void _mm_lfence(void);
void _mm_mfence(void);
UTL_EXTERN_C_END

#    pragma intrinsic(__rdtscp)
#    pragma intrinsic(_mm_sfence)
#    pragma intrinsic(_mm_lfence)
#    pragma intrinsic(_mm_mfence)

#  endif

UTL_NAMESPACE_BEGIN

namespace x86 {
namespace {
struct rdtcp_t {
    uint64_t timestamp;
    uint32_t aux;
};

#  if UTL_SUPPORTS_GNU_ASM

UTL_ATTRIBUTES(ALWAYS_INLINE) inline rdtscp_t rdtscp(decltype(instruction_order_relaxed)) noexcept {
    uint64_t high;
    uint64_t low;
    uint32_t aux;
    __asm__("rdtscp" : "=a"(low), "=d"(high), "=c"(aux) : :);
    return rdtcp_t{(high << 32) | low, aux};
}

UTL_ATTRIBUTES(ALWAYS_INLINE) inline rdtscp_t rdtscp(decltype(instruction_order_acquire)) noexcept {
    uint64_t high;
    uint64_t low;
    uint32_t aux;
    __asm__("rdtscp\n\t"
            "lfence"
            : "=a"(low), "=d"(high), "=c"(aux)
            :
            : "memory");

    return rdtcp_t{(high << 32) | low, aux};
}

UTL_ATTRIBUTES(ALWAYS_INLINE) inline rdtscp_t rdtscp(decltype(instruction_order_release)) noexcept {
    uint64_t high;
    uint64_t low;
    uint32_t aux;
    __asm__("mfence\n\t"
            "rdtscp"
            : "=a"(low), "=d"(high), "=c"(aux)
            :
            : "memory");
    return rdtcp_t{(high << 32) | low, aux};
}

UTL_ATTRIBUTES(ALWAYS_INLINE) inline rdtscp_t rdtscp(decltype(instruction_order_acq_rel)) noexcept {
    uint64_t high;
    uint64_t low;
    uint32_t aux;
    __asm__("mfence\n\t"
            "rdtscp\n\t"
            "lfence"
            : "=a"(low), "=d"(high), "=c"(aux)
            :
            : "memory");
    return rdtcp_t{(high << 32) | low, aux};
}

UTL_ATTRIBUTES(ALWAYS_INLINE) inline rdtscp_t rdtscp(decltype(instruction_order_seq_cst)) noexcept {
    return rdtscp(instruction_order_acq_rel);
}

#  elif UTL_COMPILER_MSVC // UTL_SUPPORTS_GNU_ASM

UTL_ATTRIBUTES(ALWAYS_INLINE) inline rdtscp_t rdtscp(decltype(instruction_order_relaxed)) noexcept {
    rdtscp_t result;
    result.timestamp = __rdtscp(&result.aux);
    return result;
}

UTL_ATTRIBUTES(ALWAYS_INLINE) inline rdtscp_t rdtscp(decltype(instruction_order_acquire)) noexcept {
    rdtscp_t result;
    result.timestamp = __rdtscp(&result.aux);
    _mm_lfence();
    return result;
}

UTL_ATTRIBUTES(ALWAYS_INLINE) inline rdtscp_t rdtscp(decltype(instruction_order_release)) noexcept {
    rdtscp_t result;
    _mm_mfence();
    result.timestamp = __rdtscp(&result.aux);
    return result;
}

UTL_ATTRIBUTES(ALWAYS_INLINE) inline rdtscp_t rdtscp(decltype(instruction_order_acq_rel)) noexcept {
    rdtscp_t result;
    _mm_mfence();
    result.timestamp = __rdtscp(&result.aux);
    _mm_lfence();
    return result;
}

UTL_ATTRIBUTES(ALWAYS_INLINE) inline rdtscp_t rdtscp(decltype(instruction_order_seq_cst)) noexcept {
    return rdtscp(instruction_order_acq_rel);
}
#  else

UTL_PRAGMA_WARN("Unrecognized target/compiler");

template <instruction_order N>
UTL_ATTRIBUTES(ALWAYS_INLINE, MAYBE_UNUSED) inline rdtscp_t rdtscp(instruction_order_type<N>) noexcept {
    static_assert(__UTL always_false<instruction_order_type<N>>(), "Unrecognized target/compiler");
    return {};
}

#  endif // UTL_SUPPORTS_GNU_ASM

} // namespace
} // namespace x86

UTL_NAMESPACE_END

#endif // UTL_ARCH_x86
