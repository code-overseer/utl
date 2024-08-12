// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/compare/utl_compare_traits.h"
#include "utl/tuple/utl_tuple_get_element.h"
#include "utl/tuple/utl_tuple_traits.h"
#include "utl/type_traits/utl_is_boolean_testable.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/type_traits/utl_remove_reference.h"
#include "utl/utility/utl_sequence.h"

UTL_NAMESPACE_BEGIN

namespace compare_ops {
namespace details {

template <typename T>
using const_ref_t UTL_NODEBUG = remove_reference_t<T> const&;
template <typename T>
using sequence_t UTL_NODEBUG =
    conditional_t<UTL_SCOPE is_tuple_like<T>::value, tuple_index_sequence<T>, void>;

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_eq_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_eq_impl<T, U, index_sequence<Is...>,
    enable_if_t<conjunction<UTL_SCOPE is_tuple_like<T>, UTL_SCOPE is_tuple_like<U>,
        bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
        is_equality_comparable_with<const_ref_t<tuple_element_t<Is, T>>,
            const_ref_t<tuple_element_t<Is, U>>>...>::value> // enable_if_t
    > : true_type {};

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_neq_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_neq_impl<T, U, index_sequence<Is...>,
    enable_if_t<conjunction<UTL_SCOPE is_tuple_like<T>, UTL_SCOPE is_tuple_like<U>,
        bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
        is_inequality_comparable_with<const_ref_t<tuple_element_t<Is, T>>,
            const_ref_t<tuple_element_t<Is, U>>>...>::value> // enable_if_t
    > : true_type {};

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_lt_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_lt_impl<T, U, index_sequence<Is...>,
    enable_if_t<conjunction<UTL_SCOPE is_tuple_like<T>, UTL_SCOPE is_tuple_like<U>,
        bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
        is_strict_subordinate_comparable_with<const_ref_t<tuple_element_t<Is, T>>,
            const_ref_t<tuple_element_t<Is, U>>>...>::value> // enable_if_t
    > : true_type {};

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_gt_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_gt_impl<T, U, index_sequence<Is...>,
    enable_if_t<conjunction<UTL_SCOPE is_tuple_like<T>, UTL_SCOPE is_tuple_like<U>,
        bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
        is_strict_superordinate_comparable_with<const_ref_t<tuple_element_t<Is, T>>,
            const_ref_t<tuple_element_t<Is, U>>>...>::value> // enable_if_t
    > : true_type {};

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_lteq_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_lteq_impl<T, U, index_sequence<Is...>,
    enable_if_t<conjunction<UTL_SCOPE is_tuple_like<T>, UTL_SCOPE is_tuple_like<U>,
        bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
        is_subordinate_comparable_with<const_ref_t<tuple_element_t<Is, T>>,
            const_ref_t<tuple_element_t<Is, U>>>...>::value> // enable_if_t
    > : true_type {};

template <typename T, typename U = T, typename Seq = sequence_t<T>, typename = void>
struct all_have_gteq_impl : false_type {};
template <typename T, typename U, size_t... Is>
struct all_have_gteq_impl<T, U, index_sequence<Is...>,
    enable_if_t<conjunction<UTL_SCOPE is_tuple_like<T>, UTL_SCOPE is_tuple_like<U>,
        bool_constant<tuple_size<T>::value == tuple_size<U>::value>,
        is_superordinate_comparable_with<const_ref_t<tuple_element_t<Is, T>>,
            const_ref_t<tuple_element_t<Is, U>>>...>::value> // enable_if_t
    > : true_type {};
} // namespace details

template <typename T, typename U = T>
struct all_have_eq : details::all_have_eq_impl<T, U> {};
template <typename T, typename U = T>
struct all_have_neq : details::all_have_neq_impl<T, U> {};
template <typename T, typename U = T>
struct all_have_lt : details::all_have_lt_impl<T, U> {};
template <typename T, typename U = T>
struct all_have_gt : details::all_have_gt_impl<T, U> {};
template <typename T, typename U = T>
struct all_have_lteq : details::all_have_lteq_impl<T, U> {};
template <typename T, typename U = T>
struct all_have_gteq : details::all_have_gteq_impl<T, U> {};

namespace details {
template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_eq_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_eq_impl<T, U, index_sequence<Is...>,
    enable_if_t<conjunction<all_have_eq<T, U>, UTL_SCOPE details::tuple::is_all_nothrow_gettable<T>,
        UTL_SCOPE details::tuple::is_all_nothrow_gettable<U>,
        is_nothrow_equality_comparable_with<tuple_element_t<Is, T>,
            tuple_element_t<Is, U>>...>::value>> : true_type {};

template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_neq_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_neq_impl<T, U, index_sequence<Is...>,
    enable_if_t<
        conjunction<all_have_neq<T, U>, UTL_SCOPE details::tuple::is_all_nothrow_gettable<T>,
            UTL_SCOPE details::tuple::is_all_nothrow_gettable<U>,
            is_nothrow_inequality_comparable_with<tuple_element_t<Is, T>,
                tuple_element_t<Is, U>>...>::value>> : true_type {};

template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_lt_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_lt_impl<T, U, index_sequence<Is...>,
    enable_if_t<conjunction<all_have_lt<T, U>, UTL_SCOPE details::tuple::is_all_nothrow_gettable<T>,
        UTL_SCOPE details::tuple::is_all_nothrow_gettable<U>,
        is_nothrow_strict_subordinate_comparable_with<tuple_element_t<Is, T>,
            tuple_element_t<Is, U>>...>::value>> : true_type {};

template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_gt_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_gt_impl<T, U, index_sequence<Is...>,
    enable_if_t<conjunction<all_have_gt<T, U>, UTL_SCOPE details::tuple::is_all_nothrow_gettable<T>,
        UTL_SCOPE details::tuple::is_all_nothrow_gettable<U>,
        is_nothrow_strict_superordinate_comparable_with<tuple_element_t<Is, T>,
            tuple_element_t<Is, U>>...>::value>> : true_type {};

template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_lteq_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_lteq_impl<T, U, index_sequence<Is...>,
    enable_if_t<
        conjunction<all_have_lteq<T, U>, UTL_SCOPE details::tuple::is_all_nothrow_gettable<T>,
            UTL_SCOPE details::tuple::is_all_nothrow_gettable<U>,
            is_nothrow_subordinate_comparable_with<tuple_element_t<Is, T>,
                tuple_element_t<Is, U>>...>::value>> : true_type {};

template <typename T, typename U, typename Seq = sequence_t<T>, typename = void>
struct all_have_nothrow_gteq_impl;
template <typename T, typename U, size_t... Is>
struct all_have_nothrow_gteq_impl<T, U, index_sequence<Is...>,
    enable_if_t<
        conjunction<all_have_gteq<T, U>, UTL_SCOPE details::tuple::is_all_nothrow_gettable<T>,
            UTL_SCOPE details::tuple::is_all_nothrow_gettable<U>,
            is_nothrow_superordinate_comparable_with<tuple_element_t<Is, T>,
                tuple_element_t<Is, U>>...>::value>> : true_type {};
} // namespace details

template <typename T, typename U = T>
struct all_have_nothrow_eq : details::all_have_nothrow_eq_impl<T, U> {};
template <typename T, typename U = T>
struct all_have_nothrow_neq : details::all_have_nothrow_neq_impl<T, U> {};
template <typename T, typename U = T>
struct all_have_nothrow_lt : details::all_have_nothrow_lt_impl<T, U> {};
template <typename T, typename U = T>
struct all_have_nothrow_gt : details::all_have_nothrow_gt_impl<T, U> {};
template <typename T, typename U = T>
struct all_have_nothrow_lteq : details::all_have_nothrow_lteq_impl<T, U> {};
template <typename T, typename U = T>
struct all_have_nothrow_gteq : details::all_have_nothrow_gteq_impl<T, U> {};

} // namespace compare_ops

