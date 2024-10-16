// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_ARCH_x86

#  include "utl/configuration/utl_pragma.h"

#  include "utl/type_traits/utl_constants.h"

#  include <stdint.h>

#  if UTL_COMPILER_MSVC
extern "C" void __cpuid(int*, int);
#    pragma intrinsic(__cpuid)
#  endif

UTL_NAMESPACE_BEGIN

namespace x86 {
namespace {
struct cpuid_t {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
};

#  if UTL_SUPPORTS_GNU_ASM

template <uint32_t Arg>
UTL_ATTRIBUTES(ALWAYS_INLINE) inline cpuid_t cpuid() noexcept {
    cpuid_t result;
    result.eax = Arg;
    __asm__ volatile("cpuid"
                     : "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)
                     : "a"(result.eax)
                     : "memory");
    return result;
}

#  elif UTL_COMPILER_MSVC // UTL_SUPPORTS_GNU_ASM

template <uint32_t Arg>
UTL_ATTRIBUTES(ALWAYS_INLINE) inline cpuid_t cpuid() noexcept {
    cpuid_t result;
    __cpuid(reinterpret_cast<uint32_t*>(&result), Arg);
    return result;
}

#  else

UTL_PRAGMA_WARN("Unrecognized target/compiler");

template <int Arg>
UTL_ATTRIBUTES(ALWAYS_INLINE) inline cpuid_t cpuid() noexcept {
    static_assert(always_false<value_constant<Arg>>(), "Unrecognized target/compiler");
    return {};
}

#  endif // UTL_SUPPORTS_GNU_ASM

} // namespace
} // namespace x86

UTL_NAMESPACE_END

#endif // UTL_ARCH_x86
