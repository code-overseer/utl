// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if !defined(UTL_TUPLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/tuple/utl_tuple_details.h"

UTL_NAMESPACE_BEGIN

template <size_t I, typename... U UTL_CONSTRAINT_CXX11((I < sizeof...(U)))>
UTL_CONSTRAINT_CXX20(I < sizeof...(U))
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) UTL_CONSTEXPR_CXX14 auto get(
    tuple<U...>&& target UTL_LIFETIMEBOUND) noexcept -> tuple_element_t<I, tuple<U...>>&& {
    static_assert(is_base_of<details::tuple::storage<U...>, tuple<U...>>::value, "Invalid tuple");
    return ((details::tuple::storage<U...>&&)__UTL move(target)).template get<I>();
}

template <size_t I, typename... U UTL_CONSTRAINT_CXX11((I < sizeof...(U)))>
UTL_CONSTRAINT_CXX20(I < sizeof...(U))
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) UTL_CONSTEXPR_CXX14 auto get(
    tuple<U...>& target UTL_LIFETIMEBOUND) noexcept -> tuple_element_t<I, tuple<U...>>& {
    static_assert(is_base_of<details::tuple::storage<U...>, tuple<U...>>::value, "Invalid tuple");
    return ((details::tuple::storage<U...>&)target).template get<I>();
}

template <size_t I, typename... U UTL_CONSTRAINT_CXX11((I < sizeof...(U)))>
UTL_CONSTRAINT_CXX20(I < sizeof...(U))
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get(tuple<U...> const&& target
        UTL_LIFETIMEBOUND) noexcept -> tuple_element_t<I, tuple<U...>> const&& {
    static_assert(is_base_of<details::tuple::storage<U...>, tuple<U...>>::value, "Invalid tuple");
    return ((details::tuple::storage<U...> const&&)__UTL move(target)).template get<I>();
}

template <size_t I, typename... U UTL_CONSTRAINT_CXX11((I < sizeof...(U)))>
UTL_CONSTRAINT_CXX20(I < sizeof...(U))
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get(tuple<U...> const& target
        UTL_LIFETIMEBOUND) noexcept -> tuple_element_t<I, tuple<U...>> const& {
    static_assert(is_base_of<details::tuple::storage<U...>, tuple<U...>>::value, "Invalid tuple");
    return ((details::tuple::storage<U...> const&)target).template get<I>();
}

template <typename T, typename... U>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI)
UTL_CONSTEXPR_CXX14 auto get(tuple<U...>&& target UTL_LIFETIMEBOUND) noexcept -> enable_if_t<template_count<T, tuple<U...>>::value == 1,
    decltype(get<template_index<T, tuple<U...>>::value>(__UTL move(target)))> {
    return get<template_index<T, tuple<U...>>::value>(__UTL move(target));
}

template <typename T, typename... U>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI)
UTL_CONSTEXPR_CXX14 auto get(tuple<U...>& target UTL_LIFETIMEBOUND) noexcept -> enable_if_t<template_count<T, tuple<U...>>::value == 1,
    decltype(get<template_index<T, tuple<U...>>::value>(target))> {
    return get<template_index<T, tuple<U...>>::value>(target);
}

template <typename T, typename... U>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get(tuple<U...> const& target UTL_LIFETIMEBOUND) noexcept -> enable_if_t<template_count<T, tuple<U...>>::value == 1,
    decltype(get<template_index<T, tuple<U...>>::value>(target))> {
    return get<template_index<T, tuple<U...>>::value>(target);
}

template <typename T, typename... U>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get(tuple<U...> const&& target UTL_LIFETIMEBOUND) noexcept -> enable_if_t<template_count<T, tuple<U...>>::value == 1,
    decltype(get<template_index<T, tuple<U...>>::value>(__UTL move(target)))> {
    return get<template_index<T, tuple<U...>>::value>(__UTL move(target));
}

UTL_NAMESPACE_END
