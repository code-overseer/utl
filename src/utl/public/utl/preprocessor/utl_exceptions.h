/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_compiler.h"

#if defined(__cpp_exceptions)
#  define UTL_WITH_EXCEPTIONS
#elif defined(UTL_COMPILER_MSVC) && defined(_CPPUNWIND)
#  if _CPPUNWIND
#    define UTL_WITH_EXCEPTIONS
#  endif
#elif defined(__EXCEPTIONS)
#  define UTL_WITH_EXCEPTIONS
#endif
