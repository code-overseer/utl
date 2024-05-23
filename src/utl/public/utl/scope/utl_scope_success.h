// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/exception.h"
#include "utl/scope/utl_scope_impl.h"

UTL_NAMESPACE_BEGIN

#if UTL_CXX17
template <typename F>
class scope_success : private details::scope::impl<scope_success<F>, F> {
    using base_type = details::scope::impl<scope_success<F>, F>;
    using is_movable = typename base_type::is_movable;
    using move_t = conditional_t<is_movable::value, scope_success, details::scope::invalid_t>;
    friend base_type;

public:
    template <typename Fn, typename = enable_if_t<UTL_TRAIT_VALUE(is_constructible, F, Fn&&)>>
    explicit scope_success(Fn&& func) noexcept(UTL_TRAIT_VALUE(is_nothrow_constructible, F, Fn&&))
        : base_type(forward<Fn>(func))
        , exceptions_(uncaught_exceptions()) {}
    scope_success(move_t&& other) noexcept(UTL_TRAIT_VALUE(is_nothrow_move_constructible, F))
        : base_type(move(other))
        , exceptions_(other.exceptions_) {}

    using base_type::release;

    ~scope_success() noexcept {
        if (!should_invoke()) {
            release();
        }
    }

private:
    bool should_invoke() const noexcept { return exceptions_ >= uncaught_exceptions(); }
    int exceptions_;
};

template <typename Fn>
explicit scope_success(Fn&& f) -> scope_success<decay_t<Fn>>;

template <typename Fn>
auto make_scope_success(Fn&& f) noexcept(
    UTL_TRAIT_VALUE(is_nothrow_constructible, scope_success<decay_t<Fn>>, Fn))
    -> enable_if_t<UTL_TRAIT_VALUE(is_constructible, scope_success<decay_t<Fn>>, Fn),
        scope_success<decay_t<Fn>>> {
    return scope_success<decay_t<Fn>>{forward<Fn>(f)};
}

namespace details {
namespace scope {
UTL_INLINE_CXX17 constexpr struct success_proxy_t {
    template <typename Fn>
    auto operator->*(Fn&& f) const
        noexcept(UTL_TRAIT_VALUE(is_nothrow_constructible, scope_success<decay_t<Fn>>, Fn))
            -> enable_if_t<UTL_TRAIT_VALUE(is_constructible, scope_success<decay_t<Fn>>, Fn),
                scope_success<decay_t<Fn>>> {
        return scope_success<decay_t<Fn>>{forward<Fn>(f)};
    }
} success_proxy = {};
} // namespace scope
} // namespace details

#  define UTL_ON_SCOPE_SUCCESS \
      const auto UTL_UNIQUE_VAR(ScopeSuccess) = UTL_SCOPE details::scope::success_proxy->*[&]()

#else
template <typename F>
class scope_success {
    static_assert(sizeof(F) == 0, "scope_success is unsupported prior to C++17");
};
#endif

UTL_NAMESPACE_END
