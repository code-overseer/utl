// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_COMPILER_MSVC
#  error Invalid compiler
#endif

extern "C" void _mm_mfence(void);
#pragma intrinsic(_mm_mfence)

#if UTL_ARCH_ARM
#  include "utl/hardware/arm/utl_barrier.h"

extern "C" void __dmb(unsigned int);
#  pragma intrinsic(__dmb)
#  define __UTL_ACQUIRE_BARRIER() __dmb(arm::barrier::ISHLD)
#  define __UTL_MEMORY_BARRIER() __dmb(arm::barrier::ISH)
#  define __UTL_STORE_BARRIER() __dmb(arm::barrier::ISHST)
#  define __UTL_HW_MEMORY_BARRIER() __UTL_MEMORY_BARRIER()

#elif UTL_ARCH_x86
#  define __UTL_ACQUIRE_BARRIER() UTL_COMPILER_BARRIER()
#  define __UTL_MEMORY_BARRIER() UTL_COMPILER_BARRIER()
#  define __UTL_STORE_BARRIER() UTL_COMPILER_BARRIER()
#  define __UTL_HW_MEMORY_BARRIER() _mm_mfence()
#else
#  error Unsupported architecture
#endif
