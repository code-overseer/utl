/* Copyright 2023-2024 Bryan Wong */

#pragma once

#ifdef __SIZEOF_INT128__
#define UTL_SUPPORTS_INT128
#endif

#ifdef __STDCPP_FLOAT16_T__
  #define UTL_SUPPORTS_FLOAT16
#endif
#ifdef __STDCPP_FLOAT32_T__
  #define UTL_SUPPORTS_FLOAT32
#endif
#ifdef __STDCPP_FLOAT64_T__
  #define UTL_SUPPORTS_FLOAT64
#endif
#ifdef __STDCPP_FLOAT128_T__
  #define UTL_SUPPORTS_FLOAT128
#endif
#ifdef __STDCPP_BFLOAT16_T__
  #define UTL_SUPPORTS_BFLOAT16
#endif

#include "utl/preprocessor/utl_builtins.h"
#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_pragma.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/preprocessor/utl_target.h"
