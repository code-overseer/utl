// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/concepts/utl_same_as.h"
#include "utl/type_traits/utl_add_rvalue_reference.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/utility/utl_forward.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace scope {
template <typename Impl, typename F>
class UTL_PUBLIC_TEMPLATE impl {
    static_assert(UTL_TRAIT_is_invocable(F), "Callable F must be invocable");

protected:
    using is_movable UTL_NODEBUG = is_move_constructible<F>;
    class invalid_t {
        constexpr invalid_t() noexcept = default;
        ~invalid_t() noexcept = default;
    };

private:
    using move_t UTL_NODEBUG = conditional_t<is_movable::value, impl, invalid_t>;
    using not_move_t UTL_NODEBUG = conditional_t<is_movable::value, invalid_t, impl>;

protected:
    template <UTL_CONCEPT_CXX20(constructible_as<F, add_rvalue_reference>) Fn UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_constructible(F, Fn&&))>
    UTL_HIDE_FROM_ABI constexpr impl(Fn&& func) noexcept(
        UTL_TRAIT_is_nothrow_constructible(F, Fn&&))
        : callable_(UTL_SCOPE forward<Fn>(func))
        , released_(false) {}
    impl(impl const& other) = delete;
    impl& operator=(impl const& other) = delete;
    impl(not_move_t&& other) = delete;
    impl& operator=(impl&& other) = delete;
    UTL_HIDE_FROM_ABI constexpr impl(move_t&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(F))
        : impl(is_movable{}, UTL_SCOPE move(other)) {}

    UTL_HIDE_FROM_ABI void release() noexcept { released_ = true; }

    UTL_HIDE_FROM_ABI ~impl() noexcept(noexcept(callable_())) {
        if (!released_) {
            callable_();
        }
    }

private:
    template <UTL_CONCEPT_CXX20(same_as<invalid_t>) T UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_same(T, invalid_t))>
    UTL_HIDE_FROM_ABI impl(false_type, T&& other) noexcept;

    template <UTL_CONCEPT_CXX20(same_as<impl>) T = impl UTL_REQUIRES_CXX11(
        is_movable::value && UTL_TRAIT_is_same(T, impl))>
    UTL_REQUIRES_CXX20(is_movable::value)
    UTL_HIDE_FROM_ABI constexpr impl(true_type, T&& other) noexcept
        : callable_(UTL_SCOPE move(other.callable_))
        , released_(other.released_) {
        other.release();
    }

    F callable_;
    bool released_;
};
} // namespace scope
} // namespace details

UTL_NAMESPACE_END
