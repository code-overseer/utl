// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_invoke.h"
#include "utl/utility/utl_forward.h"

UTL_NAMESPACE_BEGIN

template <typename F, typename... Args>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr invoke_result_t<F, Args> invoke(
    F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable_r(invoke_result_t<F, Args>, F,
    Args...)) {
    return details::invocable::invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...);
}

namespace details {
namespace invoke {
template <typename R>
struct invoke_t {
    template <typename F, typename... Args>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr R call(
        F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable_r(R, F, Args...)) {
        return details::invocable::invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...);
    }
};

template <>
struct invoke_t<void> {
    template <typename F, typename... Args>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr void call(
        F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...)) {
        details::invocable::invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...);
    }
};
} // namespace invoke
} // namespace details

template <typename R, typename F, typename... Args UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_invocable_r(R, F, Args...))>
UTL_REQUIRES_CXX20(UTL_TRAIT_is_invocable_r(R, F, Args...))
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr R invoke_r(F&& f, Args&&... args) noexcept(
    UTL_TRAIT_is_nothrow_invocable_r(R, F, Args...)) {
    return details::invoke::invoke_t<R>::call(__UTL forward<F>(f), __UTL forward<Args>(args)...);
}

UTL_NAMESPACE_END
