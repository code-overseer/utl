// Copyright 2023-2024 Bryan Wong

#include "utl/utl_config.h"

#include <errno.h>
#include <stdlib.h>

#if defined(ELAST)
#  define __UTL_ELAST ELAST
#elif defined(__LLVM_LIBC__)
// No __UTL_ELAST needed for LLVM libc
#elif defined(_NEWLIB_VERSION)
#  define __UTL_ELAST __ELASTERROR
#elif defined(__NuttX__)
// No __UTL_ELAST needed on NuttX
#elif defined(__Fuchsia__)
// No __UTL_ELAST needed on Fuchsia
#elif defined(__wasi__)
// No __UTL_ELAST needed on WASI
#elif defined(__EMSCRIPTEN__)
// No __UTL_ELAST needed on Emscripten
#elif defined(__linux__)
#  define __UTL_ELAST 4095
#elif defined(__APPLE__)
// No __UTL_ELAST needed on Apple
#elif UTL_TARGET_MICROSOFT
#  define __UTL_ELAST (_sys_nerr - 1)
#elif defined(__MVS__)
#  define __UTL_ELAST 1160
#elif defined(_AIX)
#  define __UTL_ELAST 127
#else
#  warning ELAST for this platform not yet implemented
#endif
