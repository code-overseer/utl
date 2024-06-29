// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_iter_move.h"
#include "utl/ranges/utl_swap.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/utility/utl_customization_point.h"
#include "utl/utility/utl_forward.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace iterator_swap {
template <typename L, typename R>
UTL_CONSTEXPR_CXX14 UTL_SCOPE iter_value_t<L> iter_exchange(L&& l, R&& r) {
    UTL_SCOPE iter_value_t<L> old(UTL_SCOPE ranges::iter_move(l));
    *l = UTL_SCOPE ranges::iter_move(r);
    return old;
}
template <typename L, typename R>
void iter_swap(L l, R r) = delete;

} // namespace iterator_swap
} // namespace details
UTL_NAMESPACE_END

#if UTL_CXX20

#  include "utl/concepts/utl_swappable.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace iterator_swap {
template <typename L, typename R>
concept exchange_swappable = requires(L&& l, R&& r) {
    typename UTL_SCOPE iter_value_t<L>;
    UTL_SCOPE iter_value_t<L>(UTL_SCOPE ranges::iter_move(l));
    *l = UTL_SCOPE ranges::iter_move(r);
    *r = UTL_SCOPE details::iterator_swap::iter_exchange(
        UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r));
};

template <typename L, typename R>
concept nothrow_exchange_swappable = exchange_swappable<L, R> && requires(L&& l, R&& r) {
    { UTL_SCOPE iter_value_t<L>(UTL_SCOPE ranges::iter_move(l)) } noexcept;
    { *l = UTL_SCOPE ranges::iter_move(r) } noexcept;
    {
        *r = UTL_SCOPE details::iterator_swap::iter_exchange(
            UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r));
    } noexcept;
};

template <typename L, typename R>
concept dereference_swappable =
    UTL_SCOPE indirectly_readable<L> && UTL_SCOPE indirectly_readable<R> &&
    UTL_SCOPE swappable_with<UTL_SCOPE iter_reference_t<L>, UTL_SCOPE iter_reference_t<R>>;

template <typename L, typename R>
concept nothrow_dereference_swappable = dereference_swappable<L, R> && requires(L&& l, R&& r) {
    { UTL_SCOPE ranges::swap(*l, *r) } noexcept;
};

template <typename L, typename R>
requires dereferenced_swappable<L, R>
void default_impl(L&& l, R&& r) noexcept(nothrow_dereference_swappable<L, R>) {
    UTL_SCOPE ranges::swap(*l, *r);
}

template <typename L, typename R>
requires (!dereferenced_swappable<L, R> && exchange_swappable<L, R>)
void default_impl(L&& l, R&& r) noexcept(nothrow_exchange_swappable<L, R>) {
    *r = iter_exchange(UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r));
}

template <typename L, typename R>
concept defaultable = requires(L&& l, R&& r) {
    UTL_SCOPE details::iterator_swap::default_impl(
        UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r));
};

template <typename L, typename R>
concept nothrow_defaultable = defaultable<L, R> && requires(L&& l, R&& r) {
    {
        UTL_SCOPE details::iterator_swap::default_impl(
            UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r))
    } noexcept;
};

template <typename L, typename R>
requires defaultable<L, R>
constexpr void iter_swap(L&& l, R&& r) noexcept(is_nothrow_defaultable<L, R>) {
    return UTL_SCOPE details::iterator_swap::default_impl(
        UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r));
}

template <typename L, typename R>
concept nothrow_iter_swap = requires(L&& l, R&& r) {
    { iter_swap(UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r)) } noexcept;
};

struct function_t {
    template <typename L, typename R>
    requires requires { iter_swap(UTL_SCOPE declval<L>(), UTL_SCOPE declval<R>()); }
    constexpr void operator()(L&& l, R&& r) const noexcept(nothrow_iter_swap<L, R>) {
        iter_swap(UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r));
    }
};
} // namespace iterator_swap
} // namespace details
UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_constants.h"
#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_logical_traits.h"
#  include "utl/type_traits/utl_void_t.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace iterator_swap {
template <typename L, typename R>
auto exchange_swappable(float) noexcept -> UTL_SCOPE false_type;
template <typename L, typename R>
auto exchange_swappable(int) noexcept -> UTL_SCOPE always_true<UTL_SCOPE iter_value_t<L>,
    decltype(UTL_SCOPE iter_value_t<L>(UTL_SCOPE ranges::iter_move(UTL_SCOPE declval<L>()))),
    decltype(*l = UTL_SCOPE ranges::iter_move(r)),
    decltype(*r = UTL_SCOPE details::iterator_swap::iter_exchange(
                 UTL_SCOPE declval<L>(), UTL_SCOPE declval<R>()))>;

template <typename L, typename R>
using is_exchange_swappable =
    decltype(UTL_SCOPE details::iterator_swap::exchange_swappable<L, R>(0));

