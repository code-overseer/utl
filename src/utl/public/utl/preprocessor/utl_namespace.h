/* Copyright 2023-2024 Bryan Wong */

#pragma once

#ifndef UTL_NS
#  define UTL_NS utl
#endif

#define UTL_SCOPE ::UTL_NS::

#ifdef UTL_NAMESPACE_BEGIN
#  error "Cannot define UTL_NAMESPACE_BEGIN"
#endif

#define UTL_NAMESPACE_BEGIN namespace UTL_NS {

#ifndef UTL_NAMESPACE_END
#  define UTL_NAMESPACE_END }
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

#define UTL_STD_NAMESPACE_BEGIN \
    namespace std {             \
    UTL_STD_ABI_NAMESPACE_BEGIN
#define UTL_STD_NAMESPACE_END \
    UTL_STD_ABI_NAMESPACE_END \
    }
