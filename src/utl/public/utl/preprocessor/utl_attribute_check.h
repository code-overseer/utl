/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_declspec.h"

#ifdef __has_cpp_attribute
#  define UTL_HAS_CPP_ATTRIBUTE(NAME) __has_cpp_attribute(NAME)
#else /* ifdef __has_cpp_attribute */
#  define UTL_HAS_CPP_ATTRIBUTE(NAME) 0
#endif /* ifdef __has_cpp_attribute */

#ifdef __has_attribute
#  define UTL_HAS_GNU_ATTRIBUTE(NAME) __has_attribute(NAME)
#else /* ifdef __has_attribute */
#  define UTL_HAS_GNU_ATTRIBUTE(NAME) 0
#endif /* ifdef __has_attribute */
