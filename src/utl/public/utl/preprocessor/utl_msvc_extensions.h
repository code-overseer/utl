/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_concatenation.h"
#include "utl/preprocessor/utl_is_empty.h"

#include "utl/configuration/utl_compiler.h"

#if UTL_COMPILER_MSVC
#  define UTL_HAS_MSVC_KEYWORD(NAME) UTL_IS_EMPTY(UTL_CONCAT(__UTL_MSVC_KEYWORD, NAME))
#  define UTL_HAS_SAL_ANNOTATION(NAME) UTL_IS_EMPTY(UTL_CONCAT(__UTL_SAL_ANNOTATION, NAME))

#  define __UTL_MSVC_KEYWORD__declspec
#  define __UTL_MSVC_KEYWORD__cdecl
#  define __UTL_MSVC_KEYWORD__fastcall
#  define __UTL_MSVC_KEYWORD__stdcall
#  define __UTL_MSVC_KEYWORD__restrict
#  define __UTL_MSVC_KEYWORD__vectorcall
#  define __UTL_MSVC_KEYWORD__unaligned
#  define __UTL_MSVC_KEYWORD__w64
#  define __UTL_MSVC_KEYWORD__forceinline

#  define __UTL_SAL_ANNOTATION_Check_return_
#else
#  define UTL_HAS_MSVC_KEYWORD(NAME) 0
#  define UTL_HAS_SAL_ANNOTATION(NAME) 0
#endif
