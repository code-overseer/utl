/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_concatenation.h"

#if defined(UTL_COMPILER_CLANG) | defined(UTL_COMPILER_GCC) | defined(UTL_COMPILER_MSVC) | \
    defined(UTL_COMPILER_INTEL)

#  define UTL_UNIQUE_VAR(var) UTL_CONCAT(var, __COUNTER__)

#else

/* On unrecognized platforms, use __LINE__ */
#  define UTL_UNIQUE_VAR(var) UTL_CONCAT(var, __LINE__)

#endif /* ifdef UTL_COMPILER_CLANG */
