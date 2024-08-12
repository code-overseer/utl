/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/configuration/utl_abi.h"

#ifndef UTL_NS
#  define UTL_NS utl
#endif

#define UTL_SCOPE ::UTL_NS::

#ifdef UTL_NAMESPACE_BEGIN
#  error "Cannot define UTL_NAMESPACE_BEGIN"
#endif
#ifdef UTL_NAMESPACE_END
#  error "Cannot define UTL_NAMESPACE_END"
#endif

/* extern C++ for MSVC */
#define UTL_NAMESPACE_BEGIN \
    UTL_EXTERN_CXX_BEGIN    \
    namespace __UTL_NAMESPACE_VISIBILITY UTL_NS {

#ifndef UTL_NAMESPACE_END
#  define UTL_NAMESPACE_END \
      }                     \
      UTL_EXTERN_CXX_END
#endif

#include <stddef.h>

#ifdef _LIBCPP_ABI_NAMESPACE
#  define UTL_STD_ABI_NAMESPACE_BEGIN inline namespace _LIBCPP_ABI_NAMESPACE {
#  define UTL_STD_ABI_NAMESPACE_END }
#elif defined(_GLIBCXX_BEGIN_NAMESPACE_VERSION)
#  define UTL_STD_ABI_NAMESPACE_BEGIN _GLIBCXX_BEGIN_NAMESPACE_VERSION
#  define UTL_STD_ABI_NAMESPACE_END _GLIBCXX_END_NAMESPACE_VERSION
#endif

#ifndef UTL_STD_ABI_NAMESPACE_BEGIN
#  define UTL_STD_ABI_NAMESPACE_BEGIN
#  define UTL_STD_ABI_NAMESPACE_END
#endif

/* extern C++ for MSVC > C++20, no effect anywhere else */
#define UTL_STD_NAMESPACE_BEGIN                                                    \
    UTL_EXTERN_CXX_BEGIN namespace UTL_ATTRIBUTE(TYPE_VISIBILITY("default")) std { \
        UTL_STD_ABI_NAMESPACE_BEGIN

#define UTL_STD_NAMESPACE_END \
    UTL_STD_ABI_NAMESPACE_END \
    }                         \
    UTL_EXTERN_CXX_END
