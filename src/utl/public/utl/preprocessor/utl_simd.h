/* Copyright 2023-2024 Bryan Wong */

#pragma once

#if defined(UTL_ARCH_x86)

/* Use SSE4.2 as a minimum SIMD support */
#  ifdef __SSE4_2__
#    define UTL_SIMD_X86_SSE4_2
#  endif
#  ifdef __AVX__
#    define UTL_SIMD_X86_AVX
#  endif

#  ifdef __AVX2__
#    define UTL_SIMD_X86_AVX2
#  endif

#  if defined(__AVX512F__) && defined(__AVX512BW__) && defined(__AVX512CD__) &&  \
      defined(__AVX512DQ__) && defined(__AVX512ER__) && defined(__AVX512PF__) && \
      defined(__AVX512VL__)
#    define UTL_SIMD_X86_AVX512
#  endif

#elif defined(UTL_ARCH_ARM)

#  ifdef __ARM_NEON
#    define UTL_SIMD_ARM_NEON
#  endif

#  ifdef __ARM_FEATURE_SVE
#    define UTL_SIMD_ARM_SVE
#    ifdef __ARM_FEATURE_SVE_BITS
#      define UTL_SIMD_ARM_SVE_BITS __ARM_FEATURE_SVE_BITS
#    endif
#  endif

#  ifdef __ARM_FEATURE_SVE2
#    define UTL_SIMD_ARM_SVE2
#  endif
#endif

#if defined(UTL_SIMD_ARM_SVE2) || defined(UTL_SIMD_ARM_SVE) || defined(UTL_SIMD_ARM_NEON) ||   \
    defined(UTL_SIMD_X86_AVX512) || defined(UTL_SIMD_X86_AVX2) || defined(UTL_SIMD_X86_AVX) || \
    defined(UTL_SIMD_X86_SSE4_2)
#  define UTL_SUPPORTS_SIMD_INTRINSICS
#endif
