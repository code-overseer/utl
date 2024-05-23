/* Copyright 2023-2024 Bryan Wong */

#pragma once

#if defined(UTL_ARCH_x86)

/* Use SSE4.2 as a minimum SIMD support */
#  ifdef __SSE4_2__
#    define UTL_SIMD_X86_SSE4_2 1
#  endif
#  ifdef __AVX__
#    define UTL_SIMD_X86_AVX 1
#  endif

#  ifdef __AVX2__
#    define UTL_SIMD_X86_AVX2 1
#  endif

#  if defined(__AVX512F__) && defined(__AVX512BW__) && defined(__AVX512CD__) &&  \
      defined(__AVX512DQ__) && defined(__AVX512ER__) && defined(__AVX512PF__) && \
      defined(__AVX512VL__)
#    define UTL_SIMD_X86_AVX512 1
#  endif

#elif defined(UTL_ARCH_ARM)

#  ifdef __ARM_NEON
#    define UTL_SIMD_ARM_NEON 1
#  endif

#  ifdef __ARM_FEATURE_SVE
#    define UTL_SIMD_ARM_SVE 1
#    ifdef __ARM_FEATURE_SVE_BITS
#      define UTL_SIMD_ARM_SVE_BITS __ARM_FEATURE_SVE_BITS
#    endif
#  endif

#  ifdef __ARM_FEATURE_SVE2
#    define UTL_SIMD_ARM_SVE2 1
#  endif
#endif

#if UTL_SIMD_ARM_SVE2 || UTL_SIMD_ARM_SVE || UTL_SIMD_ARM_NEON || UTL_SIMD_X86_AVX512 || \
    UTL_SIMD_X86_AVX2 || UTL_SIMD_X86_AVX || UTL_SIMD_X86_SSE4_2
#  define UTL_SUPPORTS_SIMD_INTRINSICS 1
#endif
