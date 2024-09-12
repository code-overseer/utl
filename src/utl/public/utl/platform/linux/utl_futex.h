// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_FUTEX_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_TARGET_LINUX

#  include "utl/chrono/utl_chrono_fwd.h"

#  include "utl/platform/utl_time_duration.h"
#  include "utl/type_traits/utl_is_trivially_copyable.h"
#  include "utl/utility/utl_intcmp.h"

#  include <errno.h>
#  include <linux/futex.h>
#  include <stdint.h>
#  include <sys/syscall.h>
#  include <unistd.h>

UTL_NAMESPACE_BEGIN

namespace details {
namespace futex {

UTL_INLINE_CXX17 constexpr size_t max_size = 4;
UTL_INLINE_CXX17 constexpr size_t min_size = 4;
#  define __UTL_UNUSED 0

template <typename T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable_v<T>)
auto wait(T* address, T value, platform::time_duration t) noexcept -> UTL_ENABLE_IF_CXX11(
    int, sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable<T>::value) {
    static constexpr uint32_t op = FUTEX_WAIT_PRIVATE;

    timespec timeout{t};
    uint32_t readable_value = 0;
    __builtin_memcpy(&readable_value, value, sizeof(value));
    auto timeout_ptr = (t.seconds | t.nanoseconds) == 0 ? nullptr : &timeout;
    if (!syscall(SYS_futex, (uint32_t*)address, op, readable_value, timeout_ptr)) {
        return 0;
    }

    auto const error = errno;
    if (error == EAGAIN) {
        return 0;
    }

    return error;
}

template <typename T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable_v<T>)
auto notify_one(T* address) noexcept -> UTL_ENABLE_IF_CXX11(
    int, sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable<T>::value) {
    static constexpr uint32_t op = FUTEX_WAKE_PRIVATE;
    static constexpr uint64_t thread_count = 1;
    return syscall(SYS_futex, op, (uint32_t*)address, 1, __UTL_UNUSED);
}

template <typename T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable_v<T>)
auto notify_all(T* address) noexcept -> UTL_ENABLE_IF_CXX11(
    int, sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable<T>::value) {
    static constexpr uint32_t op = FUTEX_WAKE_PRIVATE;
    static constexpr uint64_t thread_count = 1;
    return syscall(SYS_futex, op, (uint32_t*)address, INT_MAX, __UTL_UNUSED);
}
} // namespace futex
} // namespace details

#  undef __UTL_UNUSED

UTL_NAMESPACE_END

namespace platform {
template <typename T>
class waitable_obect<T*> {
    static_assert(!UTL_TRAIT_is_function(T), "Invalid type");

public:
    using value_type = T*;
    constexpr explicit waitable_obect(value_type& t) : address(__UTL addressof(t)) {}
    waitable_obect(waitable_obect const&) = delete;
    waitable_obect(waitable_obect&&) = delete;
    waitable_obect& operator=(waitable_obect const&) = delete;
    waitable_obect& operator=(waitable_obect&&) = delete;
    UTL_CONSTEXPR_CXX20 ~waitable_obect() noexcept = default;

    template <typename R, typename P>
    void wait(value_type old, ::std::chrono::duration<R, P> timeout,
        __UTL memory_order o = __UTL memory_order_seq_cst) const noexcept {
        wait(old, time_duration{timeout}, o);
    }

    void wait(value_type old, __UTL memory_order o = __UTL memory_order_seq_cst) const noexcept {
        return wait(old, time_duration::invalid(), o);
    }

    void set_and_notify_one(value_type value, __UTL memory_order o) noexcept {
        __UTL atomic_ref<value_type>(address).store(value, o);
        notify_one();
    }

    void set_and_notify_all(value_type value, __UTL memory_order o) noexcept {
        __UTL atomic_ref<value_type>(address).store(value, o);
        notify_all();
    }

    void wait(value_type old, time_duration,
        __UTL memory_order o = __UTL memory_order_seq_cst) const noexcept;

    void notify_one() noexcept;

    void notify_all() noexcept;

private:
    value_type address;
    unsigned waiting;
};
} // namespace platform

#endif // UTL_TARGET_LINUX
