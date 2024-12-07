// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN
namespace futex {
/**
 *
 * Waits for the value at the given pointer `type*` to differ from the expected value `type const&`.
 * If the current value at `ptr` equals the `expected` value, the thread blocks until it is
 * notified, the timeout expires, or the thread is interrupted. If the value at `ptr` is already
 * different from `expected`, it returns success immediately.
 *
 * @tparam type The type of the value being waited on.
 *
 * @param ptr Pointer to the value that the futex operates on.
 * @param expected The expected value of type `type`. The calling thread will wait only if the value
 * at `ptr` equals `expected`.
 * @param timeout An optional timeout of type `utl::tempus::duration`. Pass an invalid duration to
 * wait indefinitely.
 *
 * @return A `error_code` object representing the outcome of the wait. Possible outcomes are success
 * (when the values differ), timeout, or interruption by a signal.
 *
 * error_code wait(type*, type const&, utl::tempus::duration) noexcept;
 *
 *
 * Wakes one thread waiting on the value at the pointer `type*`.
 *
 * @tparam type The type of the value being notified.
 * @param ptr Pointer to the value that the futex operates on.
 *
 * @return An integer representing the number of threads that were notified (0 or 1)
 *
 * int notify_one(type*) noexcept;
 *
 *
 * Wakes all threads waiting on the value at the pointer `type*`.
 *
 * @tparam type The type of the value being notified.
 * @param ptr Pointer to the value that the futex operates on.
 *
 * @return An integer representing the number of threads that were notified (0 or 1)
 *
 * int notify_all(type*) noexcept;
 */
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
