// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN
template <bool B, typename T, typename> struct conditional {
    using type = T;
};
template <typename T0, typename T1> struct conditional<false, T0, T1> {
    using type = T1;
};

template <bool V, typename T, typename F> using conditional_t = typename conditional<V, T, F>::type;

template <typename...> struct disjunction : false_type {};
template <typename T> struct disjunction<T> : conditional_t<T::value, true_type, false_type> {};
template <typename Head, typename... Tail>
struct disjunction<Head, Tail...> : conditional_t<Head::value, true_type, disjunction<Tail...>> {};

template <typename...> struct conjunction : true_type {};
template <typename T> struct conjunction<T> : conditional_t<T::value, true_type, false_type> {};
template <typename Head, typename... Tail>
struct conjunction<Head, Tail...> : conditional_t<Head::value, conjunction<Tail...>, false_type> {};

template <typename T> struct negation : bool_constant<!T::value> {};

#ifdef UTL_CXX14
template <typename... Ts> UTL_INLINE_CXX17 constexpr bool disjunction_v = disjunction<Ts...>::value;
template <typename... Ts> UTL_INLINE_CXX17 constexpr bool conjunction_v = conjunction<Ts...>::value;
template <typename T> UTL_INLINE_CXX17 constexpr bool     negation_v = !T::value;
#endif

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_conditional 1
#define UTL_TRAIT_SUPPORTED_disjunction 1
#define UTL_TRAIT_SUPPORTED_conjunction 1
#define UTL_TRAIT_SUPPORTED_negation 1
