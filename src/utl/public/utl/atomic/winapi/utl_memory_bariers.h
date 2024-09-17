// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_COMPILER_MSVC
#  error Invalid compiler
#endif

extern "C" void _ReadWriteBarrier(void);
extern "C" void _mm_mfence(void);
#pragma intrinsic(_ReadWriteBarrier)
#pragma intrinsic(_mm_mfence)

#define __UTL_COMPILER_BARRIER() _ReadWriteBarrier()

#if UTL_ARCH_ARM
extern "C" void __dmb(unsigned int);
#  pragma intrinsic(__dmb)
// Inner shareable load barrier 0x9
#  define __UTL_ACQUIRE_BARRIER() __dmb(0x9)
// Inner shareable 0x8
#  define __UTL_MEMORY_BARRIER() __dmb(0xB)
// Inner shareable store barrier 0x8
#  define __UTL_STORE_BARRIER() __dmb(0xA)
#  define __UTL_HW_MEMORY_BARRIER() __UTL_MEMORY_BARRIER()

#elif UTL_ARCH_x86
#  define __UTL_ACQUIRE_BARRIER() _ReadWriteBarrier()
#  define __UTL_MEMORY_BARRIER() _ReadWriteBarrier()
#  define __UTL_STORE_BARRIER() _ReadWriteBarrier()
#  define __UTL_HW_MEMORY_BARRIER() _mm_mfence()
#else
#  error Unsupported architecture
#endif
