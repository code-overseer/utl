// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_indirectly_readable.h"
#include "utl/iterator/utl_iter_swap.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_logical_traits.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename L, typename R = L>
concept indirectly_swappable =
    indirectly_readable<L> && indirectly_readable<R> && requires(L const l, R const r) {
        __UTL ranges::iter_swap(l, l);
        __UTL ranges::iter_swap(l, r);
        __UTL ranges::iter_swap(r, l);
        __UTL ranges::iter_swap(r, r);
    };

template <typename L, typename R>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_swappable : bool_constant<indirectly_swappable<L, R>> {};

template <typename L, typename R>
inline constexpr bool is_indirectly_swappable_v = indirectly_swappable<L, R>;

UTL_NAMESPACE_END

#else

UTL_NAMESPACE_BEGIN
namespace details {
namespace indirectly_swappable {

template <typename L, typename R>
__UTL_HIDE_FROM_ABI auto trait_impl(float) noexcept -> __UTL false_type;
template <typename L, typename R>
__UTL_HIDE_FROM_ABI auto trait_impl(int) noexcept
    -> __UTL conjunction<__UTL is_indirectly_readable<L>, __UTL is_indirectly_readable<R>,
        __UTL is_invocable<decltype(__UTL ranges::iter_swap), L const, R const>>;

template <typename L, typename R>
using trait = decltype(__UTL details::indirectly_swappable::trait_impl<L, R>(0));

} // namespace indirectly_swappable
} // namespace details

template <typename L, typename R = L>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_swappable :
    details::indirectly_swappable::trait<L, R> {};

#  if UTL_CXX14
template <typename L, typename R = L>
UTL_INLINE_CXX17 constexpr bool is_indirectly_swappable_v = details::indirectly_swappable::trait<L, R>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
