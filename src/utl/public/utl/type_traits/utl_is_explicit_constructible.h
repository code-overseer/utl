// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_implicit_constructible.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace constructible {
template <typename T, typename THead, typename TMid, typename... TTail>
UTL_HIDE_FROM_ABI auto explicit_test(int) noexcept -> decltype((
    implicit_conv<T>({declval<THead>(), declval<TMid>(), declval<TTail>()...}), false_type{}));
template <typename T, typename THead, typename TMid, typename... TTail>
UTL_HIDE_FROM_ABI auto explicit_test(float) noexcept -> is_constructible<T, THead, TMid, TTail...>;
template <typename T, typename THead>
UTL_HIDE_FROM_ABI auto explicit_test(int) noexcept
    -> conjunction<negation<is_convertible<THead, T>>, is_constructible<T, THead>>;
template <typename T>
UTL_HIDE_FROM_ABI auto explicit_test(int) noexcept
    -> conjunction<negation<decltype(implicit_test<T>(0))>, is_default_constructible<T>>;
template <typename T, typename...>
UTL_HIDE_FROM_ABI auto explicit_test(...) noexcept -> false_type;

template <typename TTarget, typename... TArgs>
using is_explicit UTL_NODEBUG = decltype(explicit_test<TTarget, TArgs...>(0));
} // namespace constructible
} // namespace details

template <typename TTarget, typename... TArgs>
struct __UTL_PUBLIC_TEMPLATE is_explicit_constructible :
    details::constructible::is_explicit<TTarget, TArgs...> {};

template <typename TTarget, typename... TArgs>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_explicit_constructible :
    conjunction<is_explicit_constructible<TTarget, TArgs...>,
        is_nothrow_constructible<TTarget, TArgs...>> {};

#if UTL_CXX14
template <typename TTarget, typename... TArgs>
UTL_INLINE_CXX17 constexpr bool is_explicit_constructible_v =
    details::constructible::is_explicit<TTarget, TArgs...>::value;

template <typename TTarget, typename... TArgs>
UTL_INLINE_CXX17 constexpr bool is_nothrow_explicit_constructible_v =
    is_nothrow_explicit_constructible<TTarget, TArgs...>::value;
#endif

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_explicit_constructible(...) \
      UTL_SCOPE is_explicit_constructible_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_explicit_constructible(...) \
      UTL_SCOPE is_explicit_constructible<__VA_ARGS__>::value
#endif

#if UTL_CXX14
#  define UTL_TRAIT_is_nothrow_explicit_constructible(...) \
      UTL_SCOPE is_nothrow_explicit_constructible_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_nothrow_explicit_constructible(...) \
      UTL_SCOPE is_nothrow_explicit_constructible<__VA_ARGS__>::value
#endif
