// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/exception.h"
#include "utl/scope/utl_scope_impl.h"

UTL_NAMESPACE_BEGIN

#ifdef UTL_CXX17
template <typename F>
class scope_fail : private details::scope::impl<scope_fail<F>, F> {
    using base_type = details::scope::impl<scope_fail<F>, F>;
    using is_movable = typename base_type::is_movable;
    using move_t = conditional_t<is_movable::value, scope_fail, details::scope::invalid_t>;
    friend base_type;

public:
    template <typename Fn, typename = enable_if_t<is_constructible<F, Fn&&>::value>>
    scope_fail(Fn&& func) noexcept(is_nothrow_constructible<F, Fn&&>::value)
        : base_type(forward<Fn>(func))
        , exceptions_(uncaught_exceptions()) {}
    scope_fail(move_t&& other) noexcept(is_nothrow_move_constructible<F>::value)
        : base_type(move(other))
        , exceptions_(other.exceptions_) {}

    using base_type::release;

private:
    bool should_invoke() const noexcept { return exceptions_ < uncaught_exceptions(); }
    int exceptions_;
};

template <typename Fn>
explicit scope_fail(Fn&& f) -> scope_fail<decay_t<Fn>>;

template <typename Fn, typename = enable_if_t<is_constructible<scope_fail<decay_t<Fn>>, Fn>::value>>
scope_fail<decay_t<Fn>> make_scope_fail(Fn&& f) noexcept(
    is_nothrow_constructible<scope_fail<decay_t<Fn>>, declval<Fn>()>) {
    return scope_fail<decay_t<Fn>>{forward<Fn>(f)};
}

namespace details {
struct scope_fail_proxy_t {
    template <typename Fn,
        typename = enable_if_t<is_constructible<scope_fail<decay_t<Fn>>, Fn>::value>>
    scope_fail<decay_t<Fn>> operator->*(Fn&& f) const
        noexcept(is_nothrow_constructible<scope_fail<decay_t<Fn>>, declval<Fn>()>) {
        return scope_fail<decay_t<Fn>>{forward<Fn>(f)};
    }
} scope_fail_proxy;
} // namespace details

#  define UTL_ON_SCOPE_FAIL() auto UTL_UNIQUE_VAR(ScopeFail) = scope_fail_proxy->*[&]()
#else
template <typename F>
class scope_fail {
    static_assert(sizeof(F) == 0, "scope_fail is unsupported prior to C++17");
};
#endif

UTL_NAMESPACE_END
