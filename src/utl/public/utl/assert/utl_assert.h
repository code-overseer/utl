/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/utl_config.h"

#define UTL_ASSERTIONS_ENABLED 0
#define UTL_ASSERT(...)
#define UTL_ASSERT_MSG(...)

#if UTL_CXX14
#  define UTL_ASSERT_CXX14(...) UTL_ASSERT(__VA_ARGS__)
#  define UTL_ASSERT_MSG_CXX14(...) UTL_ASSERT_MSG(__VA_ARGS__)
#else
#  define UTL_ASSERT_CXX14(...)
#  define UTL_ASSERT_MSG_CXX14(...)
#endif
