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

template <typename F>
class UTL_ATTRIBUTES(
    _PUBLIC_TEMPLATE, NODISCARD) scope_exit : private details::scope::impl<scope_exit<F>, F> {
    using base_type UTL_NODEBUG = details::scope::impl<scope_exit<F>, F>;
    using typename base_type::invalid_t;
    using typename base_type::is_movable;
    using move_t UTL_NODEBUG = conditional_t<is_movable::value, scope_exit, invalid_t>;

public:
    template <UTL_CONCEPT_CXX20(constructible_as<F, add_rvalue_reference>) Fn UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_constructible(F, Fn&&))>
    __UTL_HIDE_FROM_ABI explicit scope_exit(Fn&& func) noexcept(
        UTL_TRAIT_is_nothrow_constructible(F, Fn&&))
        : base_type(__UTL forward<Fn>(func)) {}
    scope_exit(scope_exit const&) = delete;
    __UTL_HIDE_FROM_ABI scope_exit(move_t&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(F))
        : base_type(__UTL move(other)) {}

    using base_type::release;
};

#if UTL_CXX17

template <typename Fn>
explicit scope_exit(Fn&& f) -> scope_exit<decay_t<Fn>>;

#endif

template <typename Fn>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto make_scope_exit(Fn&& f) noexcept(
    UTL_TRAIT_is_nothrow_constructible(scope_exit<decay_t<Fn>>, Fn))
    -> enable_if_t<UTL_TRAIT_is_constructible(scope_exit<decay_t<Fn>>, Fn),
        scope_exit<decay_t<Fn>>> {
    return scope_exit<decay_t<Fn>>{forward<Fn>(f)};
}

namespace details {
namespace scope {
struct exit_factory_t {
    __UTL_HIDE_FROM_ABI constexpr explicit exit_factory_t() noexcept = default;

    template <typename Fn>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto operator->*(Fn&& f) const
        noexcept(UTL_TRAIT_is_nothrow_constructible(scope_exit<decay_t<Fn>>, Fn))
            -> enable_if_t<UTL_TRAIT_is_constructible(scope_exit<decay_t<Fn>>, Fn),
                scope_exit<decay_t<Fn>>> {
        return scope_exit<decay_t<Fn>>{forward<Fn>(f)};
    }
};

// Warning: technically not a customization point
UTL_DEFINE_CUSTOMIZATION_POINT(exit_factory_t, exit_factory);
} // namespace scope
} // namespace details

#define UTL_ON_SCOPE_EXIT \
    const auto UTL_UNIQUE_VAR(ScopeFail) = __UTL details::scope::exit_factory->*[&]()

UTL_NAMESPACE_END
