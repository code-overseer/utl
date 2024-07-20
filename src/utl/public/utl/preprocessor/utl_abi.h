/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_attribute_check.h"
#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_concatenation.h"
#include "utl/preprocessor/utl_exceptions.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/preprocessor/utl_target.h"
#include "utl/preprocessor/utl_to_string.h"

#if UTL_TARGET_MICROSOFT
#  ifdef UTL_BUILDING_LIBRARY
#    if UTL_COMPILER_GNU_COMPATIBLE
#      define UTL_ABI_PUBLIC __attribute__((dllexport))
#    else
#      define UTL_ABI_PUBLIC \
          __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#    endif
#  else
#    if UTL_COMPILER_GNU_COMPATIBLE
#      define UTL_ABI_PUBLIC __attribute__((dllimport))
#    else
#      define UTL_ABI_PUBLIC \
          __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#    endif
#  endif
#  define UTL_ABI_PRIVATE
#else
#  define UTL_ABI_PUBLIC __attribute__((visibility("default")))
#  define UTL_ABI_PRIVATE __attribute__((visibility("hidden")))
#endif

#if UTL_WITH_EXCEPTIONS
#  define UTL_ABI_EXCEPTION_TAG e
#else
#  define UTL_ABI_EXCEPTION_TAG n
#endif

#ifndef UTL_COMPILER_TAG
#  error Undefined compiler tag
#endif

#ifndef UTL_ABI_EXCEPTION_TAG
#  error Undefined exception tag
#endif

/* TODO WTF? Do this properly! */
#define UTL_HARDENING_MODE n

#define UTL_ODR_SIGNATURE          \
    UTL_CONCAT(UTL_HARDENING_MODE, \
        UTL_CONCAT(UTL_CONCAT(UTL_COMPILER_TAG, UTL_ABI_EXCEPTION_TAG), UTL_CXX))

#if __has_attribute(exclude_from_explicit_instantiation)
#  define UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION \
      __attribute__((__exclude_from_explicit_instantiation__))

#else
#  define UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#endif

#if UTL_HAS_ATTRIBUTE(__type_visibility__)
#  define UTL_PUBLIC_TYPE __attribute__((__type_visibility__("default")))
#  define UTL_PUBLIC_TEMPLATE
#else
#  define UTL_PUBLIC_TYPE
/* For GNU compailers that don't have type visibility we must keep the templates visible */
#  define UTL_PUBLIC_TEMPLATE __attribute__((__visibility__("default")))
#endif

#if UTL_HAS_ATTRIBUTE(__abi_tag__)
#  define UTL_HIDE_FROM_ABI                                   \
      UTL_ABI_PRIVATE UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION \
          __attribute__((__abi_tag__(UTL_TO_STRING(UTL_ODR_SIGNATURE))))
#else
/* What about Windows? */
#  define UTL_HIDE_FROM_ABI UTL_ABI_PRIVATE UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#endif

/* virtual functions must be linked to the same symbol */
#define UTL_HIDE_FROM_ABI_VIRTUAL UTL_ABI_PRIVATE UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
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
