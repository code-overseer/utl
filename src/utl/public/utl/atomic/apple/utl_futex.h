// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_FUTEX_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_TARGET_APPLE
#  error Invalid target
#endif // UTL_TARGET_APPLE

#include "utl/memory/utl_addressof.h"
#include "utl/numeric/utl_add_sat.h"
#include "utl/numeric/utl_limits.h"
#include "utl/system_error/utl_errc.h"
#include "utl/system_error/utl_error_category.h"
#include "utl/system_error/utl_error_code.h"
#include "utl/tempus/utl_duration.h"
#include "utl/type_traits/utl_constants.h"

#include <errno.h>
#include <stdint.h>

UTL_EXTERN_C_BEGIN
int __ulock_wait(uint32_t operation, void* addr, uint64_t value, uint32_t timeout);
int __ulock_wake(uint32_t operation, void* addr, uint64_t thread_id);
UTL_EXTERN_C_END

UTL_NAMESPACE_BEGIN

#define UL_COMPARE_AND_WAIT 1
#define UL_UNFAIR_LOCK 2
#define UL_COMPARE_AND_WAIT_SHARED 3
#define UL_UNFAIR_LOCK64_SHARED 4
#define UL_COMPARE_AND_WAIT64 5
#define UL_COMPARE_AND_WAIT64_SHARED 6

#define ULF_WAKE_ALL 0x00000100
#define ULF_WAKE_THREAD 0x00000200
#define ULF_WAKE_ALLOW_NON_OWNER 0x00000400

#define ULF_WAIT_WORKQ_DATA_CONTENTION 0x00010000
#define ULF_WAIT_CANCEL_POINT 0x00020000
#define ULF_WAIT_ADAPTIVE_SPIN 0x00040000

#define __UTL_UNUSED 0

