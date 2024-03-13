/* Copyright 2023-2024 Bryan Wong */

#pragma once

#ifndef UTL_DISABLE_INT128_SUPPORT
#  define UTL_DISABLE_INT128_SUPPORT 0
#endif

#ifdef __SIZEOF_INT128__
#  if !UTL_DISABLE_INT128_SUPPORT
#    define UTL_SUPPORTS_INT128
#  endif
#endif

#ifdef __STDCPP_FLOAT16_T__
#  define UTL_SUPPORTS_FLOAT16
#endif
#ifdef __STDCPP_FLOAT32_T__
#  define UTL_SUPPORTS_FLOAT32
#endif
#ifdef __STDCPP_FLOAT64_T__
#  define UTL_SUPPORTS_FLOAT64
#endif
#ifdef __STDCPP_FLOAT128_T__
#  define UTL_SUPPORTS_FLOAT128
#endif
#ifdef __STDCPP_BFLOAT16_T__
#  define UTL_SUPPORTS_BFLOAT16
#endif

#include "utl/preprocessor/utl_builtins.h"
#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_exceptions.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_pragma.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/preprocessor/utl_target.h"
