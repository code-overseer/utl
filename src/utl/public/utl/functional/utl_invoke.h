// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/utility/utl_forward.h"

UTL_NAMESPACE_BEGIN

template <typename F, typename... Args>
inline constexpr invoke_result_t<F, Args> invoke(F&& f, Args&&... args) noexcept(
    UTL_TRAIT_is_nothrow_invocable_r(invoke_result_t<F, Args>, F, Args...)) {
    return details::invocable::invoke(UTL_SCOPE forward<F>(f), UTL_SCOPE forward<Args>(args)...);
}

namespace details {
namespace invoke {
template <typename R>
struct invoke_t {
    template <typename F, typename... Args>
    static constexpr R call(F&& f, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_invocable_r(R, F, Args...)) {
        return details::invocable::invoke(
            UTL_SCOPE forward<F>(f), UTL_SCOPE forward<Args>(args)...);
    }
};

template <>
struct invoke_t<void> {
    template <typename F, typename... Args>
    static constexpr void call(F&& f, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, Args...)) {
        details::invocable::invoke(UTL_SCOPE forward<F>(f), UTL_SCOPE forward<Args>(args)...);
    }
};
} // namespace invoke
} // namespace details

template <typename R, typename F, typename... Args UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_invocable_r(R, F, Args...))>
UTL_REQUIRES_CXX20(UTL_TRAIT_is_invocable_r(R, F, Args...))
inline constexpr R invoke_r(F&& f, Args&&... args) noexcept(
    UTL_TRAIT_is_nothrow_invocable_r(R, F, Args...)) {
    return details::invoke::invoke_t<R>::call(
        UTL_SCOPE forward<F>(f), UTL_SCOPE forward<Args>(args)...);
}

UTL_NAMESPACE_END
