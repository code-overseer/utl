// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/tuple/utl_tuple_get_element.h"
#include "utl/type_traits/utl_is_reference.h"

#if !defined(UTL_TUPLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {

template <typename, typename>
struct is_all_reference_impl;
template <typename T, size_t... Is>
struct is_all_reference_impl<T, index_sequence<Is...>> :
    conjunction<is_reference<tuple_element_t<Is, T>>...> {};
template <typename T>
using is_all_reference UTL_NODEBUG = is_all_reference_impl<T, tuple_index_sequence<T>>;

template <typename... Ts>
struct concat_elements;

template <typename... Ts>
using concat_elements_t = typename concat_elements<Ts...>::type;

template <typename T0, typename T1, size_t... Is, size_t... Js>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<Is...>, index_sequence<Js...>) noexcept
    -> UTL_SCOPE tuple<tuple_element_t<Is, T0>..., tuple_element_t<Js, T1>...>;
template <typename T0, typename T1, size_t... Js>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<>, index_sequence<Js...>) noexcept
    -> UTL_SCOPE tuple<tuple_element_t<Js, T1>...>;

template <typename T0, typename T1, size_t... Is>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<Is...>, index_sequence<>) noexcept
    -> UTL_SCOPE tuple<tuple_element_t<Is, T0>...>;

template <typename T0, typename T1>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<>, index_sequence<>) noexcept
    -> UTL_SCOPE tuple<>;

template <typename T0, size_t... Is>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<Is...>) noexcept
    -> UTL_SCOPE tuple<tuple_element_t<Is, T0>...>;

template <typename T0>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<>) noexcept -> UTL_SCOPE tuple<>;

template <typename T0, typename T1>
UTL_HIDE_FROM_ABI auto concat_elements_helper() noexcept -> decltype(concat_elements_helper<T0, T1>(
    tuple_index_sequence<T0>{}, tuple_index_sequence<T1>{}));

template <typename T0>
UTL_HIDE_FROM_ABI auto concat_elements_helper() noexcept
    -> decltype(concat_elements_helper<T0>(tuple_index_sequence<T0>{}));

template <typename... Ts>
struct concat_elements<UTL_SCOPE tuple<Ts...>> {
    using type UTL_NODEBUG = UTL_SCOPE tuple<Ts...>;
};

template <typename T0>
struct concat_elements<T0> {
    static_assert(is_tuple_like<T0>::value, "Only tuple_like types can be concatenated");
    using type UTL_NODEBUG = decltype(concat_elements_helper<T0>());
};

template <typename T0, typename T1>
struct concat_elements<T0, T1> {
    static_assert(is_tuple_like<T0>::value, "Only tuple_like types can be concatenated");
    static_assert(is_tuple_like<T1>::value, "Only tuple_like types can be concatenated");
    using type UTL_NODEBUG = decltype(concat_elements_helper<T0, T1>());
};

template <typename T0, typename T1, typename... Ts>
struct concat_elements<T0, T1, Ts...> : concat_elements<concat_elements_t<T0, T1>, Ts...> {};

// unpacks into references
template <typename Tuple, size_t... Ns>
UTL_HIDE_FROM_ABI constexpr auto tuple_forward(Tuple&& tuple, index_sequence<Ns...>) noexcept(
    is_all_nothrow_gettable<Tuple>::value /* Only check the get expresions */
    /* We're constructing a tuple of references, so the construction should always be noexcept */
    ) -> UTL_SCOPE tuple<get_type_t<Ns, Tuple>...> {
    return {UTL_SCOPE get_element<Ns>(UTL_SCOPE forward<Tuple>(tuple))...};
}

template <typename Tuple>
UTL_HIDE_FROM_ABI constexpr auto tuple_forward(Tuple&& tuple) noexcept(
    noexcept(tuple_forward(declval<Tuple>(), tuple_index_sequence<Tuple>{})))
    -> decltype(tuple_forward(declval<Tuple>(), tuple_index_sequence<Tuple>{})) {
    return tuple_forward(forward<Tuple>(tuple), tuple_index_sequence<Tuple>{});
}

