/* Copyright 2023-2024 Bryan Wong */

#pragma once

#define UTL_COMPILER_VERSION_CONCAT_1(MAJOR, MINOR, PATCH) MAJOR##MINOR##PATCH
#define UTL_COMPILER_VERSION_CONCAT(MAJOR, MINOR, PATCH) \
    UTL_COMPILER_VERSION_CONCAT_1(MAJOR, MINOR, PATCH)

#if defined(__INTEL_LLVM_COMPILER)
#  define UTL_COMPILER_ICX 1
#  define UTL_COMPILER_ICX_AT_LEAST(VERSION) __INTEL_LLVM_COMPILER >= VERSION
#  if defined(SYCL_LANGUAGE_VERSION)
#    define UTL_COMPILER_ICX_DPCPP 1
#    define UTL_COMPILER_ICX_DPCPP_AT_LEAST(VERSION) SYCL_LANGUAGE_VERSION >= VERSION
#    define UTL_COMPILER_TAG \
        UTL_CONCAT(          \
            UTL_CONCAT(UTL_CONCAT(ICX_DPCPP, __INTEL_LLVM_COMPILER), _), SYCL_LANGUAGE_VERSION)
#  else
#    define UTL_COMPILER_TAG UTL_CONCAT(ICX, __INTEL_LLVM_COMPILER)
#  endif

#elif defined(__clang__)
#  define UTL_COMPILER_CLANG 1
#  define UTL_COMPILER_CLANG_AT_LEAST(MAJOR, MINOR, PATCH) \
      __clang_major__ > MAJOR ||                           \
          (__clang_major__ == MAJOR &&                     \
              (__clang_minor__ > MINOR ||                  \
                  (__clang_minor__ == MINOR && __clang_patchlevel__ > PATCH)))

#  define UTL_COMPILER_TAG \
      UTL_CONCAT(CLANG,    \
          UTL_COMPILER_VERSION_CONCAT(__clang_major__, __clang_minor__, __clang_patchlevel__))
#elif defined(__INTEL_COMPILER)
#  define UTL_COMPILER_ICC 1
#  define UTL_COMPILER_ICC_AT_LEAST(VERSION) __VERSION__ >= VERSION
#  define UTL_COMPILER_TAG UTL_CONCAT(ICC, __VERSION__)
#elif defined(__GNUC__)
#  define UTL_COMPILER_GCC 1
#  define UTL_COMPILER_GCC_AT_LEAST(MAJOR, MINOR, PATCH) \
      __GNUC__ > MAJOR ||                                \
          (__GNUC__ == MAJOR &&                          \
              (__GNUC_MINOR__ > MINOR ||                 \
                  (__GNUC_MINOR__ == MINOR && __GNUC_PATCHLEVEL__ > PATCH)))
#  define UTL_COMPILER_TAG \
      UTL_CONCAT(GCC, UTL_COMPILER_VERSION_CONCAT(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__))
#elif defined(_MSC_VER)
#  define UTL_COMPILER_MSVC 1
#  define UTL_COMPILER_SUPPORTS_DECLSPEC 1
#  define UTL_COMPILER_MSVC_AT_LEAST(VERSION) _MSC_VER >= VERSION
#  define UTL_COMPILER_TAG UTL_CONCAT(MSVC, _MSC_VER)
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

#if __GNUC__
#  define UTL_COMPILER_GNU_COMPATIBLE 1
#endif

#if UTL_COMPILER_CLANG | UTL_COMPILER_GCC | UTL_COMPILER_ICC | UTL_COMPILER_ICX
#  define UTL_SUPPORTS_GNU_ASM 1
#endif
