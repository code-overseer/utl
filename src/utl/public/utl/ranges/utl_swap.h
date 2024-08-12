// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_USE_STD_ranges_swap && UTL_CXX20

#  include <ranges>

UTL_NAMESPACE_BEGIN
using ::std::ranges::swap;
UTL_NAMESPACE_END

#else

#  if UTL_USE_STD_ranges_swap
UTL_PRAGMA_WARN(
    "ranges::swap is only available in C++20, ignoring `UTL_USE_STD_ranges_swap` defintion");
#  endif

#  include "utl/type_traits/utl_extent.h"
#  include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#  include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#  include "utl/utility/utl_customization_point.h"
#  include "utl/utility/utl_exchange.h"
#  include "utl/utility/utl_forward.h"

#  if UTL_CXX20

#    include "utl/concepts/utl_assignable_from.h"
#    include "utl/concepts/utl_move_constructible.h"

UTL_NAMESPACE_BEGIN

namespace ranges {
namespace details {
namespace swap {

struct function_t;
template <typename T>
void swap(T&, T&) = delete;

template <typename L, typename R>
concept unqualified_swappable =
    requires(L&& l, R&& r) { swap(__UTL forward<L>(l), __UTL forward<R>(r)); };

template <typename L, typename R>
concept nothrow_unqualified_swappable = unqualified_swappable<L, R> && requires(L&& l, R&& r) {
    { swap(__UTL forward<L>(l), __UTL forward<R>(r)) } noexcept;
};

template <typename L, typename R, size_t N>
concept unqualified_swappable_array =
    (!unqualified_swappable<L (&)[N], R (&)[N]> && __UTL extent_v<L> == __UTL extent_v<R> &&
        requires(L (&l)[N], R (&r)[N], function_t const& func) { func(*l, *r); });

template <typename T>
concept exchangable =
    !unqualified_swappable<T&, T&> && __UTL move_constructible<T> && __UTL assignable_from<T&, T>;

struct function_t {
    template <typename L, typename R>
    requires unqualified_swappable<L, R>
    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) UTL_CONSTEXPR_CXX14 void operator()(L&& l, R&& r) const
        noexcept(nothrow_unqualified_swappable<L, R>) {
        swap(l, r);
    }

    template <typename L, typename R, size_t N>
    requires unqualified_swappable_array<L, R, N>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void operator()(L (&l)[N], R (&r)[N]) const
        noexcept(noexcept((*this)(*l, *r))) {
        for (decltype(N) i = 0; i < N; ++i) {
            (*this)(l[i], r[i]);
        }
    }

    template <exchangable T>
    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI)
    UTL_CONSTEXPR_CXX14 void operator()(T& l, T& r) const noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_assignable(T)) {
        r = __UTL exchange(l, __UTL move(r));
    }
};
} // namespace swap
} // namespace details

inline namespace cpo {
UTL_DEFINE_CUSTOMIZATION_POINT(__UTL ranges::details::swap::function_t, swap);
} // namespace cpo

namespace details {
namespace swap {
template <typename L, typename R>
concept invocable = requires(L&& l, R&& r) {
    __UTL ranges::swap(__UTL forward<L>(l), __UTL forward<R>(r));
    __UTL ranges::swap(__UTL forward<R>(r), __UTL forward<L>(l));
};

template <typename L, typename R>
concept nothrow_invocable = invocable<L, R> && requires(L&& l, R&& r) {
    { __UTL ranges::swap(__UTL forward<L>(l), __UTL forward<R>(r)) } noexcept;
    { __UTL ranges::swap(__UTL forward<R>(r), __UTL forward<L>(l)) } noexcept;
};
} // namespace swap
} // namespace details

} // namespace ranges

UTL_NAMESPACE_END

#  else

#    include "utl/type_traits/utl_constants.h"
#    include "utl/type_traits/utl_declval.h"
#    include "utl/type_traits/utl_enable_if.h"
#    include "utl/type_traits/utl_is_move_assignable.h"
#    include "utl/type_traits/utl_is_move_constructible.h"
#    include "utl/type_traits/utl_logical_traits.h"
#    include "utl/type_traits/utl_void_t.h"

