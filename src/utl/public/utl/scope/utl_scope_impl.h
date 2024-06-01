// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/utility/utl_forward.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace scope {
class invalid_t {
private:
    constexpr invalid_t() noexcept = default;
    ~invalid_t() noexcept = default;
};

template <typename Impl, typename F>
class impl {
    static_assert(UTL_TRAIT_is_invocable(F), "Callable F must be invocable");

protected:
    using is_movable = is_move_constructible<F>;

private:
    using move_t = conditional_t<is_movable::value, impl, details::scope::invalid_t>;
    using not_move_t = conditional_t<is_movable::value, details::scope::invalid_t, impl>;

protected:
    template <typename Fn, typename = enable_if_t<UTL_TRAIT_is_constructible(F, Fn&&)>>
    impl(Fn&& func) noexcept(UTL_TRAIT_is_nothrow_constructible(F, Fn&&))
        : callable(UTL_SCOPE forward<Fn>(func))
        , released_(false) {}
    impl(impl const& other) = delete;
    impl& operator=(impl const& other) = delete;
    impl(not_move_t&& other) = delete;
    impl& operator=(impl&& other) = delete;
    constexpr impl(move_t&& other) noexcept(UTL_TRAIT_is_nothrow_move_constructible(F))
        : impl(is_movable{}, UTL_SCOPE move(other)) {}

    void release() noexcept { released_ = true; }

    ~impl() noexcept(noexcept(callable())) {
        if (!released_) {
            callable();
        }
    }

private:
    // TODO ignore warning
    constexpr impl(false_type, details::scope::invalid_t&&) noexcept;
    template <typename T = impl,
        typename = enable_if_t<is_movable::value && UTL_TRAIT_is_same(T, impl)>>
    constexpr impl(true_type, T&& other) noexcept
        : callable(UTL_SCOPE move(other.callable))
        , released_(other.released_) {
        other.release();
    }

    F callable;
    bool released_;
};
} // namespace scope
} // namespace details

UTL_NAMESPACE_END
