/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_compiler.h"

#ifndef _LIBCPP_HAS_NO_EXCEPTIONS
#  if defined(__cpp_exceptions)
#    define UTL_WITH_EXCEPTIONS 1
#  elif UTL_COMPILER_MSVC && defined(_CPPUNWIND)
#    if _CPPUNWIND
#      define UTL_WITH_EXCEPTIONS 1
#    endif
#  elif defined(__EXCEPTIONS)
#    define UTL_WITH_EXCEPTIONS 1
#  endif
#endif
