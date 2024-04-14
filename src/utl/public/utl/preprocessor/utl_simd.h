/* Copyright 2023-2024 Bryan Wong */

#pragma once

#if defined(UTL_ARCH_x86)

#  if defined(__SSE4_2__) | defined(__AVX__) | defined(__AVX2__) | defined(__AVX512F__)
#    define UTL_SUPPORTS_SIMD_INTRINSICS
#  endif
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

#  ifdef __AVX512F__
#    define UTL_SIMD_X86_AVX512
#  endif

#elif defined(UTL_ARCH_ARM)
#  if defined(__ARM_NEON) | defined(__ARM_FEATURE_SVE) | defined(__ARM_FEATURE_SVE2)
#    define UTL_SUPPORTS_SIMD_INTRINSICS
#  endif

#  ifdef __ARM_NEON
#    define UTL_SIMD_ARM_NEON
#  endif

#  ifdef __ARM_FEATURE_SVE
#    define UTL_SIMD_ARM_SVE
#  endif

#  ifdef __ARM_FEATURE_SVE2
#    define UTL_SIMD_ARM_SVE2
#  endif

#endif
