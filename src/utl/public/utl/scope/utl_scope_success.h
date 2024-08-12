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
class UTL_ATTRIBUTES(_PUBLIC_TEMPLATE, NODISCARD) scope_success :
    private details::scope::impl<scope_success<F>, F> {
    using base_type UTL_NODEBUG = details::scope::impl<scope_success<F>, F>;
    using typename base_type::invalid_t;
    using typename base_type::is_movable;
    using move_t UTL_NODEBUG = conditional_t<is_movable::value, scope_success, invalid_t>;
    friend base_type;

public:
    template <UTL_CONCEPT_CXX20(constructible_as<F, add_rvalue_reference>) Fn
            UTL_REQUIRES_CXX11(UTL_TRAIT_is_constructible(F, Fn&&))>
    __UTL_HIDE_FROM_ABI explicit scope_success(Fn&& func) noexcept(
        UTL_TRAIT_is_nothrow_constructible(F, Fn&&))
        : base_type(UTL_SCOPE forward<Fn>(func))
        , exceptions_(uncaught_exceptions()) {}
    scope_success(scope_success const&) = delete;
    __UTL_HIDE_FROM_ABI scope_success(move_t&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(F))
        : base_type(UTL_SCOPE move(other))
        , exceptions_(other.exceptions_) {}

    using base_type::release;

    __UTL_HIDE_FROM_ABI ~scope_success() noexcept {
        if (!should_invoke()) {
            release();
        }
    }

private:
    __UTL_HIDE_FROM_ABI bool should_invoke() const noexcept {
        return exceptions_ >= uncaught_exceptions();
    }
    int exceptions_;
};

template <typename Fn>
explicit scope_success(Fn&& f) -> scope_success<decay_t<Fn>>;

template <typename Fn>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) auto make_scope_success(Fn&& f) noexcept(
    UTL_TRAIT_is_nothrow_constructible(scope_success<decay_t<Fn>>, Fn))
    -> enable_if_t<UTL_TRAIT_is_constructible(scope_success<decay_t<Fn>>, Fn),
        scope_success<decay_t<Fn>>> {
    return scope_success<decay_t<Fn>>{UTL_SCOPE forward<Fn>(f)};
}

namespace details {
namespace scope {
struct success_factory_t {
    __UTL_HIDE_FROM_ABI constexpr explicit success_factory_t() noexcept = default;
    template <typename Fn>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) auto operator->*(Fn&& f) const
        noexcept(UTL_TRAIT_is_nothrow_constructible(scope_success<decay_t<Fn>>, Fn))
            -> enable_if_t<UTL_TRAIT_is_constructible(scope_success<decay_t<Fn>>, Fn),
                scope_success<decay_t<Fn>>> {
        return scope_success<decay_t<Fn>>{UTL_SCOPE forward<Fn>(f)};
    }
};

inline constexpr success_factory_t success_factory{};
} // namespace scope
} // namespace details

#  define UTL_ON_SCOPE_SUCCESS \
      const auto UTL_UNIQUE_VAR(ScopeSuccess) = UTL_SCOPE details::scope::success_factory->*[&]()

#else
template <typename F>
class scope_success {
    static_assert(sizeof(F) == 0, "scope_success is unsupported prior to C++17");
};
#endif

UTL_NAMESPACE_END
