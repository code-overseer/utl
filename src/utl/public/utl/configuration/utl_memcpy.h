/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/configuration/utl_compiler.h"
#include "utl/configuration/utl_keywords.h"
#include "utl/configuration/utl_standard.h"

#if UTL_HAS_BUILTIN(__builtin_memcpy_inline)
#  define __UTL_MEMCPY __builtin_memcpy_inline

#elif UTL_HAS_BUILTIN(__builtin_memcpy)

#  define __UTL_MEMCPY __builtin_memcpy

#else

#  if UTL_CXX
extern "C" void* memcpy(void* UTL_RESTRICT, void const* UTL_RESTRICT, decltype(sizeof(0)));
#  else
void* memcpy(void* UTL_RESTRICT, void const* UTL_RESTRICT, size_t);
#  endif

#  if UTL_COMPILER_MSVC
#    pragma intrinsic(memcpy)
#  endif
#  define __UTL_MEMCPY ::memcpy

#endif