UTL_NAMESPACE_BEGIN

namespace ranges {
namespace details {
namespace swap {

struct function_t;
template <typename T>
void swap(T&, T&) = delete;

template <typename L, typename R>
__UTL_HIDE_FROM_ABI auto unqualified_swappable_impl(float) noexcept -> __UTL false_type;
template <typename L, typename R>
__UTL_HIDE_FROM_ABI auto unqualified_swappable_impl(int) noexcept
    -> __UTL always_true_type<decltype(swap(__UTL declval<L>(), __UTL declval<R>()))>;

template <typename L, typename R>
__UTL_HIDE_FROM_ABI auto nothrow_unqualified_swappable_impl(float) noexcept -> __UTL false_type;
template <typename L, typename R>
__UTL_HIDE_FROM_ABI auto nothrow_unqualified_swappable_impl(int) noexcept
    -> __UTL bool_constant<noexcept(swap(__UTL declval<L>(), __UTL declval<R>()))>;

template <typename L, typename R, size_t N>
__UTL_HIDE_FROM_ABI auto unqualified_swappable_array_impl(float) noexcept -> __UTL false_type;

template <typename L, typename R, size_t N, typename F = function_t const&>
__UTL_HIDE_FROM_ABI auto unqualified_swappable_array_impl(int) noexcept -> __UTL conjunction<
    __UTL bool_constant<(__UTL extent_v<L> == __UTL extent_v<R>)>,
    __UTL always_true_type<decltype(__UTL declval<F>()(__UTL declval<L&>(), __UTL declval<R&>()))>>;

template <typename L, typename R>
using unqualified_swappable = decltype(unqualified_swappable_impl<L, R>(0));
template <typename L, typename R>
using nothrow_unqualified_swappable = decltype(nothrow_unqualified_swappable_impl<L, R>(0));
template <typename L, typename R, size_t N>
using unqualified_swappable_array = decltype(unqualified_swappable_array_impl<L, R, N>(0));

struct function_t {
private:
    struct third_t {};
    struct second_t : third_t {};
    static constexpr struct first_t : second_t {
    } first = {};

    template <typename L, typename R>
    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI)
    UTL_CONSTEXPR_CXX14 __UTL enable_if_t<unqualified_swappable<L, R>::value>
    dispatch(L&& l, R&& r, first_t) const noexcept(nothrow_unqualified_swappable<L, R>::value) {
        swap(__UTL forward<L>(l), __UTL forward<R>(r));
    }

    template <typename L, typename R, size_t N>
    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI)
    UTL_CONSTEXPR_CXX14 __UTL enable_if_t<unqualified_swappable_array<L, R, N>::value>
    dispatch(L (&l)[N], R (&r)[N], second_t) const noexcept(noexcept((*this)(*l, *r))) {
        for (decltype(N) i = 0; i < N; ++i) {
            (*this)(l[i], r[i]);
        }
    }

    template <typename T>
    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI)
    UTL_CONSTEXPR_CXX14 __UTL
        enable_if_t<UTL_TRAIT_is_move_constructible(T) && UTL_TRAIT_is_move_assignable(T)>
        dispatch(T& l, T& r, third_t) const noexcept(
            UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_assignable(T)) {
        r = __UTL exchange(l, __UTL move(r));
    }

public:
    template <typename L, typename R>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 auto operator()(L&& l, R&& r) const noexcept(
        noexcept(this->dispatch(__UTL declval<L>(), __UTL declval<R>(), function_t::first)))
        -> decltype(this->dispatch(__UTL declval<L>(), __UTL declval<R>(), function_t::first)) {
        this->dispatch(__UTL forward<L>(l), __UTL forward<R>(r), function_t::first);
    }
};
} // namespace swap
} // namespace details

inline namespace cpo {
UTL_DEFINE_CUSTOMIZATION_POINT(__UTL ranges::details::swap::function_t, swap);
} // namespace cpo
} // namespace ranges

UTL_NAMESPACE_END

#  endif

#endif
