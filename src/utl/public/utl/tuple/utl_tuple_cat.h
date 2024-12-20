// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

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
__UTL_HIDE_FROM_ABI auto concat_elements_helper(
    index_sequence<Is...>, index_sequence<Js...>) noexcept
    -> __UTL tuple<tuple_element_t<Is, T0>..., tuple_element_t<Js, T1>...>;
template <typename T0, typename T1, size_t... Js>
__UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<>, index_sequence<Js...>) noexcept
    -> __UTL tuple<tuple_element_t<Js, T1>...>;

template <typename T0, typename T1, size_t... Is>
__UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<Is...>, index_sequence<>) noexcept
    -> __UTL tuple<tuple_element_t<Is, T0>...>;

template <typename T0, typename T1>
__UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<>, index_sequence<>) noexcept
    -> __UTL tuple<>;

template <typename T0, size_t... Is>
__UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<Is...>) noexcept
    -> __UTL tuple<tuple_element_t<Is, T0>...>;

template <typename T0>
__UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<>) noexcept -> __UTL tuple<>;

template <typename T0, typename T1>
__UTL_HIDE_FROM_ABI auto concat_elements_helper() noexcept
    -> decltype(concat_elements_helper<T0, T1>(
        tuple_index_sequence<T0>{}, tuple_index_sequence<T1>{}));

template <typename T0>
__UTL_HIDE_FROM_ABI auto concat_elements_helper() noexcept
    -> decltype(concat_elements_helper<T0>(tuple_index_sequence<T0>{}));

template <typename... Ts>
struct concat_elements<__UTL tuple<Ts...>> {
    using type UTL_NODEBUG = __UTL tuple<Ts...>;
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
__UTL_HIDE_FROM_ABI constexpr auto tuple_forward(Tuple&& tuple, index_sequence<Ns...>) noexcept(
    is_all_nothrow_gettable<Tuple>::value /* Only check the get expresions */
    /* We're constructing a tuple of references, so the construction should always be noexcept */
    ) -> __UTL tuple<get_type_t<Ns, Tuple>...> {
    return {__UTL get_element<Ns>(__UTL forward<Tuple>(tuple))...};
}

template <typename Tuple>
__UTL_HIDE_FROM_ABI constexpr auto tuple_forward(Tuple&& tuple) noexcept(
    noexcept(tuple_forward(declval<Tuple>(), tuple_index_sequence<Tuple>{})))
    -> decltype(tuple_forward(declval<Tuple>(), tuple_index_sequence<Tuple>{})) {
    return tuple_forward(forward<Tuple>(tuple), tuple_index_sequence<Tuple>{});
}

template <typename T>
__UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(T&& tuple) noexcept -> T&& {
    static_assert(is_all_reference<T>::value, "Elements must all be references");
    return __UTL forward<T>(tuple);
}

template <typename T0>
__UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(T0&& tuple0, __UTL tuple<> const& tuple1) noexcept
    -> T0&& {
    static_assert(is_all_reference<T0>::value, "Elements must all be references");
    return __UTL forward<T0>(tuple0);
}

template <typename T0>
__UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(__UTL tuple<> const& tuple1, T0&& tuple0) noexcept
    -> T0&& {
    static_assert(is_all_reference<T0>::value, "Elements must all be references");
    return __UTL forward<T0>(tuple0);
}

template <typename T0, typename T1, size_t... I0, size_t... I1>
__UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl_idx(T0&& tuple0, T1&& tuple1,
    index_sequence<I0...>, index_sequence<I1...>) noexcept(is_all_nothrow_gettable<T0>::value &&
    is_all_nothrow_gettable<T1>::value &&
    is_nothrow_constructible<concat_elements_t<T0, T1>, get_type_t<I0, T0>...,
        get_type_t<I1, T1>...>::value) -> concat_elements_t<T0, T1> {
    return {__UTL get_element<I0>(__UTL forward<T0>(tuple0))...,
        __UTL get_element<I1>(__UTL forward<T1>(tuple1))...};
}

template <typename T0, typename T1>
__UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(T0&& t0, T1&& t1) noexcept(
    noexcept(tuple_cat_impl_idx(declval<T0>(), declval<T1>(), tuple_index_sequence<T0>{},
        tuple_index_sequence<T1>{}))) -> concat_elements_t<T0, T1> {
    static_assert(conjunction<is_all_reference<T0>, is_all_reference<T1>>::value,
        "Elements must all be references");
    return tuple_cat_impl_idx(__UTL forward<T0>(t0), __UTL forward<T1>(t1),
        tuple_index_sequence<T0>{}, tuple_index_sequence<T1>{});
}

template <typename T0, typename T1, typename T2, typename... Tail>
__UTL_HIDE_FROM_ABI constexpr auto tuple_cat_impl(T0&& t0, T1&& t1, T2&& t2,
    Tail&&... tail) noexcept(noexcept(tuple_cat_impl(tuple_cat_impl(declval<T0>(), declval<T1>()),
    declval<T2>(), declval<Tail>()...))) -> concat_elements_t<T0, T1, T2, Tail...> {
    static_assert(conjunction<is_all_reference<T0>, is_all_reference<T1>, is_all_reference<T2>,
                      is_all_reference<Tail>...>::value,
        "Elements must all be references");
    return tuple_cat_impl(tuple_cat_impl(__UTL forward<T0>(t0), __UTL forward<T1>(t1)),
        __UTL forward<T2>(t2), forward<Tail>(tail)...);
}

} // namespace tuple
} // namespace details

template <typename... Tuples>
UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) constexpr auto tuple_cat(Tuples&&... args) noexcept(
    noexcept(details::tuple::tuple_cat_impl(details::tuple::tuple_forward(declval<Tuples>())...)))
    -> details::tuple::concat_elements_t<remove_reference_t<Tuples>...> {
    using return_type = details::tuple::concat_elements_t<remove_reference_t<Tuples>...>;
    return return_type(details::tuple::tuple_cat_impl(
        details::tuple::tuple_forward(__UTL forward<Tuples>(args))...));
}

UTL_NAMESPACE_END
