/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_paste.h"

#if UTL_COMPILER_MSVC
#  define UTL_HAS_MSVC_MODIFIER(NAME) UTL_IS_EMPTY(UTL_CONCAT(UTL_MSVC_MODIFIER, NAME))
#  define UTL_HAS_MSVC_DECORATOR(NAME) UTL_IS_EMPTY(UTL_CONCAT(UTL_MSVC_DECORATOR, NAME))
#  define UTL_HAS_SAL_ANNOTATION(NAME) UTL_IS_EMPTY(UTL_CONCAT(UTL_SAL_ANNOTATION, NAME))

#  define UTL_MSVC_MODIFIER__cdecl
#  define UTL_MSVC_MODIFIER__fastcall
#  define UTL_MSVC_MODIFIER__stdcall
#  define UTL_MSVC_MODIFIER__stdcall
#  define UTL_MSVC_MODIFIER__restrict
#  define UTL_MSVC_MODIFIER__forceinline
#  define UTL_MSVC_MODIFIER__vectorcall
#  define UTL_MSVC_MODIFIER__unaligned
#  define UTL_MSVC_MODIFIER__w64

#  define UTL_MSVC_DECORATOR__forceline
#  define UTL_SAL_ANNOTATION_Check_return_

#endif
