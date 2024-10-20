// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_ARCH_x86

#  if UTL_TARGET_MICROSOFT && !UTL_SUPPORTS_GNU_ASM
extern "C" void _mm_pause(void);
#    define __UTL_PAUSE() _mm_pause()
#  else
#    define __UTL_PAUSE() __asm__ volatile("pause" : : : "memory")
#  endif
UTL_NAMESPACE_BEGIN

namespace x86 {
namespace {
UTL_ATTRIBUTE(ALWAYS_INLINE) inline void pause() noexcept {
    __UTL_PAUSE();
}
} // namespace
} // namespace x86

UTL_NAMESPACE_END

#  undef __UTL_PAUSE

#endif
