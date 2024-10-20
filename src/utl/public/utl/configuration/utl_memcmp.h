/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/configuration/utl_compiler.h"
#include "utl/configuration/utl_keywords.h"
#include "utl/configuration/utl_standard.h"

#if UTL_HAS_BUILTIN(__builtin_memcmp)

#  define __UTL_MEMCMP __builtin_memcmp

#else

#  if UTL_CXX
extern "C" int memcmp(void const*, void const*, decltype(sizeof(0)));
#  else
int memcmp(void const*, void const*, size_t);
#  endif

#  if UTL_COMPILER_MSVC
#    pragma intrinsic(memcmp)
#  endif
#  define __UTL_MEMCMP ::memcmp

#endif
