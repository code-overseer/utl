/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/configuration/utl_attributes.h"
#include "utl/configuration/utl_compiler.h"
#include "utl/configuration/utl_exceptions.h"
#include "utl/configuration/utl_standard.h"
#include "utl/configuration/utl_target.h"

#include "utl/preprocessor/utl_concatenation.h"
#include "utl/preprocessor/utl_to_string.h"

#if UTL_COMPILER_MSVC

#  if !UTL_HAS_DECLSPEC(dllexport) || !UTL_HAS_DECLSPEC(dllimport)
#    error "Unrecognized compiler"
#  endif

#  ifdef UTL_BUILDING_LIBRARY
#    define __UTL_ATTRIBUTE__ABI_PUBLIC (DLLEXPORT)
#    define __UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PUBLIC
#    define __UTL_ABI_PUBLIC __declspec(dllexport)
#  else
#    define __UTL_ATTRIBUTE__ABI_PUBLIC (DLLIMPORT)
#    define __UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PUBLIC
#    define __UTL_ABI_PUBLIC __declspec(dllimport)
#  endif
#  define __UTL_ABI_PRIVATE
#  define __UTL_ATTRIBUTE__ABI_PRIVATE
#  ifdef __UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PRIVATE
#    error '__UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PRIVATE' cannot be defined
#  endif
#  define __UTL_PUBLIC_TEMPLATE_DATA
#else

#  if !UTL_HAS_GNU_ATTRIBUTE(__visibility__) || !UTL_HAS_GNU_ATTRIBUTE(__visibility__)
#    error "Unrecognized compiler"
#  endif

#  define __UTL_ATTRIBUTE__ABI_PUBLIC (VISIBILITY("default"))
#  define __UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PUBLIC
#  define __UTL_ATTRIBUTE__ABI_PRIVATE (VISIBILITY("hidden"))
#  define __UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PRIVATE

#  define __UTL_ABI_PUBLIC __attribute__((__visibility__("default")))
#  define __UTL_ABI_PRIVATE __attribute__((__visibility__("hidden")))
#  define __UTL_PUBLIC_TEMPLATE_DATA __attribute__((__visibility__("default")))

#endif

#if !UTL_HAS_GNU_ATTRIBUTE(TYPE_VISIBILITY)
/* For GNU compilers that don't have type visibility we must keep the templates visible */
#  define __UTL_PUBLIC_TEMPLATE __attribute__((__visibility__("default")))
#  define __UTL_ATTRIBUTE__PUBLIC_TEMPLATE (VISIBILITY("default"))
#  define __UTL_ATTRIBUTE_TYPE_AGGREGATE__PUBLIC_TEMPLATE
#  define __UTL_NAMESPACE_VISIBILITY
#else
#  define __UTL_NAMESPACE_VISIBILITY __attribute__((__type_visibility__("default")))
#  define __UTL_PUBLIC_TEMPLATE
#  define __UTL_ATTRIBUTE__PUBLIC_TEMPLATE
#  ifdef __UTL_ATTRIBUTE_TYPE_AGGREGATE__PUBLIC_TEMPLATE
#    error '__UTL_ATTRIBUTE_TYPE_AGGREGATE__PUBLIC_TEMPLATE' cannot be defined
#  endif
#endif

#define __UTL_ODR_SIGNATURE_1(_0, _1, _2, _3, _4) _0##_1##_2##_3##_4
#define __UTL_ODR_SIGNATURE_0(_0, _1, _2, _3, _4) __UTL_ODR_SIGNATURE_1(_0, _1, _2, _3, _4)
#define __UTL_ODR_SIGNATURE              \
    UTL_TO_STRING(__UTL_ODR_SIGNATURE_0( \
        CXX, UTL_CXX, __UTL_HARDENING_MODE, __UTL_ABI_EXCEPTION_TAG, UTL_COMPILER_TAG))

#if UTL_HAS_GNU_ATTRIBUTE(__exclude_from_explicit_instantiation__)
#  define __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION \
      __attribute__((__exclude_from_explicit_instantiation__))
#else
#  define __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#endif

#if UTL_HAS_GNU_ATTRIBUTE(__abi_tag__)
#  define __UTL_ABI_TAG(TAG) __attribute__((__abi_tag__(TAG)))
#else
#  define __UTL_ABI_TAG(TAG)
#endif

#define __UTL_HIDE_FROM_ABI \
    __UTL_ABI_PRIVATE __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION __UTL_ABI_TAG(__UTL_ODR_SIGNATURE)
#define __UTL_ATTRIBUTE__HIDE_FROM_ABI \
    __UTL_ATTRIBUTE__ABI_PRIVATE(EXCLUDE_FROM_EXPLICIT_INSTANTIATION)(ABI_TAG(__UTL_ODR_SIGNATURE))
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE__HIDE_FROM_ABI

/* virtual functions must be linked to the same symbol */
#define __UTL_HIDE_FROM_ABI_VIRTUAL __UTL_ABI_PRIVATE __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#define __UTL_ATTRIBUTE__HIDE_FROM_ABI_VIRTUAL \
    __UTL_ATTRIBUTE__ABI_PRIVATE(EXCLUDE_FROM_EXPLICIT_INSTANTIATION)
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE__HIDE_FROM_ABI_VIRTUAL
#define __UTL_HIDE_FROM_ABI_UNTAGGED __UTL_ABI_PRIVATE __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#define __UTL_ATTRIBUTE__HIDE_FROM_ABI_UNTAGGED \
    __UTL_ATTRIBUTE__ABI_PRIVATE(EXCLUDE_FROM_EXPLICIT_INSTANTIATION)
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE__HIDE_FROM_ABI_UNTAGGED

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

/* TODO: Use debug/opt level */
#define __UTL_HARDENING_MODE n

#if UTL_CXX
#  define UTL_EXTERN_C extern "C"
#  define UTL_EXTERN_C_BEGIN UTL_EXTERN_C {
#  define UTL_EXTERN_C_END }
#else
#  define UTL_EXTERN_C
#  define UTL_EXTERN_C_BEGIN
#  define UTL_EXTERN_C_END
#endif

#if UTL_COMPILER_MSVC && UTL_CXX20
#  define UTL_EXTERN_CXX extern "C++"
#  define UTL_EXTERN_CXX_BEGIN UTL_EXTERN_CXX {
#  define UTL_EXTERN_CXX_END }
#else
#  define UTL_EXTERN_CXX
#  define UTL_EXTERN_CXX_BEGIN
#  define UTL_EXTERN_CXX_END
#endif
