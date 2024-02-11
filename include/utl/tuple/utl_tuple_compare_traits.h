// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/type_traits/utl_boolean_testable.h"
#include "utl/compare/utl_compare_traits.h"
#include "utl/type_traits/utl_std_traits.h"
#include "utl/tuple/utl_tuple_traits.h"
#include "utl/utility/utl_sequence.h"

#define TT_SCOPE UTL_SCOPE tuple_traits::

UTL_NAMESPACE_BEGIN

namespace compare_ops {
namespace details {

template<typename T>
using const_ref_t = const remove_reference_t<T>&;
template<typename T>
using sequence_t = conditional_t<TT_SCOPE is_tuple_like<T>::value, tuple_index_sequence<T>, void>;

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_eq_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_eq_impl<T, U, index_sequence<Is...>, enable_if_t<
        conjunction<
            TT_SCOPE is_tuple_like<T>,
            TT_SCOPE is_tuple_like<U>,
            bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
            has_eq<
                const_ref_t<tuple_element_t<Is, T>>,
                const_ref_t<tuple_element_t<Is, U>>
            >...
        >::value
    >   // enable_if_t
> : true_type {};

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_neq_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_neq_impl<T, U, index_sequence<Is...>, enable_if_t<
        conjunction<
            TT_SCOPE is_tuple_like<T>,
            TT_SCOPE is_tuple_like<U>,
            bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
            has_neq<
                const_ref_t<tuple_element_t<Is, T>>,
                const_ref_t<tuple_element_t<Is, U>>
            >...
        >::value
    >   // enable_if_t
> : true_type {};

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_lt_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_lt_impl<T, U, index_sequence<Is...>, enable_if_t<
        conjunction<
            TT_SCOPE is_tuple_like<T>,
            TT_SCOPE is_tuple_like<U>,
            bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
            has_lt<
                const_ref_t<tuple_element_t<Is, T>>,
                const_ref_t<tuple_element_t<Is, U>>
            >...
        >::value
    >   // enable_if_t
> : true_type {};

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_gt_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_gt_impl<T, U, index_sequence<Is...>, enable_if_t<
        conjunction<
            TT_SCOPE is_tuple_like<T>,
            TT_SCOPE is_tuple_like<U>,
            bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
            has_gt<
                const_ref_t<tuple_element_t<Is, T>>,
                const_ref_t<tuple_element_t<Is, U>>
            >...
        >::value
    >   // enable_if_t
> : true_type {};

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_lteq_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_lteq_impl<T, U, index_sequence<Is...>, enable_if_t<
        conjunction<
            TT_SCOPE is_tuple_like<T>,
            TT_SCOPE is_tuple_like<U>,
            bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
            has_lteq<
                const_ref_t<tuple_element_t<Is, T>>,
                const_ref_t<tuple_element_t<Is, U>>
            >...
        >::value
    >   // enable_if_t
> : true_type {};

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_gteq_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_gteq_impl<T, U, index_sequence<Is...>, enable_if_t<
        conjunction<
            TT_SCOPE is_tuple_like<T>,
            TT_SCOPE is_tuple_like<U>,
            bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
            has_gteq<
                const_ref_t<tuple_element_t<Is, T>>,
                const_ref_t<tuple_element_t<Is, U>>
            >...
        >::value
    >   // enable_if_t
> : true_type {};
}    // namespace details

template <typename T, typename U = T>
struct all_have_eq : details::all_have_eq_impl<T,U> {};
template <typename T, typename U = T>
struct all_have_neq : details::all_have_neq_impl<T,U> {};
template <typename T, typename U = T>
struct all_have_lt : details::all_have_lt_impl<T,U> {};
template <typename T, typename U = T>
struct all_have_gt : details::all_have_gt_impl<T,U> {};
template <typename T, typename U = T>
struct all_have_lteq : details::all_have_lteq_impl<T,U> {};
template <typename T, typename U = T>
struct all_have_gteq : details::all_have_gteq_impl<T,U> {};

namespace details {
template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_eq_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_eq_impl<T,U, index_sequence<Is...>, enable_if_t<
        conjunction<
            all_have_eq<T, U>,
            TT_SCOPE is_all_nothrow_gettable<T>,
            TT_SCOPE is_all_nothrow_gettable<U>,
            has_nothrow_eq<
                tuple_element_t<Is, T>,
                tuple_element_t<Is, U>
            >...
        >::value
    >
> : true_type {};

template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_neq_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_neq_impl<T,U, index_sequence<Is...>, enable_if_t<
        conjunction<
            all_have_neq<T, U>,
            TT_SCOPE is_all_nothrow_gettable<T>,
            TT_SCOPE is_all_nothrow_gettable<U>,
            has_nothrow_neq<
                tuple_element_t<Is, T>,
                tuple_element_t<Is, U>
            >...
        >::value
    >
> : true_type {};

template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_lt_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_lt_impl<T,U, index_sequence<Is...>, enable_if_t<
        conjunction<
            all_have_lt<T, U>,
            TT_SCOPE is_all_nothrow_gettable<T>,
            TT_SCOPE is_all_nothrow_gettable<U>,
            has_nothrow_lt<
                tuple_element_t<Is, T>,
                tuple_element_t<Is, U>
            >...
        >::value
    >
> : true_type {};

template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_gt_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_gt_impl<T,U, index_sequence<Is...>, enable_if_t<
        conjunction<
            all_have_gt<T, U>,
            TT_SCOPE is_all_nothrow_gettable<T>,
            TT_SCOPE is_all_nothrow_gettable<U>,
            has_nothrow_gt<
                tuple_element_t<Is, T>,
                tuple_element_t<Is, U>
            >...
        >::value
    >
> : true_type {};

template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_lteq_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_lteq_impl<T,U, index_sequence<Is...>, enable_if_t<
        conjunction<
            all_have_lteq<T, U>,
            TT_SCOPE is_all_nothrow_gettable<T>,
            TT_SCOPE is_all_nothrow_gettable<U>,
            has_nothrow_lteq<
                tuple_element_t<Is, T>,
                tuple_element_t<Is, U>
            >...
        >::value
    >
> : true_type {};

template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_gteq_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_gteq_impl<T,U, index_sequence<Is...>, enable_if_t<
        conjunction<
            all_have_gteq<T, U>,
            TT_SCOPE is_all_nothrow_gettable<T>,
            TT_SCOPE is_all_nothrow_gettable<U>,
            has_nothrow_gteq<
                tuple_element_t<Is, T>,
                tuple_element_t<Is, U>
            >...
        >::value
    >
> : true_type {};
}    // namespace details

template <typename T, typename U = T>
struct all_have_nothrow_eq : details::all_have_nothrow_eq_impl<T,U> {};
template <typename T, typename U = T>
struct all_have_nothrow_neq : details::all_have_nothrow_neq_impl<T,U> {};
template <typename T, typename U = T>
struct all_have_nothrow_lt : details::all_have_nothrow_lt_impl<T,U> {};
template <typename T, typename U = T>
struct all_have_nothrow_gt : details::all_have_nothrow_gt_impl<T,U> {};
template <typename T, typename U = T>
struct all_have_nothrow_lteq : details::all_have_nothrow_lteq_impl<T,U> {};
template <typename T, typename U = T>
struct all_have_nothrow_gteq : details::all_have_nothrow_gteq_impl<T,U> {};

}    // namespace compare_ops