template <typename T>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(T&& tuple) noexcept -> T&& {
    static_assert(is_all_reference<T>::value, "Elements must all be references");
    return UTL_SCOPE forward<T>(tuple);
}

template <typename T0>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(
    T0&& tuple0, UTL_SCOPE tuple<> const& tuple1) noexcept -> T0&& {
    static_assert(is_all_reference<T0>::value, "Elements must all be references");
    return UTL_SCOPE forward<T0>(tuple0);
}

template <typename T0>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(
    UTL_SCOPE tuple<> const& tuple1, T0&& tuple0) noexcept -> T0&& {
    static_assert(is_all_reference<T0>::value, "Elements must all be references");
    return UTL_SCOPE forward<T0>(tuple0);
}

template <typename T0, typename T1, size_t... I0, size_t... I1>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl_idx(T0&& tuple0, T1&& tuple1, index_sequence<I0...>,
    index_sequence<I1...>) noexcept(is_all_nothrow_gettable<T0>::value &&
    is_all_nothrow_gettable<T1>::value &&
    is_nothrow_constructible<concat_elements_t<T0, T1>, get_type_t<I0, T0>...,
        get_type_t<I1, T1>...>::value) -> concat_elements_t<T0, T1> {
    return {UTL_SCOPE get_element<I0>(UTL_SCOPE forward<T0>(tuple0))...,
        UTL_SCOPE get_element<I1>(UTL_SCOPE forward<T1>(tuple1))...};
}

template <typename T0, typename T1>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(T0&& t0, T1&& t1) noexcept(
    noexcept(tuple_cat_impl_idx(declval<T0>(), declval<T1>(), tuple_index_sequence<T0>{},
        tuple_index_sequence<T1>{}))) -> concat_elements_t<T0, T1> {
    static_assert(conjunction<is_all_reference<T0>, is_all_reference<T1>>::value,
        "Elements must all be references");
    return tuple_cat_impl_idx(UTL_SCOPE forward<T0>(t0), UTL_SCOPE forward<T1>(t1),
        tuple_index_sequence<T0>{}, tuple_index_sequence<T1>{});
}

template <typename T0, typename T1, typename T2, typename... Tail>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(T0&& t0, T1&& t1, T2&& t2, Tail&&... tail) noexcept(
    noexcept(tuple_cat_impl(tuple_cat_impl(declval<T0>(), declval<T1>()), declval<T2>(),
        declval<Tail>()...))) -> concat_elements_t<T0, T1, T2, Tail...> {
    static_assert(conjunction<is_all_reference<T0>, is_all_reference<T1>, is_all_reference<T2>,
                      is_all_reference<Tail>...>::value,
        "Elements must all be references");
    return tuple_cat_impl(tuple_cat_impl(UTL_SCOPE forward<T0>(t0), UTL_SCOPE forward<T1>(t1)),
        UTL_SCOPE forward<T2>(t2), forward<Tail>(tail)...);
}

} // namespace tuple
} // namespace details

template <typename... Tuples>
UTL_ATTRIBUTES(NODISCARD, FLATTEN, HIDE_FROM_ABI) constexpr auto tuple_cat(Tuples&&... args) noexcept(
    noexcept(details::tuple::tuple_cat_impl(details::tuple::tuple_forward(declval<Tuples>())...)))
    -> details::tuple::concat_elements_t<remove_reference_t<Tuples>...> {
    using return_type = details::tuple::concat_elements_t<remove_reference_t<Tuples>...>;
    return return_type(details::tuple::tuple_cat_impl(
        details::tuple::tuple_forward(UTL_SCOPE forward<Tuples>(args))...));
}

UTL_NAMESPACE_END
