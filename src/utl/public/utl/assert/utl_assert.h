/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/utl_config.h"

#define UTL_ASSERTIONS_ENABLED 0
UTL_NAMESPACE_BEGIN
namespace details {
namespace assertions {
template <typename F>
__UTL_HIDE_FROM_ABI constexpr int null_op(F&&) noexcept {
    return 0;
}
} // namespace assertions
} // namespace details
UTL_NAMESPACE_END
#define UTL_ASSERT(...)                                                                            \
    __UTL details::assertions::null_op([&]() {                                                     \
        static_assert(                                                                             \
            static_cast<decltype(static_cast<bool>(__VA_ARGS__))*>(0) == nullptr, "syntax check"); \
    })
#define UTL_ASSERT_MSG(X, ...) UTL_ASSERT(X)

#if UTL_CXX14
#  define UTL_ASSERT_CXX14(...) UTL_ASSERT(__VA_ARGS__)
#  define UTL_ASSERT_MSG_CXX14(...) UTL_ASSERT_MSG(__VA_ARGS__)
#else
#  define UTL_ASSERT_CXX14(...)                                                                    \
      __UTL details::assertions::null_op([&]() {                                                   \
        static_assert(                                                                             \
            static_cast<decltype(static_cast<bool>(__VA_ARGS__))*>(0) == nullptr, "syntax check"); \
      })
#  define UTL_ASSERT_MSG_CXX14(X, ...) UTL_ASSERT_CXX14(X)
#endif
