/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_msvc_extensions.h"

#if UTL_COMPILER_MSVC
#  define UTL_CDECL __cdecl
#  define UTL_FASTCALL __fastcall
#  define UTL_STDCALL __stdcall
#  define UTL_RESTRICT __restrict
#  define UTL_UNALIGNED __unaligned
#  define UTL_VECTORCALL __vectorcall
#  define UTL_W64 __w64
#  define UTL_FORCEINLINE __forceinline
#else
#  define UTL_CDECL
#  define UTL_FASTCALL
#  define UTL_STDCALL
#  define UTL_RESTRICT __restrict__
#  define UTL_UNALIGNED
#  define UTL_VECTORCALL
#  define UTL_W64
#  define UTL_FORCEINLINE
#endif
