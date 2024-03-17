// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_implicitly_constructible.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_x_convertible.h"
#include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace constructible {
template <typename T, typename THead, typename TMid, typename... TTail>
auto explicit_test(int) -> decltype((
    implicit_conv<T>({declval<THead>(), declval<TMid>(), declval<TTail>()...}), false_type{}));
template <typename T, typename THead, typename TMid, typename... TTail>
auto explicit_test(float) -> is_constructible<T, THead, TMid, TTail...>;
template <typename T, typename THead>
auto explicit_test(int)
    -> conjunction<negation<is_convertible<THead, T>>, is_constructible<T, THead>>;
template <typename T>
auto explicit_test(int)
    -> conjunction<negation<decltype(implicit_test<T>(0))>, is_default_constructible<T>>;
template <typename T, typename...>
auto explicit_test(...) -> false_type;

template <typename TTarget, typename... TArgs>
using is_explicit = decltype(explicit_test<TTarget, TArgs...>(0));
} // namespace constructible
} // namespace details

template <typename TTarget, typename... TArgs>
struct is_explicit_constructible : details::constructible::is_explicit<TTarget, TArgs...> {};

template <typename TTarget, typename... TArgs>
struct is_nothrow_explicit_constructible :
    conjunction<is_explicit_constructible<TTarget, TArgs...>,
        is_nothrow_constructible<TTarget, TArgs...>> {};

UTL_NAMESPACE_END
