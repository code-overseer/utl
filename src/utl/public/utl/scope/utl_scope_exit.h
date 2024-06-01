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
    template <typename Fn, typename = enable_if_t<UTL_TRAIT_is_constructible(F, Fn&&)>>
    explicit scope_exit(Fn&& func) noexcept(UTL_TRAIT_is_nothrow_constructible(F, Fn&&))
        : base_type(forward<Fn>(func)) {}
    scope_exit(move_t&& other) noexcept(UTL_TRAIT_is_nothrow_move_constructible(F))
        : base_type(move(other)) {}

    using base_type::release;
};

#if UTL_CXX17

template <typename Fn>
explicit scope_exit(Fn&& f) -> scope_exit<decay_t<Fn>>;

#endif

template <typename Fn>
auto make_scope_exit(Fn&& f) noexcept(UTL_TRAIT_is_nothrow_constructible(scope_exit<decay_t<Fn>>,
    Fn)) -> enable_if_t<UTL_TRAIT_is_constructible(scope_exit<decay_t<Fn>>, Fn),
    scope_exit<decay_t<Fn>>> {
    return scope_exit<decay_t<Fn>>{forward<Fn>(f)};
}

namespace details {
namespace scope {
UTL_INLINE_CXX17 constexpr struct exit_proxy_t {
    template <typename Fn>
    auto operator->*(Fn&& f) const
        noexcept(UTL_TRAIT_is_nothrow_constructible(scope_exit<decay_t<Fn>>, Fn))
            -> enable_if_t<UTL_TRAIT_is_constructible(scope_exit<decay_t<Fn>>, Fn),
                scope_exit<decay_t<Fn>>> {
        return scope_exit<decay_t<Fn>>{forward<Fn>(f)};
    }
} exit_proxy = {};
} // namespace scope
} // namespace details

#define UTL_ON_SCOPE_EXIT \
    const auto UTL_UNIQUE_VAR(ScopeFail) = UTL_SCOPE details::scope::exit_proxy->*[&]()

UTL_NAMESPACE_END
