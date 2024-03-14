// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/scope/utl_scope_impl.h"

UTL_NAMESPACE_BEGIN

template <typename F>
class scope_exit : private details::scope::impl<scope_exit<F>, F> {
    using base_type = details::scope::impl<scope_exit<F>, F>;
    using base_type::is_movable;
    using move_t = conditional_t<is_movable::value, scope_exit, details::scope::invalid_t>;

public:
    template <typename Fn, typename = enable_if_t<is_constructible<F, Fn&&>::value>>
    scope_exit(Fn&& func) noexcept(is_nothrow_constructible<F, Fn&&>::value)
        : base_type(forward<Fn>(func)) {}
    scope_exit(move_t&& other) noexcept(is_nothrow_move_constructible<F>::value)
        : base_type(move(other)) {}

    using base_type::release;

private:
    static constexpr bool should_invoke() noexcept { return true; }
};

#ifdef UTL_CXX17

template <typename Fn>
explicit scope_exit(Fn&& f) -> scope_exit<decay_t<Fn>>;

#endif

template <typename Fn, typename = enable_if_t<is_constructible<scope_exit<decay_t<Fn>>, Fn>::value>>
scope_exit<decay_t<Fn>> make_scope_exit(Fn&& f) noexcept(
    is_nothrow_constructible<scope_exit<decay_t<Fn>>, declval<Fn>()>) {
    return scope_exit<decay_t<Fn>>{forward<Fn>(f)};
}

namespace details {
namespace scope {
struct exit_proxy_t {
    template <typename Fn,
        typename = enable_if_t<is_constructible<scope_exit<decay_t<Fn>>, Fn>::value>>
    scope_exit<decay_t<Fn>> operator->*(Fn&& f) const
        noexcept(is_nothrow_constructible<scope_exit<decay_t<Fn>>, declval<Fn>()>) {
        return scope_exit<decay_t<Fn>>{forward<Fn>(f)};
    }
} exit_proxy;
} // namespace scope
} // namespace details

#define UTL_ON_SCOPE_FAIL() \
    auto UTL_UNIQUE_VAR(ScopeFail) = UTL_SCOPE details::scope::exit_proxy->*[&]()

UTL_NAMESPACE_END
