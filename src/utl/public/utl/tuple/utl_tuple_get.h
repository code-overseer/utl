// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

#if !defined(UTL_TUPLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#define TT_SCOPE UTL_SCOPE tuple_traits::

UTL_NAMESPACE_BEGIN

template <size_t I, typename... U>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)
UTL_CONSTEXPR_CXX14 auto get(tuple<U...>&& target UTL_LIFETIMEBOUND) noexcept -> decltype(TT_SCOPE decl_element<I, decltype(target)>()) {
    static_assert(is_base_of<details::tuple::storage<U...>, tuple<U...>>::value, "Invalid tuple");
    return ((details::tuple::storage<U...>&&)move(target)).template get<I>();
}

template <size_t I, typename... U, typename = enable_if_t<(I < sizeof...(U))>>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)
UTL_CONSTEXPR_CXX14 auto get(tuple<U...>& target UTL_LIFETIMEBOUND) noexcept -> decltype(TT_SCOPE decl_element<I, decltype(target)>()) {
    static_assert(is_base_of<details::tuple::storage<U...>, tuple<U...>>::value, "Invalid tuple");
    return ((details::tuple::storage<U...>&)target).template get<I>();
}

template <size_t I, typename... U, typename = enable_if_t<(I < sizeof...(U))>>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get(tuple<U...> const&& target UTL_LIFETIMEBOUND) noexcept -> decltype(TT_SCOPE decl_element<I, decltype(target)>()) {
    static_assert(is_base_of<details::tuple::storage<U...>, tuple<U...>>::value, "Invalid tuple");
    return ((details::tuple::storage<U...> const&&)move(target)).template get<I>();
}

template <size_t I, typename... U, typename = enable_if_t<(I < sizeof...(U))>>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get(tuple<U...> const& target UTL_LIFETIMEBOUND) noexcept -> decltype(TT_SCOPE decl_element<I, decltype(target)>()) {
    static_assert(is_base_of<details::tuple::storage<U...>, tuple<U...>>::value, "Invalid tuple");
    return ((details::tuple::storage<U...> const&)target).template get<I>();
}

template <typename T, typename... U>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)
UTL_CONSTEXPR_CXX14 auto get(tuple<U...>&& target UTL_LIFETIMEBOUND) noexcept -> enable_if_t<template_count<T, tuple<U...>>::value == 1,
    decltype(get<template_index<T, tuple<U...>>::value>(move(target)))> {
    return get<template_index<T, tuple<U...>>::value>(move(target));
}

template <typename T, typename... U>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)
UTL_CONSTEXPR_CXX14 auto get(tuple<U...>& target UTL_LIFETIMEBOUND) noexcept -> enable_if_t<template_count<T, tuple<U...>>::value == 1,
    decltype(get<template_index<T, tuple<U...>>::value>(target))> {
    return get<template_index<T, tuple<U...>>::value>(target);
}

template <typename T, typename... U>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get(tuple<U...> const& target UTL_LIFETIMEBOUND) noexcept -> enable_if_t<template_count<T, tuple<U...>>::value == 1,
    decltype(get<template_index<T, tuple<U...>>::value>(target))> {
    return get<template_index<T, tuple<U...>>::value>(target);
}

template <typename T, typename... U>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get(tuple<U...> const&& target UTL_LIFETIMEBOUND) noexcept -> enable_if_t<template_count<T, tuple<U...>>::value == 1,
    decltype(get<template_index<T, tuple<U...>>::value>(move(target)))> {
    return get<template_index<T, tuple<U...>>::value>(move(target));
}

UTL_NAMESPACE_END

#undef TT_SCOPE
