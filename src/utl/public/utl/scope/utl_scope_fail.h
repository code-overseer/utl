// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_constructible_as.h"
#include "utl/exception.h"
#include "utl/scope/utl_scope_impl.h"
#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/utility/utl_customization_point.h"

UTL_NAMESPACE_BEGIN

#if UTL_CXX17

template <typename F>
class
    UTL_ATTRIBUTES(_PUBLIC_TEMPLATE, NODISCARD) scope_fail : private details::scope::impl<scope_fail<F>, F> {
    using base_type = details::scope::impl<scope_fail<F>, F>;
    using typename base_type::invalid_t;
    using typename base_type::is_movable;
    using move_t = conditional_t<is_movable::value, scope_fail, invalid_t>;
    friend base_type;

public:
    template <UTL_CONCEPT_CXX20(constructible_as<F, add_rvalue_reference>) Fn
            UTL_REQUIRES_CXX11(UTL_TRAIT_is_constructible(F, Fn&&))>
    __UTL_HIDE_FROM_ABI explicit scope_fail(Fn&& func) noexcept(
        UTL_TRAIT_is_nothrow_constructible(F, Fn&&))
        : base_type(__UTL forward<Fn>(func))
        , exceptions_(__UTL uncaught_exceptions()) {}
    scope_fail(scope_fail const&) = delete;
    __UTL_HIDE_FROM_ABI scope_fail(move_t&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(F))
        : base_type(__UTL move(other))
        , exceptions_(other.exceptions_) {}

    using base_type::release;

    __UTL_HIDE_FROM_ABI ~scope_fail() noexcept {
        if (!should_invoke()) {
            release();
        }
    }

private:
    __UTL_HIDE_FROM_ABI bool should_invoke() const noexcept {
        return exceptions_ < __UTL uncaught_exceptions();
    }
    int exceptions_;
};

template <typename Fn>
explicit scope_fail(Fn&& f) -> scope_fail<decay_t<Fn>>;

template <typename Fn>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto make_scope_fail(Fn&& f) noexcept(
    UTL_TRAIT_is_nothrow_constructible(scope_fail<decay_t<Fn>>, Fn))
    -> enable_if_t<UTL_TRAIT_is_constructible(scope_fail<decay_t<Fn>>, Fn),
        scope_fail<decay_t<Fn>>> {
    return scope_fail<decay_t<Fn>>{__UTL forward<Fn>(f)};
}

namespace details {
namespace scope {
struct fail_factory_t {
    __UTL_HIDE_FROM_ABI constexpr explicit fail_factory_t() noexcept = default;
    template <typename Fn>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto operator->*(Fn&& f) const
        noexcept(UTL_TRAIT_is_nothrow_constructible(scope_fail<decay_t<Fn>>, Fn))
            -> enable_if_t<UTL_TRAIT_is_constructible(scope_fail<decay_t<Fn>>, Fn),
                scope_fail<decay_t<Fn>>> {
        return scope_fail<decay_t<Fn>>{__UTL forward<Fn>(f)};
    }
};

inline constexpr fail_factory_t fail_factory{};
} // namespace scope
} // namespace details

#  define UTL_ON_SCOPE_FAIL \
      const auto UTL_UNIQUE_VAR(ScopeFail) = __UTL details::scope::fail_factory->*[&]()

#else  // if UTL_CXX17
template <typename F>
class scope_fail {
    static_assert(sizeof(F) == 0, "scope_fail is unsupported prior to C++17");
};
#endif // if UTL_CXX17

UTL_NAMESPACE_END
