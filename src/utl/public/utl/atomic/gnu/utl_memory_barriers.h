// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_COMPILER_GNU_BASED
#  error Invalid compiler
#endif

#if UTL_ARCH_ARM
#  include "utl/hardware/arm/utl_barrier.h"
#  if UTL_HAS_BUILTIN(__builtin_arm_dmb)
#    define __UTL_ACQUIRE_BARRIER() __builtin_arm_dmb(arm::barrier::ISHLD)
#    define __UTL_MEMORY_BARRIER() __builtin_arm_dmb(arm::barrier::ISH)
#    define __UTL_STORE_BARRIER() __builtin_arm_dmb(arm::barrier::ISHST)
#  else
#    define __UTL_ACQUIRE_BARRIER() __asm__ volatile("dmb ishld" : : : "memory")
#    define __UTL_MEMORY_BARRIER() __asm__ volatile("dmb ish" : : : "memory")
#    define __UTL_STORE_BARRIER() __asm__ volatile("dmb ishst" : : : "memory")
#  endif
#  define __UTL_HW_MEMORY_BARRIER() __UTL_MEMORY_BARRIER()

#elif UTL_ARCH_x86
#  define __UTL_ACQUIRE_BARRIER() UTL_COMPILER_BARRIER()
#  define __UTL_MEMORY_BARRIER() UTL_COMPILER_BARRIER()
#  define __UTL_STORE_BARRIER() UTL_COMPILER_BARRIER()
#  define __UTL_HW_MEMORY_BARRIER() __asm__ volatile("mfence" : : : "memory")
#else
#  error Unsupported architecture
#endif