template <typename L, typename R>
auto nothrow_exchange_swappable(float) noexcept -> UTL_SCOPE false_type;
template <typename L, typename R>
auto nothrow_exchange_swappable(int) noexcept
    -> UTL_SCOPE bool_constant<noexcept(UTL_SCOPE iter_value_t<L>(
                                   UTL_SCOPE ranges::iter_move(UTL_SCOPE declval<L>()))) &&
        noexcept(*l = UTL_SCOPE ranges::iter_move(r)) &&
        noexcept(*r = UTL_SCOPE details::iterator_swap::iter_exchange(
                     UTL_SCOPE declval<L>(), UTL_SCOPE declval<R>()))>;

template <typename L, typename R>
using is_nothrow_exchange_swappable =
    decltype(UTL_SCOPE details::iterator_swap::nothrow_exchange_swappable<L, R>(0));

template <typename L, typename R>
auto dereference_swappable(float) noexcept -> UTL_SCOPE false_type;
template <typename L, typename R>
auto dereference_swappable(int) noexcept -> UTL_SCOPE
    conjunction<UTL_SCOPE is_indirectly_readable<L>, UTL_SCOPE is_indirectly_readable<R>,
        UTL_SCOPE is_swappable_with<UTL_SCOPE iter_reference_t<L>, UTL_SCOPE iter_reference_t<R>>>;

template <typename L, typename R>
using is_dereference_swappable =
    decltype(UTL_SCOPE details::iterator_swap::dereference_swappable<L, R>(0));

template <typename L, typename R>
auto nothrow_dereference_swappable(float) noexcept -> UTL_SCOPE false_type;
template <typename L, typename R>
auto nothrow_dereference_swappable(int) noexcept
    -> UTL_SCOPE conjunction<UTL_SCOPE bool_constant<noexcept(*UTL_SCOPE declval<L&>()) &&
                                 noexcept(*UTL_SCOPE declval<R&>())>,
        UTL_SCOPE is_nothrow_swappable_with<UTL_SCOPE iter_reference_t<L>,
            UTL_SCOPE iter_reference_t<R>>>;

template <typename L, typename R>
using is_nothrow_dereference_swappable =
    decltype(UTL_SCOPE details::iterator_swap::nothrow_dereference_swappable<L, R>(0));

template <typename L, typename R>
UTL_SCOPE enable_if_t<is_dereference_swappable<L, R>::value> default_impl(
    L&& l, R&& r, unsigned int) noexcept(is_nothrow_dereference_swappable<L, R>::value) {
    UTL_SCOPE ranges::swap(*l, *r);
}

template <typename L, typename R>
UTL_SCOPE enable_if_t<is_exchange_swappable<L, R>::value> default_impl(
    L&& l, R&& r, unsigned short) noexcept(is_nothrow_exchange_swappable<L, R>::value) {
    *r = iter_exchange(UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r));
}

template <typename L, typename R>
int default_impl(L&&, R&&, ...) noexcept;

template <typename L, typename R>
using is_defaultable = UTL_SCOPE is_void<decltype(UTL_SCOPE details::iterator_swap::default_impl(
    UTL_SCOPE declval<L>(), UTL_SCOPE declval<R>(), 0u))>;

template <typename L, typename R>
using is_nothrow_defaultable = UTL_SCOPE bool_constant<is_defaultable<L, R>::value &&
    noexcept(UTL_SCOPE details::iterator_swap::default_impl(
        UTL_SCOPE declval<L>(), UTL_SCOPE declval<R>(), 0u))>;

template <typename L, typename R>
constexpr UTL_SCOPE enable_if_t<is_defaultable<L, R>::value> iter_swap(L&& l, R&& r) noexcept(
    is_nothrow_defaultable<L, R>::value) {
    return UTL_SCOPE details::iterator_swap::default_impl(
        UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r), 0u);
}

template <typename L, typename R>
auto nothrow_iter_swap(float) noexcept -> UTL_SCOPE false_type;

template <typename L, typename R>
auto nothrow_iter_swap(int) noexcept
    -> UTL_SCOPE bool_constant<noexcept(iter_swap(UTL_SCOPE declval<L>(), UTL_SCOPE declval<R>()))>;

template <typename L, typename R>
using is_nothrow_iter_swap = decltype(nothrow_iter_swap<L, R>(0));

template <typename L, typename R>
using iter_swappable =
    UTL_SCOPE void_t<decltype(iter_swap(UTL_SCOPE declval<L>(), UTL_SCOPE declval<R>()))>;

struct function_t {
    template <typename L, typename R>
    constexpr iter_swappable<L, R> operator()(L&& l, R&& r) const
        noexcept(is_nothrow_iter_swap<L, R>::value) {
        iter_swap(UTL_SCOPE forward<L>(l), UTL_SCOPE forward<R>(r));
    }
};
} // namespace iterator_swap
} // namespace details
UTL_NAMESPACE_END

#endif

UTL_NAMESPACE_BEGIN

namespace ranges {
inline namespace iterator {
UTL_DEFINE_CUSTOMIZATION_POINT(UTL_SCOPE details::iterator_swap::function_t, iter_swap);
} // namespace iterator
} // namespace ranges

UTL_NAMESPACE_END
