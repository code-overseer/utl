// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/exception.h"
#include "utl/scope/utl_scope_impl.h"

UTL_NAMESPACE_BEGIN

#if UTL_CXX17
template <typename F>
class scope_fail : private details::scope::impl<scope_fail<F>, F> {
    using base_type = details::scope::impl<scope_fail<F>, F>;
    using is_movable = typename base_type::is_movable;
    using move_t = conditional_t<is_movable::value, scope_fail, details::scope::invalid_t>;
    friend base_type;

public:
    template <typename Fn, typename = enable_if_t<UTL_TRAIT_is_constructible(F, Fn&&)>>
    explicit scope_fail(Fn&& func) noexcept(UTL_TRAIT_is_nothrow_constructible(F, Fn&&))
        : base_type(UTL_SCOPE forward<Fn>(func))
        , exceptions_(uncaught_exceptions()) {}
    scope_fail(move_t&& other) noexcept(UTL_TRAIT_is_nothrow_move_constructible(F))
        : base_type(UTL_SCOPE move(other))
        , exceptions_(other.exceptions_) {}

    using base_type::release;

    ~scope_fail() noexcept {
        if (!should_invoke()) {
            release();
        }
    }

private:
    bool should_invoke() const noexcept { return exceptions_ < uncaught_exceptions(); }
    int exceptions_;
};

template <typename Fn>
explicit scope_fail(Fn&& f) -> scope_fail<decay_t<Fn>>;

template <typename Fn>
auto make_scope_fail(Fn&& f) noexcept(UTL_TRAIT_is_nothrow_constructible(scope_fail<decay_t<Fn>>,
    Fn)) -> enable_if_t<UTL_TRAIT_is_constructible(scope_fail<decay_t<Fn>>, Fn),
    scope_fail<decay_t<Fn>>> {
    return scope_fail<decay_t<Fn>>{UTL_SCOPE forward<Fn>(f)};
}

namespace details {
namespace scope {
UTL_INLINE_CXX17 constexpr struct fail_proxy_t {
    template <typename Fn>
    auto operator->*(Fn&& f) const
        noexcept(UTL_TRAIT_is_nothrow_constructible(scope_fail<decay_t<Fn>>, Fn))
            -> enable_if_t<UTL_TRAIT_is_constructible(scope_fail<decay_t<Fn>>, Fn),
                scope_fail<decay_t<Fn>>> {
        return scope_fail<decay_t<Fn>>{UTL_SCOPE forward<Fn>(f)};
    }
} fail_proxy = {};
} // namespace scope
} // namespace details

#  define UTL_ON_SCOPE_FAIL \
      const auto UTL_UNIQUE_VAR(ScopeFail) = UTL_SCOPE details::scope::fail_proxy->*[&]()

#else  // if UTL_CXX17
template <typename F>
class scope_fail {
    static_assert(sizeof(F) == 0, "scope_fail is unsupported prior to C++17");
};
#endif // if UTL_CXX17

UTL_NAMESPACE_END
