/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_compiler.h"

#if defined(UTL_COMPILER_CLANG)

#  if defined(__CYGWIN__) || defined(_WIN32) || defined(_WIN64)
#    define UTL_TARGET_MICROSOFT
#  elif defined(__APPLE__)
#    define UTL_TARGET_APPLE
#  elif defined(RENAME_TO_SONY_IDENTIFIER) /* NDA */
#    define UTL_TARGET_SONY
#  elif defined(RENAME_TO_NINTENDO_IDENTIFIER) /* NDA */
#    define UTL_TARGET_NINTENDO
#  elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__) || \
      defined(__OpenBSD__)
#    define UTL_TARGET_BSD
#  elif defined(__linux__)
#    define UTL_TARGET_LINUX
#  elif defined(__unix__)
#    define UTL_TARGET_UNIX
#  else
#    error "Unrecognized target for Clang Compiler"
#  endif

#elif defined(UTL_COMPILER_GCC)

#  if defined(__CYGWIN__) || defined(_WIN32) || defined(_WIN64)
#    define UTL_TARGET_MICROSOFT
#  elif defined(__APPLE__)
#    define UTL_TARGET_APPLE
#  elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__) || \
      defined(__OpenBSD__)
#    define UTL_TARGET_BSD
#  elif defined(__linux__)
#    define UTL_TARGET_LINUX
#  elif defined(__unix__)
#    define UTL_TARGET_UNIX
#  else
#    error "Unrecognized target for GNU Compiler"
#  endif

#elif defined(UTL_COMPILER_MSVC)

#  define UTL_TARGET_MICROSOFT

#elif defined(UTL_COMPILER_INTEL)

#  if defined(__CYGWIN__) || defined(_WIN32) || defined(_WIN64)
#    define UTL_TARGET_MICROSOFT
#  elif defined(__APPLE__)
#    define UTL_TARGET_APPLE
#  elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__) || \
      defined(__OpenBSD__)
#    define UTL_TARGET_BSD
#  elif defined(__linux__)
#    define UTL_TARGET_LINUX
#  elif defined(__unix__)
#    define UTL_TARGET_UNIX
#  else
#    error "Unrecognized target for Intel Compiler"
#  endif

#endif /* ifdef UTL_COMPILER_CLANG */

#if defined(UTL_TARGET_APPLE) && !defined(UTL_TARGET_UNIX)
#  define UTL_TARGET_UNIX
#endif /* ifdef UTL_TARGET_APPLE && !defined(UTL_TARGET_UNIX) */

#if defined(UTL_TARGET_LINUX) && !defined(UTL_TARGET_UNIX)
#  define UTL_TARGET_UNIX
#endif /* ifdef UTL_TARGET_LINUX && !defined(UTL_TARGET_UNIX) */

#if defined(UTL_TARGET_SONY) && !defined(UTL_TARGET_BSD)
#  define UTL_TARGET_BSD
#  define UTL_TARGET_UNIX
#endif /* ifdef UTL_TARGET_SONY && !defined(UTL_TARGET_BSD) */

#if defined(UTL_TARGET_BSD) && !defined(UTL_TARGET_UNIX)
#  define UTL_TARGET_UNIX
#endif /* ifdef UTL_TARGET_BSD && !defined(UTL_TARGET_UNIX) */

#ifdef UTL_TARGET_SONY
#  if defined(RENAME_TO_PS4_IDENTIFIER) /* NDA */
#    define UTL_TARGET_SONY_PS4
#  elif defined(RENAME_TO_PS5_IDENTIFIER) /* NDA */
#    define UTL_TARGET_SONY_PS5
#  else
#    error "Unrecognized Sony target"
#  endif
#endif /* ifdef UTL_TARGET_SONY */

#ifdef UTL_TARGET_MICROSOFT

#  if defined(__CYGWIN__)
#    define UTL_TARGET_MICROSOFT_WINDOWS
#    define UTL_TARGET_MICROSOFT_CYGWIN
#  elif defined(_WIN32) || defined(_WIN64)
#    define UTL_TARGET_MICROSOFT_WINDOWS
#  elif defined(RENAME_TO_GDK_XBO_IDENTIFIER) /* NDA */
#    define UTL_TARGET_MICROSOFT_GDK_XBO
#  elif defined(RENAME_TO_GDK_XSX_IDENTIFIER) /* NDA */
#    define UTL_TARGET_MICROSOFT_GDK_XSX
#  else
#    error "Unrecognized Microsoft target"
#  endif

#  if defined(RENAME_TO_DESKTOP_GDK_IDENTIFIER) /* NDA */
#    define UTL_TARGET_MICROSOFT_GDK_WINDOWS
#  endif

#endif /* ifdef UTL_TARGET_MICROSOFT */
