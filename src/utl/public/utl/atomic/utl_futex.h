// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN
namespace futex {

class __UTL_ABI_PUBLIC result {
public:
    __UTL_HIDE_FROM_ABI explicit constexpr result(int val) noexcept : value_(val) {}

    __UTL_HIDE_FROM_ABI constexpr bool interrupted() const noexcept;
    __UTL_HIDE_FROM_ABI constexpr bool timed_out() const noexcept;
    __UTL_HIDE_FROM_ABI constexpr bool failed() const noexcept;

    __UTL_HIDE_FROM_ABI constexpr int value() const noexcept { return value_; }

    __UTL_HIDE_FROM_ABI static UTL_CONSTEVAL result success() noexcept;

private:
    int value_;
};
} // namespace futex

UTL_NAMESPACE_END

#define UTL_FUTEX_PRIVATE_HEADER_GUARD
#if UTL_TARGET_LINUX
#  include "utl/atomic/linux/utl_futex.h"
#elif UTL_TARGET_APPLE
#  include "utl/atomic/apple/utl_futex.h"
#elif UTL_TARGET_MICROSOFT
#  include "utl/atomic/winapi/utl_futex.h"
#elif UTL_TARGET_BSD
#  include "utl/atomic/bsd/utl_futex.h"
// TODO: Non-native futex
#else
#  error Unsupported platform
#endif
#undef UTL_FUTEX_PRIVATE_HEADER_GUARD