// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_attributes.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_modify_x_cv.h"

/** TODO: This needs a rewrite and probably a rename **/

UTL_NAMESPACE_BEGIN

namespace details {
namespace accumulate_qualifiers {

template<template<typename> class copier>
struct applier {
    template<typename T>
    using apply = typename copier<T>::type;
};

enum class qualifier : int {
    none = 0,
    c = 1,
    v = 2,
    cv = 3
};

UTL_ATTRIBUTE(NODISCARD)
UTL_CONSTEVAL_CXX20 qualifier operator|(qualifier l, qualifier r) noexcept {
    return qualifier((int)l | (int)r);
}

UTL_ATTRIBUTE(NODISCARD)
UTL_CONSTEVAL_CXX20 qualifier operator&(qualifier l, qualifier r) noexcept {
    return qualifier((int)l & (int)r);
}

UTL_ATTRIBUTE(NODISCARD)
UTL_CONSTEVAL_CXX20 qualifier operator*(bool l, qualifier r) noexcept {
    return l ? r : qualifier::none;
}

template<typename T>
struct qualifier_constant : value_constant<qualifier,
    ((is_const<T>::value * qualifier::c) | (is_volatile<T>::value * qualifier::v))> {};

template<qualifier Q, typename... Ts>
struct accumulate_cv_impl;

template<qualifier Q, typename T>
struct accumulate_cv_impl<Q, T> : value_constant<qualifier, Q | qualifier_constant<T>::value> {};

template<qualifier Q, typename Head, typename... Tail>
struct accumulate_cv_impl<Q, Head, Tail...> : conditional_t<
    accumulate_cv_impl<Q, Head>::value == qualifier::cv, 
    accumulate_cv_impl<Q, Head>,
    accumulate_cv_impl<accumulate_cv_impl<Q, Head>::value, Tail...>
> {};

template<typename... Ts>
using accumulate_cv = accumulate_cv_impl<qualifier::none, Ts...>;

template<qualifier>
struct cv_applier : applier<type_identity> {};
template<>
struct cv_applier<qualifier::c> : applier<add_const> {};
template<>
struct cv_applier<qualifier::v> : applier<add_volatile> {};
template<>
struct cv_applier<qualifier::cv> : applier<add_cv> {};

template<qualifier>
struct c_applier : applier<type_identity> {};
template<>
struct c_applier<qualifier::c> : applier<add_const> {};
template<>
struct c_applier<qualifier::cv> : applier<add_const> {};

template<qualifier>
struct v_applier : applier<type_identity> {};
template<>
struct v_applier<qualifier::v> : applier<add_volatile> {};
template<>
struct v_applier<qualifier::cv> : applier<add_volatile> {};

template<typename... Ts>
using const_impl = c_applier<accumulate_cv<Ts...>::value>;

template<typename... Ts>
using volatile_impl = v_applier<accumulate_cv<Ts...>::value>;

template<typename... Ts>
using cv_impl = cv_applier<accumulate_cv<Ts...>::value>;
}   // namespace accumulate_qualifiers
}   // namespace details

template<typename... Ts>
struct combine_const : details::accumulate_qualifiers::const_impl<Ts...> {};
template<typename... Ts>
struct combine_volatile : details::accumulate_qualifiers::volatile_impl<Ts...> {};
template<typename... Ts>
struct combine_cv : details::accumulate_qualifiers::cv_impl<Ts...> {};

UTL_NAMESPACE_END
