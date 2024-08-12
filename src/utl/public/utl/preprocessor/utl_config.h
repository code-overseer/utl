/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/configuration/utl_abi.h"
#include "utl/configuration/utl_architecture.h"
#include "utl/configuration/utl_attributes.h"
#include "utl/configuration/utl_builtins.h"
#include "utl/configuration/utl_compiler.h"
#include "utl/configuration/utl_exceptions.h"
#include "utl/configuration/utl_keywords.h"
#include "utl/configuration/utl_namespace.h"
#include "utl/configuration/utl_pragma.h"
#include "utl/configuration/utl_simd.h"
#include "utl/configuration/utl_standard.h"
#include "utl/configuration/utl_target.h"
#include "utl/configuration/utl_types.h"

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

#if __OPTIMIZE__
#  define UTL_OPTIMIZATIONS_ENABLED 1
#endif
