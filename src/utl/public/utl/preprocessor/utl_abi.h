/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_concatenation.h"
#include "utl/preprocessor/utl_exceptions.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/preprocessor/utl_target.h"
#include "utl/preprocessor/utl_to_string.h"

#if UTL_TARGET_MICROSOFT
#  ifdef UTL_BUILDING_LIBRARY
#    define UTL_ABI_PUBLIC UTL_ATTRIBUTE(DLLEXPORT)
#    define __UTL_ATTRIBUTE_ABI_PUBLIC (DLLEXPORT)
#    define __UTL_ATTRIBUTE_TYPE_AGGREGATE_ABI_PUBLIC
#  else
#    define UTL_ABI_PUBLIC UTL_ATTRIBUTE(DLLIMPORT)
#    define __UTL_ATTRIBUTE_ABI_PUBLIC (DLLIMPORT)
#    define __UTL_ATTRIBUTE_TYPE_AGGREGATE_ABI_PUBLIC
#  endif
#else
#  define UTL_ABI_PUBLIC UTL_ATTRIBUTE(VISIBILITY("default"))
#  define UTL_ABI_PRIVATE UTL_ATTRIBUTE(VISIBILITY("hidden"))
#  define __UTL_ATTRIBUTE_ABI_PUBLIC (VISIBILITY("default"))
#  define __UTL_ATTRIBUTE_TYPE_AGGREGATE_ABI_PUBLIC
#  define __UTL_ATTRIBUTE_ABI_PRIVATE (VISIBILITY("hidden"))
#  define __UTL_ATTRIBUTE_TYPE_AGGREGATE_ABI_PRIVATE
#endif

#if !UTL_HAS_ATTRIBUTE(TYPE_VISIBILITY)
/* For GNU compilers that don't have type visibility we must keep the templates visible */
#  define UTL_PUBLIC_TEMPLATE UTL_ATTRIBUTE(VISIBILITY("default"))
#  define __UTL_ATTRIBUTE_PUBLIC_TEMPLATE (VISIBILITY("default"))
#  define __UTL_ATTRIBUTE_TYPE_AGGREGATE_PUBLIC_TEMPLATE
#endif

#define __UTL_ODR_SIGNATURE_1(_0, _1, _2, _3, _4) _0##_1##_2##_3##_4
#define __UTL_ODR_SIGNATURE_0(_0, _1, _2, _3, _4) __UTL_ODR_SIGNATURE_1(_0, _1, _2, _3, _4)
#define __UTL_ODR_SIGNATURE              \
    UTL_TO_STRING(__UTL_ODR_SIGNATURE_0( \
        CXX, UTL_CXX, __UTL_HARDENING_MODE, __UTL_ABI_EXCEPTION_TAG, UTL_COMPILER_TAG))

#define UTL_HIDE_FROM_ABI UTL_ATTRIBUTE(HIDE_FROM_ABI)
#define __UTL_ATTRIBUTE_HIDE_FROM_ABI \
    (VISIBILITY("hidden"))(EXCLUDE_FROM_EXPLICIT_INSTANTIATION)(ABI_TAG(__UTL_ODR_SIGNATURE))
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_HIDE_FROM_ABI

/* virtual functions must be linked to the same symbol */
#define UTL_HIDE_FROM_ABI_VIRTUAL UTL_ATTRIBUTE(HIDE_FROM_ABI_VIRTUAL)
#define __UTL_ATTRIBUTE_HIDE_FROM_ABI_VIRTUAL \
    (VISIBILITY("hidden"))(EXCLUDE_FROM_EXPLICIT_INSTANTIATION)
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_HIDE_FROM_ABI_VIRTUAL

#if UTL_WITH_EXCEPTIONS
#  define __UTL_ABI_EXCEPTION_TAG e
#else
#  define __UTL_ABI_EXCEPTION_TAG n
#endif

#ifndef UTL_COMPILER_TAG
#  error Undefined compiler tag
#endif

#ifndef __UTL_ABI_EXCEPTION_TAG
#  error Undefined exception tag
#endif

/* TODO WTF? Do this properly! */
#define __UTL_HARDENING_MODE n

#define UTL_EXTERN_C extern "C"
#define UTL_EXTERN_C_BEGIN UTL_EXTERN_C {
#define UTL_EXTERN_C_END }

#if UTL_COMPILER_MSVC && UTL_CXX20
#  define UTL_EXTERN_CXX extern "C++"
#  define UTL_EXTERN_CXX_BEGIN UTL_EXTERN_CXX {
#  define UTL_EXTERN_CXX_END }
#else
#  define UTL_EXTERN_CXX
#  define UTL_EXTERN_CXX_BEGIN
#  define UTL_EXTERN_CXX_END
#endif