UTL_NAMESPACE_END

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace compare_traits {

template <typename T, typename U, typename Cat, size_t... Is>
__UTL_HIDE_FROM_ABI auto all_three_way_comparable_with_test(float, index_sequence<Is...>)
    -> false_type;

template <typename T, typename U, typename Cat, size_t... Is>
__UTL_HIDE_FROM_ABI auto all_three_way_comparable_with_test(int, index_sequence<Is...>)
    -> bool_constant<tuple_size<T>::value == tuple_size<U>::value &&
        conjunction<decltype(three_way_comparable_with_test<
            remove_cvref_t<decltype(UTL_SCOPE get_element<Is>(UTL_SCOPE declval<T>()))>,
            remove_cvref_t<decltype(UTL_SCOPE get_element<Is>(UTL_SCOPE declval<U>()))>, Cat>(
            0))...>::value>;

template <typename T, typename U, typename Cat, size_t... Is>
__UTL_HIDE_FROM_ABI auto all_nothrow_three_way_comparable_with_test(float, index_sequence<Is...>)
    -> false_type;

template <typename T, typename U, typename Cat, size_t... Is>
__UTL_HIDE_FROM_ABI auto all_nothrow_three_way_comparable_with_test(int, index_sequence<Is...>)
    -> bool_constant<tuple_size<T>::value == tuple_size<U>::value &&
        UTL_SCOPE details::tuple::is_all_nothrow_gettable<T>::value &&
        UTL_SCOPE details::tuple::is_all_nothrow_gettable<U>::value &&
        conjunction<decltype(nothrow_three_way_comparable_with_test<
            remove_cvref_t<decltype(UTL_SCOPE get_element<Is>(UTL_SCOPE declval<T>()))>,
            remove_cvref_t<decltype(UTL_SCOPE get_element<Is>(UTL_SCOPE declval<U>()))>, Cat>(
            0))...>::value>;

template <typename T, typename U, typename Cat, size_t N = tuple_size<T>::value>
using all_three_way_comparable_with_test_t UTL_NODEBUG =
    decltype(all_three_way_comparable_with_test<T, U, Cat>(0, make_index_sequence<N>{}));

template <typename T, typename U, typename Cat, size_t N = tuple_size<T>::value>
using all_nothrow_three_way_comparable_with_test_t UTL_NODEBUG =
    decltype(all_nothrow_three_way_comparable_with_test<T, U, Cat>(0, make_index_sequence<N>{}));

} // namespace compare_traits
} // namespace details

template <typename T, typename U = T, typename Cat = std::partial_ordering>
struct __UTL_PUBLIC_TEMPLATE is_all_three_way_comparable_with :
    details::compare_traits::all_three_way_comparable_with_test_t<T, U, Cat> {};

template <typename T, typename U = T, typename Cat = std::partial_ordering>
struct __UTL_PUBLIC_TEMPLATE is_all_nothrow_three_way_comparable_with :
    details::compare_traits::all_nothrow_three_way_comparable_with_test_t<T, U, Cat> {};

UTL_NAMESPACE_END

#else // UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename T, typename U = T, typename Cat = void, typename = void>
struct __UTL_PUBLIC_TEMPLATE is_all_three_way_comparable_with : false_type {};

template <typename T, typename U = T, typename Cat = void, typename = void>
struct __UTL_PUBLIC_TEMPLATE is_all_nothrow_three_way_comparable_with : false_type {};

UTL_NAMESPACE_END

#endif // UTL_CXX20
