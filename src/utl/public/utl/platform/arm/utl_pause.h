// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_PAUSE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_ARCH_ARM
#  if UTL_TARGET_MICROSOFT && !UTL_SUPPORTS_GNU_ASM
extern "C" void __yield(void);
#    define __UTL_PAUSE() __yield()
#  else
#    define __UTL_PAUSE() __asm__ volatile("yield")
#  endif

UTL_NAMESPACE_BEGIN

namespace platform {
inline void pause() noexcept {
    __UTL_PAUSE();
}
} // namespace platform
#  undef __UTL_PAUSE

UTL_NAMESPACE_END

#endif
