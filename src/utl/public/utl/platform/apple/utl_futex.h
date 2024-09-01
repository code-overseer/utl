// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_FUTEX_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if UTL_TARGET_APPLE

#  include "utl/chrono/utl_chrono_fwd.h"

#  include "utl/numeric/utl_add_sat.h"
#  include "utl/utility/utl_intcmp.h"

#  include <errno.h>
#  include <stdint.h>

UTL_EXTERN_C_BEGIN
int __ulock_wait(uint32_t operation, void* addr, uint64_t value, uint32_t timeout);
int __ulock_wake(uint32_t operation, void* addr, uint64_t thread_id);
UTL_EXTERN_C_END

UTL_NAMESPACE_BEGIN

#  define UL_COMPARE_AND_WAIT 1
#  define UL_UNFAIR_LOCK 2
#  define UL_COMPARE_AND_WAIT_SHARED 3
#  define UL_UNFAIR_LOCK64_SHARED 4
#  define UL_COMPARE_AND_WAIT64 5
#  define UL_COMPARE_AND_WAIT64_SHARED 6

#  define ULF_WAKE_ALL 0x00000100
#  define ULF_WAKE_THREAD 0x00000200
#  define ULF_WAKE_ALLOW_NON_OWNER 0x00000400

#  define ULF_WAIT_WORKQ_DATA_CONTENTION 0x00010000
#  define ULF_WAIT_CANCEL_POINT 0x00020000
#  define ULF_WAIT_ADAPTIVE_SPIN 0x00040000

#  define __UTL_UNUSED 0

namespace details {
namespace futex {

UTL_INLINE_CXX17 constexpr size_t max_size = 8;
UTL_INLINE_CXX17 constexpr size_t min_size = 4;

template <typename T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable_v<T>)
auto wait(T* address, T value, platform::time_duration t) noexcept -> UTL_ENABLE_IF_CXX11(
    int, sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable<T>::value) {
    static constexpr uint32_t type = UL_COMPARE_AND_WAIT | UL_UNFAIR_LOCK;
    static constexpr uint32_t op = type | ULF_WAIT_WORKQ_DATA_CONTENTION;
    auto micro = t.nanoseconds * 1000;
    micro = __UTL add_sat(t.seconds / 1000000, micro);
    if (!__UTL in_range<uint32_t>(micro)) UTL_UNLIKELY {
        return EINVAL;
    }

    uint64_t readable_value = 0;
    __builtin_memcpy(&readable_value, value, sizeof(value));
    return __ulock_wait(op, address, readable_value, (uint32_t)microseconds);
}

template <typename T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable_v<T>)
auto notify_one(T* address) noexcept -> UTL_ENABLE_IF_CXX11(
    int, sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable<T>::value) {
    static constexpr uint32_t op = UL_COMPARE_AND_WAIT | UL_UNFAIR_LOCK;
    return __ulock_wake(op, address, __UTL_UNUSED);
}

template <typename T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable_v<T>)
auto notify_all(T* address) noexcept -> UTL_ENABLE_IF_CXX11(
    int, sizeof(T) == 4 && alignof(T) == 4 && is_trivially_copyable<T>::value) {
    static constexpr uint32_t type = UL_COMPARE_AND_WAIT | UL_UNFAIR_LOCK;
    static constexpr uint32_t op = type | ULF_WAKE_ALL;
    return __ulock_wake(op, address, __UTL_UNUSED);
}

template <typename T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 8 && alignof(T) == 8 && is_trivially_copyable_v<T>)
auto wait(T* address, T value, platform::time_duration t) noexcept -> UTL_ENABLE_IF_CXX11(
    int, sizeof(T) == 8 && alignof(T) == 8 && is_trivially_copyable<T>::value) {
    static constexpr uint32_t type = UL_COMPARE_AND_WAIT | UL_UNFAIR_LOCK;
    static constexpr uint32_t op = type | ULF_WAIT_WORKQ_DATA_CONTENTION;
    auto micro = t.nanoseconds * 1000;
    micro = __UTL add_sat(t.seconds / 1000000, micro);
    if (!__UTL in_range<uint32_t>(micro)) UTL_UNLIKELY {
        return EINVAL;
    }

    uint64_t readable_value = 0;
    __builtin_memcpy(&readable_value, value, sizeof(value));
    return __ulock_wait(op, address, readable_value, (uint32_t)microseconds);
}

template <typename T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 8 && alignof(T) == 8 && is_trivially_copyable_v<T>)
auto notify_one(T* address) noexcept -> UTL_ENABLE_IF_CXX11(
    int, sizeof(T) == 8 && alignof(T) == 8 && is_trivially_copyable<T>::value) {
    static constexpr uint32_t op = UL_COMPARE_AND_WAIT64 | UL_UNFAIR_LOCK;
    return __ulock_wake(op, address, __UTL_UNUSED);
}

template <typename T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 8 && alignof(T) == 8 && is_trivially_copyable_v<T>)
auto notify_all(T* address) noexcept -> UTL_ENABLE_IF_CXX11(
    int, sizeof(T) == 8 && alignof(T) == 8 && is_trivially_copyable<T>::value) {
    static constexpr uint32_t type = UL_COMPARE_AND_WAIT64 | UL_UNFAIR_LOCK;
    static constexpr uint32_t op = type | ULF_WAKE_ALL;
    return __ulock_wake(op, address, __UTL_UNUSED);
}

} // namespace futex
} // namespace details

#  undef __UTL_UNUSED

#  undef UL_COMPARE_AND_WAIT
#  undef UL_UNFAIR_LOCK
#  undef UL_COMPARE_AND_WAIT_SHARED
#  undef UL_UNFAIR_LOCK64_SHARED
#  undef UL_COMPARE_AND_WAIT64
#  undef UL_COMPARE_AND_WAIT64_SHARED

#  undef ULF_WAKE_ALL
#  undef ULF_WAKE_THREAD
#  undef ULF_WAKE_ALLOW_NON_OWNER

#  undef ULF_WAIT_WORKQ_DATA_CONTENTION
#  undef ULF_WAIT_CANCEL_POINT
#  undef ULF_WAIT_ADAPTIVE_SPIN

UTL_NAMESPACE_END

#endif // UTL_TARGET_APPLE
