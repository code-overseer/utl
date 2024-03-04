// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/type_traits/utl_std_traits.h"

UTL_NAMESPACE_BEGIN

namespace constructible {
namespace details {
template <typename T, typename THead, typename... TTail>
auto implicit_test(float) -> decltype((declval<void(T)>()({declval<THead>(), declval<TTail>()...}),
    is_constructible<T, THead, TTail...>{}));
template <typename T, typename THead>
auto implicit_test(int) -> conjunction<is_convertible<THead, T>, is_constructible<T, THead>>;
template <typename T>
auto implicit_test(int) -> decltype((declval<void(T)>()({}), is_default_constructible<T>{}));
template <typename T, typename...> auto implicit_test(...) -> false_type;

template <typename TTarget, typename... TArgs>
using is_implicit = decltype(implicit_test<TTarget, TArgs...>(0));

template <typename T, typename THead, typename TMid, typename... TTail>
auto explicit_test(int) -> decltype((
    declval<void(T)>()({declval<THead>(), declval<TMid>(), declval<TTail>()...}), false_type{}));
template <typename T, typename THead, typename TMid, typename... TTail>
auto explicit_test(float)
    -> decltype((declval<void(T)>()(T{declval<THead>(), declval<TMid>(), declval<TTail>()...}),
        is_constructible<T, THead, TMid, TTail...>{}));
template <typename T, typename THead>
auto explicit_test(int)
    -> conjunction<negation<is_convertible<THead, T>>, is_constructible<T, THead>>;
template <typename T>
auto explicit_test(int) -> conjunction<negation<decltype(implicit_test<T>(0))>,
    decltype((declval<void(T)>()(T{}), is_default_constructible<T>{}))>;
template <typename T, typename...> auto explicit_test(...) -> false_type;

template <typename TTarget, typename... TArgs>
using is_explicit = decltype(explicit_test<TTarget, TArgs...>(0));
} // namespace details
} // namespace constructible

template <typename TTarget, typename... TArgs>
struct is_implicit_constructible : constructible::details::is_implicit<TTarget, TArgs...> {};

template <typename TTarget, typename... TArgs>
struct is_nothrow_implicit_constructible :
    conjunction<is_implicit_constructible<TTarget, TArgs...>,
        is_nothrow_constructible<TTarget, TArgs...>> {};

template <typename TTarget, typename... TArgs>
struct is_explicit_constructible : constructible::details::is_explicit<TTarget, TArgs...> {};

template <typename TTarget, typename... TArgs>
struct is_nothrow_explicit_constructible :
    conjunction<is_explicit_constructible<TTarget, TArgs...>,
        is_nothrow_constructible<TTarget, TArgs...>> {};

UTL_NAMESPACE_END
