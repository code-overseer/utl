// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_COMPILER_GNU_BASED
#  error Invalid compiler
#endif

#define __UTL_COMPILER_BARRIER() __asm__ volatile("" : : : "memory")
#if UTL_ARCH_ARM
#  if UTL_HAS_BUILTIN(__builtin_arm_dmb)
// Inner shareable load barrier 0x9
#    define __UTL_ACQUIRE_BARRIER() __builtin_arm_dmb(0x9)
// Inner shareable 0x8
#    define __UTL_MEMORY_BARRIER() __builtin_arm_dmb(0xB)
// Inner shareable store barrier 0x8
#    define __UTL_STORE_BARRIER() __builtin_arm_dmb(0xA)
#  else
#    define __UTL_ACQUIRE_BARRIER() __asm__ volatile("dmb ishld" : : : "memory")
#    define __UTL_MEMORY_BARRIER() __asm__ volatile("dmb ish" : : : "memory")
#    define __UTL_STORE_BARRIER() __asm__ volatile("dmb ishst" : : : "memory")
#  endif
#  define __UTL_HW_MEMORY_BARRIER() __UTL_MEMORY_BARRIER()

#elif UTL_ARCH_x86
#  include <emmintrin.h>
#  define __UTL_ACQUIRE_BARRIER() __asm__ volatile("" : : : "memory")
#  define __UTL_MEMORY_BARRIER() __asm__ volatile("" : : : "memory")
#  define __UTL_STORE_BARRIER() __asm__ volatile("" : : : "memory")
#  define __UTL_HW_MEMORY_BARRIER() _mm_mfence()
#else
#  error Unsupported architecture
#endif
