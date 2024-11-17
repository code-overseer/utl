/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/utl_config.h"

#define UTL_ASSERTIONS_ENABLED 0
#define UTL_ASSERT(...) \
    static_assert(      \
        static_cast<decltype(static_cast<bool>(__VA_ARGS__))*>(0) == nullptr, "syntax check")
#define UTL_ASSERT_MSG(X, ...) \
    static_assert(static_cast<decltype(static_cast<bool>(X))*>(0) == nullptr, "syntax check")

#if UTL_CXX14
#  define UTL_ASSERT_CXX14(...) UTL_ASSERT(__VA_ARGS__)
#  define UTL_ASSERT_MSG_CXX14(...) UTL_ASSERT_MSG(__VA_ARGS__)
#else
#  define UTL_ASSERT_CXX14(...) \
      static_assert(            \
          static_cast<decltype(static_cast<bool>(__VA_ARGS__))*>(0) == nullptr, "syntax check")
#  define UTL_ASSERT_MSG_CXX14(X, ...) \
      static_assert(static_cast<decltype(static_cast<bool>(X))*>(0) == nullptr, "syntax check")
#endif
