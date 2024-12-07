// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_FUTEX_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_TARGET_BSD
#  error Invalid Target
#endif // UTL_TARGET_BSD

#include "utl/memory/utl_addressof.h"
#include "utl/system_error/utl_errc.h"
#include "utl/system_error/utl_error_category.h"
#include "utl/system_error/utl_error_code.h"
#include "utl/tempus/utl_duration.h"
#include "utl/type_traits/utl_is_const.h"
#include "utl/type_traits/utl_is_trivially_copyable.h"
#include "utl/type_traits/utl_is_volatile.h"

#include <errno.h>
#include <sys/futex.h>
#include <sys/time.h>

UTL_NAMESPACE_BEGIN

namespace futex {

#if UTL_CXX20

template <typename T>
concept waitable_type = (sizeof(T) == 4 && alignof(T) == 4 && UTL_TRAIT_is_trivially_copyable(T) &&
    !UTL_TRAIT_is_const(T) && !UTL_TRAIT_is_volatile(T));
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
auto waitable_impl(int) noexcept -> __UTL bool_constant<sizeof(T) == 4 && alignof(T) == 4 &&
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

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) auto wait(T* address, T const& value,
    __UTL tempus::duration t) noexcept -> UTL_ENABLE_IF_CXX11(error_code, UTL_TRAIT_is_futex_waitable(T)) {
    if (t == __UTL tempus::duration::zero()) {
        return make_error_code(errc::timed_out);
    }

    auto const timeout = static_cast<timespec>(t);
    auto const timeout_ptr = !t ? nullptr : &timeout;

    int readable_value = 0;
    __UTL_MEMCPY(&readable_value, __UTL addressof(value), sizeof(value));
    auto wait_on = reinterpret_cast<uint32_t*>(address);

    if (!::futex(wait_on, FUTEX_WAIT, readable_value, timeout_ptr, nullptr)) {
        return error_code{};
    }

    auto const error = errno;
    if (error == EAGAIN) {
        return error_code{};
    }

    return error_code{error, system_category()};
}

template <UTL_CONCEPT_CXX20(waitable_type) T>
UTL_ATTRIBUTE(_HIDE_FROM_ABI) auto notify_one(T* address) noexcept
    -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_futex_waitable(T)) {
    static constexpr int wake_one = 1;
    ::futex(reinterpret_cast<uint32_t*>(address), FUTEX_WAKE, wake_one, nullptr, nullptr);
}

template <UTL_CONCEPT_CXX20(waitable_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_futex_waitable(T))>
UTL_ATTRIBUTE(_HIDE_FROM_ABI) auto notify_all(T* address) noexcept
    -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_futex_waitable(T)) {
    static constexpr int wake_all = INT_MAX;
    ::futex(reinterpret_cast<uint32_t*>(address), FUTEX_WAKE, wake_all, nullptr, nullptr);
}

#undef UTL_TRAIT_is_futex_waitable
} // namespace futex

UTL_NAMESPACE_END
