// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_std_traits.h"
#include "utl/type_traits/utl_void_t.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace bool_testable {

template <typename T> struct is_boolean : false_type {};

template <> struct is_boolean<bool> : true_type {};
template <> struct is_boolean<bool const> : true_type {};
template <> struct is_boolean<bool volatile> : true_type {};
template <> struct is_boolean<bool const volatile> : true_type {};

/**
 *  These checks don't check the result of mixed types operations, only with boolean
 */
template <typename T, typename = void> struct no_adl_and_l : true_type {};
template <typename T>
struct no_adl_and_l<T, void_t<decltype(operator&& (declval<T>(), true))>> : false_type {};
template <typename T, typename = void> struct no_adl_and_r : true_type {};
template <typename T>
struct no_adl_and_r<T, void_t<decltype(operator&& (true, declval<T>()))>> : false_type {};
template <typename T, typename = void> struct no_adl_and_lr : true_type {};
template <typename T>
struct no_adl_and_lr<T, void_t<decltype(operator&& (declval<T>(), declval<T>()))>> : false_type {};
template <typename T>
using no_adl_and = conjunction<no_adl_and_l<T>, no_adl_and_r<T>, no_adl_and_lr<T>>;

template <typename T, typename = void> struct no_member_and_l : true_type {};
template <typename T>
struct no_member_and_l<T, void_t<decltype(declval<T>().operator&& (true))>> : false_type {};
template <typename T, typename = void> struct no_member_and_lr : true_type {};
template <typename T>
struct no_member_and_lr<T, void_t<decltype(declval<T>().operator&& (declval<T>()))>> :
    false_type {};
template <typename T> using no_member_and = conjunction<no_member_and_l<T>, no_member_and_lr<T>>;

template <typename T, typename = void> struct has_native_and : false_type {};
template <typename T>
struct has_native_and<T,
    enable_if_t<conjunction<is_boolean<decltype(declval<T>() && true)>,
        is_boolean<decltype(true && declval<T>())>,
        bool_constant<!(false && declval<T>())> // if short-circuiting succeeds, declval is not
                                                // invoked
        >::value>> : true_type {};

template <typename T>
struct conjunctable : conjunction<no_adl_and<T>, no_member_and<T>, has_native_and<T>> {};

template <typename T, typename = void> struct no_adl_or_l : true_type {};
template <typename T>
struct no_adl_or_l<T, void_t<decltype(operator|| (declval<T>(), true))>> : false_type {};
template <typename T, typename = void> struct no_adl_or_r : true_type {};
template <typename T>
struct no_adl_or_r<T, void_t<decltype(operator|| (true, declval<T>()))>> : false_type {};
template <typename T, typename = void> struct no_adl_or_lr : true_type {};
template <typename T>
struct no_adl_or_lr<T, void_t<decltype(operator|| (declval<T>(), declval<T>()))>> : false_type {};
template <typename T>
using no_adl_or = conjunction<no_adl_or_l<T>, no_adl_or_r<T>, no_adl_or_lr<T>>;

template <typename T, typename = void> struct no_member_or_l : true_type {};
template <typename T>
struct no_member_or_l<T, void_t<decltype(declval<T>().operator|| (true))>> : false_type {};
template <typename T, typename = void> struct no_member_or_lr : true_type {};
template <typename T>
struct no_member_or_lr<T, void_t<decltype(declval<T>().operator|| (declval<T>()))>> : false_type {};
template <typename T> using no_member_or = conjunction<no_member_or_l<T>, no_member_or_lr<T>>;

template <typename T, typename = void> struct has_native_or : false_type {};
template <typename T>
struct has_native_or<T,
    enable_if_t<conjunction<is_boolean<decltype(declval<T>() || false)>,
        is_boolean<decltype(false || declval<T>())>,
        bool_constant<(true || declval<T>())> // if short-circuiting succeeds, declval is not
                                              // invoked
        >::value>> : true_type {};

template <typename T>
struct disjunctable : conjunction<no_adl_or<T>, no_member_or<T>, has_native_or<T>> {};

template <typename T, typename = void> struct no_adl_neg : true_type {};
template <typename T>
struct no_adl_neg<T, void_t<decltype(operator!(declval<T>()))>> : false_type {};

template <typename T, typename = void> struct no_member_neg : true_type {};
template <typename T>
struct no_member_neg<T, void_t<decltype(declval<T>().operator!())>> : false_type {};

template <typename T, typename = bool> struct has_native_neg : false_type {};
template <typename T> struct has_native_neg<T, decltype(!declval<T>())> : true_type {};

template <typename T>
struct negatable : conjunction<no_adl_neg<T>, no_member_neg<T>, has_native_neg<T>> {};

template <typename T, typename = bool> struct castable : false_type {};
template <typename T> struct castable<T, decltype(static_cast<bool>(declval<T>()))> : true_type {};

} // namespace bool_testable
} // namespace details

template <typename T>
struct is_boolean_testable :
    conjunction<details::bool_testable::castable<T, bool>, details::bool_testable::negatable<T>,
        details::bool_testable::conjunctable<T>, details::bool_testable::disjunctable<T>> {};

#ifdef UTL_CXX20

template <typename T>
concept boolean_testable = is_boolean_testable<T>::value;

#endif // ifdef UTL_CXX20

UTL_NAMESPACE_END