namespace futex {

UTL_INLINE_CXX17 constexpr size_t max_size = 8;
UTL_INLINE_CXX17 constexpr size_t min_size = 4;
#if UTL_CXX20
template <typename T>
concept waitable_type =
    (sizeof(T) <= max_size && sizeof(T) >= min_size && alignof(T) == sizeof(T) &&
        UTL_TRAIT_is_trivially_copyable(T) && !UTL_TRAIT_is_const(T) && !UTL_TRAIT_is_volatile(T));
template <typename T>
struct is_waitable : __UTL bool_constant<waitable_type<T>> {};
template <typename T>
inline constexpr bool is_waitable_v = waitable_type<T>;
#  define UTL_TRAIT_is_futex_waitable(TYPE) __UTL futex::waitable_type<TYPE>

#else // UTL_CXX20

namespace details {
template <typename T>
auto waitable_impl(float) noexcept -> __UTL false_type;
template <typename T>
auto waitable_impl(int) noexcept -> __UTL
    bool_constant<(sizeof(T) <= max_size && sizeof(T) >= min_size) && alignof(T) == sizeof(T) &&
        UTL_TRAIT_is_trivially_copyable(T) && !UTL_TRAIT_is_const(T) && !UTL_TRAIT_is_volatile(T)>;
template <typename T>
using waitable UTL_NODEBUG = decltype(waitable_impl<T>(0));
} // namespace details

template <typename T>
struct is_waitable : details::waitable<T> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_waitable_v = __UTL futex::details::waitable<T>::value;
#  endif // UTL_CXX14
#  define UTL_TRAIT_is_futex_waitable(TYPE) __UTL futex::details::waitable<TYPE>::value
#endif // UTL_CXX20

namespace details {
UTL_ATTRIBUTE(_HIDE_FROM_ABI) inline uint32_t to_microseconds(__UTL tempus::duration t) noexcept {
    if (!t) {
        return UTL_NUMERIC_maximum(uint32_t);
    }

    auto const micro = __UTL add_sat<uint64_t>(t.seconds() * 1000000, t.nanoseconds() / 1000);
    return micro > UTL_NUMERIC_maximum(uint32_t) ? UTL_NUMERIC_maximum(uint32_t) : micro;
}
} // namespace details

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 4)
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline auto wait(
    T* address, T const& value, __UTL tempus::duration t) noexcept
    -> UTL_ENABLE_IF_CXX11(error_code, UTL_TRAIT_is_futex_waitable(T) && sizeof(T) == 4) {
    if (t == __UTL tempus::duration::zero()) {
        return make_error_code(errc::timed_out);
    }

    static constexpr uint32_t type = UL_COMPARE_AND_WAIT | UL_UNFAIR_LOCK;
    static constexpr uint32_t op = type | ULF_WAIT_WORKQ_DATA_CONTENTION;
    uint32_t const microseconds = details::to_microseconds(t);
    uint64_t readable_value = 0;
    __UTL_MEMCPY(&readable_value, __UTL addressof(value), sizeof(value));
    // If value is not equal ulock_wait returns 0
    if (__ulock_wait(op, address, readable_value, microseconds)) {
        return error_code{errno, system_category()};
    }

    return error_code{};
}

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 4)
UTL_ATTRIBUTE(_HIDE_FROM_ABI) inline auto notify_one(T* address) noexcept
    -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_futex_waitable(T) && sizeof(T) == 4) {
    static constexpr uint32_t op = UL_COMPARE_AND_WAIT | UL_UNFAIR_LOCK;
    __ulock_wake(op, address, __UTL_UNUSED);
}

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 4)
UTL_ATTRIBUTE(_HIDE_FROM_ABI) inline auto notify_all(T* address) noexcept
    -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_futex_waitable(T) && sizeof(T) == 4) {
    static constexpr uint32_t type = UL_COMPARE_AND_WAIT | UL_UNFAIR_LOCK;
    static constexpr uint32_t op = type | ULF_WAKE_ALL;
    __ulock_wake(op, address, __UTL_UNUSED);
}

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 8)
UTL_ATTRIBUTE(_HIDE_FROM_ABI, NODISCARD) inline auto wait(
    T* address, T const& value, __UTL tempus::duration t) noexcept
    -> UTL_ENABLE_IF_CXX11(error_code, UTL_TRAIT_is_futex_waitable(T) && sizeof(T) == 8) {
    if (t == __UTL tempus::duration::zero()) {
        return make_error_code(errc::timed_out);
    }

    static constexpr uint32_t type = UL_COMPARE_AND_WAIT64 | UL_UNFAIR_LOCK;
    static constexpr uint32_t op = type | ULF_WAIT_WORKQ_DATA_CONTENTION;
    uint32_t const microseconds = details::to_microseconds(t);
    uint64_t readable_value = 0;
    __UTL_MEMCPY(&readable_value, __UTL addressof(value), sizeof(value));
    // If value is not equal ulock_wait returns 0
    if (__ulock_wait(op, address, readable_value, microseconds)) {
        return error_code{errno, system_category()};
    }

    return error_code{};
}

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 8)
UTL_ATTRIBUTE(_HIDE_FROM_ABI) inline auto notify_one(T* address) noexcept
    -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_futex_waitable(T) && sizeof(T) == 8) {
    static constexpr uint32_t op = UL_COMPARE_AND_WAIT64 | UL_UNFAIR_LOCK;
    __ulock_wake(op, address, __UTL_UNUSED);
}

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_CONSTRAINT_CXX20(sizeof(T) == 8)
UTL_ATTRIBUTE(_HIDE_FROM_ABI) inline auto notify_all(T* address) noexcept
    -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_futex_waitable(T) && sizeof(T) == 8) {
    static constexpr uint32_t type = UL_COMPARE_AND_WAIT64 | UL_UNFAIR_LOCK;
    static constexpr uint32_t op = type | ULF_WAKE_ALL;
    __ulock_wake(op, address, __UTL_UNUSED);
}

} // namespace futex

#undef __UTL_UNUSED

#undef UL_COMPARE_AND_WAIT
#undef UL_UNFAIR_LOCK
#undef UL_COMPARE_AND_WAIT_SHARED
#undef UL_UNFAIR_LOCK64_SHARED
#undef UL_COMPARE_AND_WAIT64
#undef UL_COMPARE_AND_WAIT64_SHARED

#undef ULF_WAKE_ALL
#undef ULF_WAKE_THREAD
#undef ULF_WAKE_ALLOW_NON_OWNER

#undef ULF_WAIT_WORKQ_DATA_CONTENTION
#undef ULF_WAIT_CANCEL_POINT
#undef ULF_WAIT_ADAPTIVE_SPIN

UTL_NAMESPACE_END
