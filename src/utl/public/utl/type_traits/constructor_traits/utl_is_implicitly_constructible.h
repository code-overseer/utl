// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/constructor_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_x_convertible.h"
#include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace constructible {

template <typename T>
void implicit_conv(T) noexcept;

template <typename T, typename THead, typename... TTail>
auto implicit_test(float) noexcept
    -> decltype((implicit_conv<T>({declval<THead>(), declval<TTail>()...}),
        is_constructible<T, THead, TTail...>{}));
template <typename T, typename THead>
auto implicit_test(int) noexcept
    -> conjunction<is_convertible<THead, T>, is_constructible<T, THead>>;
template <typename T>
auto implicit_test(int) noexcept -> decltype((implicit_conv<T>({}), is_default_constructible<T>{}));
template <typename T, typename...>
auto implicit_test(...) noexcept -> false_type;

template <typename TTarget, typename... TArgs>
using is_implicit = decltype(implicit_test<TTarget, TArgs...>(0));

} // namespace constructible
} // namespace details

template <typename TTarget, typename... TArgs>
struct is_implicit_constructible : details::constructible::is_implicit<TTarget, TArgs...> {};

template <typename TTarget, typename... TArgs>
struct is_nothrow_implicit_constructible :
    conjunction<is_implicit_constructible<TTarget, TArgs...>,
        is_nothrow_constructible<TTarget, TArgs...>> {};

UTL_NAMESPACE_END
