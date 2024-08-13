// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

template <typename M>
struct member_traits;

template <typename Type, typename Class>
struct member_traits<Type Class::*> {
    using type = Type;
    using value_type = Type Class::*;
    using owner_type = Class;
};

template <typename M, M Pointer>
struct member_info;

template <typename Type, typename Class, Type Class::*V>
struct member_info<Type Class::*, V> : member_traits<Type Class::*> {
    using member_traits::owner_type;
    using member_traits::type;
    using member_traits::value_type;
    static constexpr value_type value = V;

    template <typename T, typename... Args,
        typename = enable_if_t<is_base_of<Class, decay_t<T>>::value && is_function<type>::value>>
    constexpr decltype((declval<copy_cvref_t<T, Class>>()().*value)(declval<Args>()...)) operator()(
        T&& instance, Args&&... args) const {
        return (static_cast<copy_cvref_t<T&&, Class>>(forward<T>(instance))->*value)(
            forward<Args>(args)...);
    }

    template <typename T,
        typename = enable_if_t<is_convertible<T&&, copy_cvref_t<T&&, Class>>::value &&
            !is_function<type>::value>>
    constexpr copy_cvref_t<T&&, type> operator()(T&& instance) const {
        return static_cast<copy_cvref_t<T&&, Class>>(forward<T>(instance)).*value;
    }
};

template <typename M, M Pointer>
using member_info_t = typename member_info<M, Pointer>::type;

#ifndef UTL_CXX17

#  define UTL_MEMBER_INFO(CLASS, MEMBER) __UTL member_info<decltype(&CLASS::MEMBER), &CLASS::MEMBER>

#else // ifndef UTL_CXX17

template <auto V>
using member_info_of = member_info<decltype(V), V>;
template <auto V>
using member_info_of_t = typename member_info<decltype(V), V>::type;

#  define UTL_MEMBER_INFO(CLASS, MEMBER) __UTL member_info_of<&CLASS::MEMBER>

#endif // ifndef UTL_CXX17

UTL_NAMESPACE_END
