// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_FUTEX_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_TARGET_MICROSOFT
#  error Invalid Target
#endif

// Tested on: https://godbolt.org/z/K393T4884

#include "utl/memory/utl_addressof.h"
#include "utl/numeric/utl_limits.h"
#include "utl/type_traits/utl_is_class.h"
#include "utl/type_traits/utl_remove_cv.h"

#pragma comment(lib, "synchronization")
#pragma comment(lib, "kernel32")

#ifndef _WINDEF_
typedef void VOID, *PVOID;
typedef int BOOL;
typedef unsigned long DWORD;
#  if defined(_WIN64)
typedef unsigned __int64 ULONG_PTR;
#  else
typedef unsigned long ULONG_PTR;
#  endif
typedef ULONG_PTR SIZE_T;
#endif

extern "C" DWORD GetLastError();
extern "C" BOOL WaitOnAddress(VOID volatile*, PVOID, SIZE_T, DWORD);
extern "C" void WakeByAddressSingle(PVOID);
extern "C" void WakeByAddressAll(PVOID);

namespace futex {
namespace details {
UTL_INLINE_CXX17 constexpr DWORD error_timeout = 0x5B4u;
UTL_INLINE_CXX17 constexpr DWORD error_success = 0x0u;
} // namespace details

UTL_CONSTEVAL result result::success() noexcept {
    return result(details::error_success);
}

constexpr bool result::interrupted() const noexcept {
    return false;
}
constexpr bool result::timed_out() const noexcept {
    return static_cast<DWORD>(value_) == details::error_timeout;
}
constexpr bool result::failed() const noexcept {
    return value_ != details::error_success;
}

UTL_INLINE_CXX17 constexpr size_t max_size = 8;
UTL_INLINE_CXX17 constexpr size_t min_size = 1;
#if UTL_CXX20
template <typename T>
concept waitable_type = (sizeof(T) <= max_size && sizeof(T) >= min_size &&
    alignof(T) == sizeof(T) && UTL_TRAIT_is_trivially_copyable(T));
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
auto waitable_impl(int) noexcept -> __UTL bool_constant<sizeof(T) <= max_size &&
    sizeof(T) >= min_size && alignof(T) == sizeof(T) && UTL_TRAIT_is_trivially_copyable(T)>;
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
UTL_ATTRIBUTE(_HIDE_FROM_ABI) inline DWORD to_milliseconds(__UTL tempus::duration t) noexcept {
    if (!t) {
        return UTL_NUMERIC_maximum(DWORD);
    }

    auto const milli = __UTL add_sat<uint64_t>(t.seconds() * 1000, t.nanoseconds() / 1000000);
    return milli > UTL_NUMERIC_maximum(DWORD) ? UTL_NUMERIC_maximum(DWORD) : milli;
}
} // namespace details

template <typename T>
result wait(T const*, T const& value, __UTL tempus::duration) = delete;
template <typename T>
result wait(T const volatile*, T const& value, __UTL tempus::duration) = delete;

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) auto wait(
    T* address, remove_cv_t<T> value, __UTL tempus::duration t) noexcept
    -> UTL_ENABLE_IF_CXX11(result, UTL_TRAIT_is_futex_waitable(T) && !UTL_TRAIT_is_class(T)) {
    if (t == __UTL tempus::duration::zero()) {
        return result{details::ERROR_TIMEOUT};
    }

    if (WaitOnAddress(address, __UTL addressof(value), sizeof(T), details::to_milliseconds(t))) {
        return result::success();
    }

    return result{GetLastError()};
}

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_class(T))
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) auto wait(
    T* address, remove_cv_t<T> const& value, __UTL tempus::duration t) noexcept
    -> UTL_ENABLE_IF_CXX11(result, UTL_TRAIT_is_futex_waitable(T) && UTL_TRAIT_is_class(T)) {
    if (t == __UTL tempus::duration::zero()) {
        return result{details::ERROR_TIMEOUT};
    }

    alignof(T) unsigned char buffer[sizeof(T)];
    auto cmp = __UTL_MEMCPY(buffer, __UTL addressof(value), sizeof(T));
    if (WaitOnAddress(address, cmp, sizeof(T), details::to_milliseconds(t))) {
        return result::success();
    }

    return result{GetLastError()};
}

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_ATTRIBUTE(_HIDE_FROM_ABI) auto notify_one(T* address) noexcept
    -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_futex_waitable(T)) {
    WakeByAddressSingle((PVOID)address);
}

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_ATTRIBUTE(_HIDE_FROM_ABI) auto notify_all(T* address) noexcept
    -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_futex_waitable(T)) {
    WakeByAddressAll((PVOID)address);
}

} // namespace futex