#ifdef UTL_CXX20

namespace details {
namespace compare_traits {

template<typename T, typename U, typename Cat, size_t... Is>
auto all_three_way_comparable_with_test(float, index_sequence<Is...>) -> false_type;

template<typename T, typename U, typename Cat, size_t... Is>
auto all_three_way_comparable_with_test(int, index_sequence<Is...>)  -> bool_constant<
    tuple_size<T>::value == tuple_size<U>::value &&
    conjunction<decltype(three_way_comparable_with_test<remove_cvref_t<decltype(TT_SCOPE get<Is>(declval<T>()))>, remove_cvref_t<decltype(TT_SCOPE get<Is>(declval<U>()))>, Cat>(0))...>::value
>;

template <typename T, typename U, typename Cat, size_t... Is>
auto all_nothrow_three_way_comparable_with_test(float, index_sequence<Is...>) -> false_type;

template <typename T, typename U, typename Cat, size_t... Is>
auto all_nothrow_three_way_comparable_with_test(int, index_sequence<Is...>) -> bool_constant<
    tuple_size<T>::value == tuple_size<U>::value &&
    TT_SCOPE is_all_nothrow_gettable<T>::value &&
    TT_SCOPE is_all_nothrow_gettable<U>::value &&
    conjunction<decltype(nothrow_three_way_comparable_with_test<remove_cvref_t<decltype(TT_SCOPE get<Is>(declval<T>()))>, remove_cvref_t<decltype(TT_SCOPE get<Is>(declval<U>()))>, Cat>(0))...>::value
>;

template <typename T, typename U, typename Cat, size_t N = tuple_size<T>::value>
using all_three_way_comparable_with_test_t = decltype(all_three_way_comparable_with_test<T,U,Cat>(0, make_index_sequence<N>{}));

template <typename T, typename U, typename Cat, size_t N = tuple_size<T>::value>
using all_nothrow_three_way_comparable_with_test_t = decltype(all_nothrow_three_way_comparable_with_test<T,U,Cat>(0, make_index_sequence<N>{}));

}   // namespace compare_traits
}   // namespace details


template <typename T, typename U = T, typename Cat = std::partial_ordering>
struct is_all_three_way_comparable_with : details::compare_traits::all_three_way_comparable_with_test_t<T,U,Cat> {};

template <typename T, typename U = T, typename Cat = std::partial_ordering>
struct is_all_nothrow_three_way_comparable_with : details::compare_traits::all_nothrow_three_way_comparable_with_test_t<T,U,Cat> {};

#else   // ifdef UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename T, typename U = T, typename Cat = void, typename = void>
struct is_all_three_way_comparable_with : false_type {};

template <typename T, typename U = T, typename Cat = void, typename = void>
struct is_all_nothrow_three_way_comparable_with : false_type {};

UTL_NAMESPACE_END

#endif  // ifdef UTL_CXX20

UTL_NAMESPACE_END

#undef TT_SCOPE
