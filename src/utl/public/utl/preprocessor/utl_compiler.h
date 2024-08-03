/* Copyright 2023-2024 Bryan Wong */

#pragma once

#if defined(__INTEL_LLVM_COMPILER)
#  define UTL_COMPILER_ICX 1
#  if defined(SYCL_LANGUAGE_VERSION)
#    define UTL_COMPILER_ICX_DPCPP 1
#  endif
#  define UTL_COMPILER_ICX_AT_LEAST(VERSION) __INTEL_LLVM_COMPILER >= VERSION
#  define UTL_COMPILER_ICX_DPCPP_AT_LEAST(VERSION) SYCL_LANGUAGE_VERSION >= VERSION
#elif defined(__clang__)
#  define UTL_COMPILER_CLANG 1
#  define UTL_COMPILER_CLANG_AT_LEAST(MAJOR, MINOR, PATCH) \
      __clang_major__ > MAJOR ||                           \
          (__clang_major__ == MAJOR &&                     \
              (__clang_minor__ > MINOR ||                  \
                  (__clang_minor__ == MINOR && __clang_patchlevel__ > PATCH)))
#elif defined(__INTEL_COMPILER)
#  define UTL_COMPILER_ICC 1
#  define UTL_COMPILER_ICC_AT_LEAST(VERSION) __VERSION__ >= VERSION
#elif defined(__GNUC__)
#  define UTL_COMPILER_GCC 1
#  define UTL_COMPILER_GCC_AT_LEAST(MAJOR, MINOR, PATCH) \
      __GNUC__ > MAJOR ||                                \
          (__GNUC__ == MAJOR &&                          \
              (__GNUC_MINOR__ > MINOR ||                 \
                  (__GNUC_MINOR__ == MINOR && __GNUC_PATCHLEVEL__ > PATCH)))
#elif defined(_MSC_VER)
#  define UTL_COMPILER_MSVC 1
#  define UTL_COMPILER_SUPPORTS_DECLSPEC 1
#  define UTL_COMPILER_MSVC_AT_LEAST(VERSION) _MSC_VER >= VERSION
#  if _MSVC_TRADITIONAL
/* Using non-conformant MSVC preprocessor */
#    define UTL_MSVC_PREPROCESSOR 1
#  endif
#elif
#  error "Unknown compiler"
#endif

#ifndef UTL_COMPILER_CLANG_AT_LEAST
#  define UTL_COMPILER_CLANG_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_CLANG_AT_LEAST */

#ifndef UTL_COMPILER_ICX_AT_LEAST
#  define UTL_COMPILER_ICX_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_ICX_AT_LEAST */

#ifndef UTL_COMPILER_ICC_AT_LEAST
#  define UTL_COMPILER_ICC_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_ICX_AT_LEAST */

#ifndef UTL_COMPILER_ICX_DPCPP_AT_LEAST
#  define UTL_COMPILER_ICX_DPCPP_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_ICX_DPCPP_AT_LEAST */

#ifndef UTL_COMPILER_GCC_AT_LEAST
#  define UTL_COMPILER_GCC_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_GCC_AT_LEAST */

#ifndef UTL_COMPILER_MSVC_AT_LEAST
#  define UTL_COMPILER_MSVC_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_MSVC_AT_LEAST */

#if UTL_COMPILER_MSVC && !UTL_COMPILER_MSVC_AT_LEAST(1914)
#  error "Minimum version for MSVC not met"
#endif

#if UTL_COMPILER_GCC && !UTL_COMPILER_GCC_AT_LEAST(4, 8, 1)
#  error "Minimum version for GCC not met"
#endif

#if UTL_COMPILER_CLANG && !UTL_COMPILER_CLANG_AT_LEAST(3, 4, 1)
/* Clang has a preprocessor bug in 3.3 */
#  error "Minimum version for Clang not met"
#endif

#if UTL_COMPILER_CLANG | UTL_COMPILER_GCC | UTL_COMPILER_ICC | UTL_COMPILER_ICX
#  define UTL_SUPPORTS_GNU_ASM 1
#endif
