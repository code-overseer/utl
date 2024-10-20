// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_PAUSE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_ARCH_ARM
#  if UTL_TARGET_MICROSOFT && !UTL_SUPPORTS_GNU_ASM
extern "C" void __yield(void);
#    define __UTL_YIELD() __yield()
#  else
#    define __UTL_YIELD() __asm__ volatile("yield" : : : "memory")
#  endif

UTL_NAMESPACE_BEGIN

namespace arm {
namespace {
UTL_ATTRIBUTE(ALWAYS_INLINE) inline void yield() noexcept {
    __UTL_YIELD();
}
} // namespace
} // namespace arm
#  undef __UTL_YIELD

UTL_NAMESPACE_END

#endif
