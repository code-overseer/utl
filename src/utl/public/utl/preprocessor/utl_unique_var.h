/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_concatenation.h"

#if UTL_COMPILER_CLANG | UTL_COMPILER_GCC | UTL_COMPILER_MSVC | UTL_COMPILER_INTEL

#  define UTL_UNIQUE_VAR(var) UTL_CONCAT(var, __COUNTER__)

#else

/* On unrecognized platforms, use __LINE__ */
#  define UTL_UNIQUE_VAR(var) UTL_CONCAT(var, __LINE__)

#endif /* if UTL_COMPILER_CLANG */
