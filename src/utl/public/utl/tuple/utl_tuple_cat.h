// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/type_traits/utl_is_reference.h"

#if !defined(UTL_TUPLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#define TT_SCOPE UTL_SCOPE tuple_traits::

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

// unpacks into references
template <typename Tuple, size_t... Ns>
UTL_HIDE_FROM_ABI constexpr auto tuple_forward(Tuple&& tuple, index_sequence<Ns...>) noexcept(
    TT_SCOPE is_all_nothrow_gettable<Tuple>::value /* Only check the get expresions */
    /* We're constructing a tuple of references, so the construction should always be noexcept */
    ) -> UTL_SCOPE tuple<decltype(TT_SCOPE decl_element<Ns, Tuple>())...> {
    return {UTL_TUPLE_GET(Ns, forward<Tuple>(tuple))...};
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
    return forward<T>(tuple);
}

template <typename T0>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(
    T0&& tuple0, UTL_SCOPE tuple<> const& tuple1) noexcept -> T0&& {
    static_assert(is_all_reference<T0>::value, "Elements must all be references");
    return forward<T0>(tuple0);
}

template <typename T0>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(
    UTL_SCOPE tuple<> const& tuple1, T0&& tuple0) noexcept -> T0&& {
    static_assert(is_all_reference<T0>::value, "Elements must all be references");
    return forward<T0>(tuple0);
}

template <typename T0, typename T1, size_t... I0, size_t... I1>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl_idx(T0&& tuple0, T1&& tuple1, index_sequence<I0...>,
    index_sequence<I1...>) noexcept(TT_SCOPE is_all_nothrow_gettable<T0>::value &&
    TT_SCOPE is_all_nothrow_gettable<T1>::value &&
    is_nothrow_constructible<TT_SCOPE concat_elements_t<T0, T1>,
        decltype(UTL_TUPLE_GET(I0, declval<T0>()))...,
        decltype(UTL_TUPLE_GET(I1, declval<T1>()))...>::value)
    -> TT_SCOPE concat_elements_t<T0, T1> {
    return {UTL_TUPLE_GET(I0, forward<T0>(tuple0))..., UTL_TUPLE_GET(I1, forward<T1>(tuple1))...};
}

template <typename T0, typename T1>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(T0&& t0, T1&& t1) noexcept(
    noexcept(tuple_cat_impl_idx(declval<T0>(), declval<T1>(), tuple_index_sequence<T0>{},
        tuple_index_sequence<T1>{}))) -> TT_SCOPE concat_elements_t<T0, T1> {
    static_assert(conjunction<is_all_reference<T0>, is_all_reference<T1>>::value,
        "Elements must all be references");
    return tuple_cat_impl_idx(
        forward<T0>(t0), forward<T1>(t1), tuple_index_sequence<T0>{}, tuple_index_sequence<T1>{});
}

template <typename T0, typename T1, typename T2, typename... Tail>
UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(T0&& t0, T1&& t1, T2&& t2, Tail&&... tail) noexcept(
    noexcept(tuple_cat_impl(tuple_cat_impl(declval<T0>(), declval<T1>()), declval<T2>(),
        declval<Tail>()...))) -> TT_SCOPE concat_elements_t<T0, T1, T2, Tail...> {
    static_assert(conjunction<is_all_reference<T0>, is_all_reference<T1>, is_all_reference<T2>,
                      is_all_reference<Tail>...>::value,
        "Elements must all be references");
    return tuple_cat_impl(
        tuple_cat_impl(forward<T0>(t0), forward<T1>(t1)), forward<T2>(t2), forward<Tail>(tail)...);
}

} // namespace tuple
} // namespace details

template <typename... Tuples>
UTL_ATTRIBUTES(NODISCARD, FLATTEN, HIDE_FROM_ABI) constexpr auto tuple_cat(Tuples&&... args) noexcept(
    noexcept(details::tuple::tuple_cat_impl(details::tuple::tuple_forward(declval<Tuples>())...)))
    -> TT_SCOPE concat_elements_t<remove_reference_t<Tuples>...> {
    using return_type = TT_SCOPE concat_elements_t<remove_reference_t<Tuples>...>;
    return return_type(
        details::tuple::tuple_cat_impl(details::tuple::tuple_forward(forward<Tuples>(args))...));
}

UTL_NAMESPACE_END

#undef TT_SCOPE