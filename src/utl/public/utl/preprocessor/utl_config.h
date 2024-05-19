/* Copyright 2023-2024 Bryan Wong */

#pragma once

#ifndef UTL_DISABLE_INT128_SUPPORT
#  define UTL_DISABLE_INT128_SUPPORT 0
#endif

#ifdef __SIZEOF_INT128__
#  if !UTL_DISABLE_INT128_SUPPORT
#    define UTL_SUPPORTS_INT128 1
#  endif
#endif

#ifdef __STDCPP_FLOAT16_T__
#  define UTL_SUPPORTS_FLOAT16 1
#endif
#ifdef __STDCPP_FLOAT32_T__
#  define UTL_SUPPORTS_FLOAT32 1
#endif
#ifdef __STDCPP_FLOAT64_T__
#  define UTL_SUPPORTS_FLOAT64 1
#endif
#ifdef __STDCPP_FLOAT128_T__
#  define UTL_SUPPORTS_FLOAT128 1
#endif
#ifdef __STDCPP_BFLOAT16_T__
#  define UTL_SUPPORTS_BFLOAT16 1
#endif

#ifdef __cpp_char8_t
#  if __cpp_char8_t >= 201811L
#    define UTL_SUPPORTS_CHAR8_T 1
#  endif
#endif

#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_builtins.h"
#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_exceptions.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_pragma.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/preprocessor/utl_target.h"

#ifndef __cpp_aligned_new
#  define UTL_SUPPORTS_ALIGNED_ALLOCATION 0
#elif __cpp_aligned_new >= 201606L
#  define UTL_SUPPORTS_ALIGNED_ALLOCATION 1
#else
#  define UTL_SUPPORTS_ALIGNED_ALLOCATION 0
#endif

#ifndef __cpp_sized_deallocation
#  define UTL_SUPPORTS_SIZED_DEALLOCATION 0
#elif __cpp_sized_deallocation >= 201309L
#  define UTL_SUPPORTS_SIZED_DEALLOCATION 1
#else
#  define UTL_SUPPORTS_SIZED_DEALLOCATION 0
#endif

#ifdef __STDCPP_DEFAULT_NEW_ALIGNMENT__
#  define UTL_DEFAULT_NEW_ALIGNMENT __STDCPP_DEFAULT_NEW_ALIGNMENT__
#endif

#ifdef UTL_COMPILER_MSVC
#  define UTL_RESTRICT __restrict
#else
#  define UTL_RESTRICT __restrict__
#endif
