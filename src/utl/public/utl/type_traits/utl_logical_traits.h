// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN
template <bool B, typename T, typename>
struct __UTL_PUBLIC_TEMPLATE conditional {
    using type UTL_NODEBUG = T;
};
template <typename T0, typename T1>
struct __UTL_PUBLIC_TEMPLATE conditional<false, T0, T1> {
    using type UTL_NODEBUG = T1;
};

template <bool V, typename T, typename F>
using conditional_t = typename conditional<V, T, F>::type;

template <typename...>
struct __UTL_PUBLIC_TEMPLATE disjunction : false_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE disjunction<T> : conditional_t<T::value, true_type, false_type> {};
template <typename Head, typename... Tail>
struct __UTL_PUBLIC_TEMPLATE disjunction<Head, Tail...> :
    conditional_t<Head::value, true_type, disjunction<Tail...>> {};

template <typename...>
struct __UTL_PUBLIC_TEMPLATE conjunction : true_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE conjunction<T> : conditional_t<T::value, true_type, false_type> {};
template <typename Head, typename... Tail>
struct __UTL_PUBLIC_TEMPLATE conjunction<Head, Tail...> :
    conditional_t<Head::value, conjunction<Tail...>, false_type> {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE negation : bool_constant<!T::value> {};

#if UTL_CXX14
template <typename... Ts>
UTL_INLINE_CXX17 constexpr bool disjunction_v = disjunction<Ts...>::value;
template <typename... Ts>
UTL_INLINE_CXX17 constexpr bool conjunction_v = conjunction<Ts...>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool negation_v = !T::value;
#endif

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_conditional 1
#define UTL_TRAIT_SUPPORTED_disjunction 1
#define UTL_TRAIT_SUPPORTED_conjunction 1
#define UTL_TRAIT_SUPPORTED_negation 1

#if UTL_CXX14
#  define UTL_TRAIT_conjunction(...) __UTL conjunction_v<__VA_ARGS__>
#  define UTL_TRAIT_disjunction(...) __UTL disjunction_v<__VA_ARGS__>
#  define UTL_TRAIT_negation(...) __UTL negation_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_conjunction(...) __UTL conjunction<__VA_ARGS__>::value
#  define UTL_TRAIT_disjunction(...) __UTL disjunction<__VA_ARGS__>::value
#  define UTL_TRAIT_negation(...) __UTL negation<__VA_ARGS__>::value
#endif
