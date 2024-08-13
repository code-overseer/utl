// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_boolean.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_void_t.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace bool_testable {
struct fourth_t {};
struct third_t : fourth_t {};
struct second_t : third_t {};
struct first_t : second_t {};

/**
 *  These checks don't check the result of mixed types operations, only with boolean
 */
template <typename T>
__UTL_HIDE_FROM_ABI auto no_adl_and_impl(fourth_t) noexcept -> __UTL true_type;
template <typename T>
__UTL_HIDE_FROM_ABI auto no_adl_and_impl(first_t) noexcept
    -> __UTL always_false_type<decltype(operator&&(__UTL declval<T>(), true))>;
template <typename T>
__UTL_HIDE_FROM_ABI auto no_adl_and_impl(second_t) noexcept
    -> __UTL always_false_type<decltype(operator&&(true, __UTL declval<T>()))>;
template <typename T>
__UTL_HIDE_FROM_ABI auto no_adl_and_impl(third_t) noexcept
    -> __UTL always_false_type<decltype(operator&&(__UTL declval<T>(), __UTL declval<T>()))>;

template <typename T>
using no_adl_and UTL_NODEBUG = decltype(no_adl_and_impl<T>(first_t{}));

template <typename T>
__UTL_HIDE_FROM_ABI auto no_member_and_impl(third_t) noexcept -> __UTL true_type;
template <typename T>
__UTL_HIDE_FROM_ABI auto no_member_and_impl(second_t) noexcept
    -> __UTL always_false_type<decltype(__UTL declval<T>().operator&&(true))>;
template <typename T>
__UTL_HIDE_FROM_ABI auto no_member_and_impl(first_t) noexcept
    -> __UTL always_false_type<decltype(__UTL declval<T>().operator&&(__UTL declval<T>()))>;
template <typename T>
using no_member_and UTL_NODEBUG = decltype(no_member_and_impl<T>(first_t{}));

template <typename T>
__UTL_HIDE_FROM_ABI auto has_native_and_impl(float) noexcept -> __UTL false_type;

template <typename T>
__UTL_HIDE_FROM_ABI auto has_native_and_impl(int) noexcept
    -> __UTL conjunction<is_boolean<decltype(__UTL declval<T>() && true)>,
        is_boolean<decltype(true && __UTL declval<T>())>,
        bool_constant<!(false && __UTL declval<T>())>>;

template <typename T>
using has_native_and UTL_NODEBUG = decltype(has_native_and_impl<T>(0));

template <typename T>
struct conjunctable : conjunction<no_adl_and<T>, no_member_and<T>, has_native_and<T>> {};

template <typename T>
__UTL_HIDE_FROM_ABI auto no_adl_or_impl(fourth_t) noexcept -> __UTL true_type;
template <typename T>
__UTL_HIDE_FROM_ABI auto no_adl_or_impl(first_t) noexcept
    -> __UTL always_false_type<decltype(operator||(__UTL declval<T>(), true))>;
template <typename T>
__UTL_HIDE_FROM_ABI auto no_adl_or_impl(second_t) noexcept
    -> __UTL always_false_type<decltype(operator||(true, __UTL declval<T>()))>;
template <typename T>
__UTL_HIDE_FROM_ABI auto no_adl_or_impl(third_t) noexcept
    -> __UTL always_false_type<decltype(operator||(__UTL declval<T>(), __UTL declval<T>()))>;
template <typename T>
using no_adl_or UTL_NODEBUG = decltype(no_adl_or_impl<T>(first_t{}));

template <typename T>
__UTL_HIDE_FROM_ABI auto no_member_or_impl(third_t) noexcept -> __UTL true_type;
template <typename T>
__UTL_HIDE_FROM_ABI auto no_member_or_impl(second_t) noexcept
    -> __UTL always_false_type<decltype(__UTL declval<T>().operator||(true))>;
template <typename T>
__UTL_HIDE_FROM_ABI auto no_member_or_impl(first_t) noexcept
    -> __UTL always_false_type<decltype(__UTL declval<T>().operator||(__UTL declval<T>()))>;
template <typename T>
using no_member_or UTL_NODEBUG = decltype(no_member_or_impl<T>(first_t{}));

template <typename T>
__UTL_HIDE_FROM_ABI auto has_native_or_impl(float) noexcept -> __UTL false_type;

template <typename T>
__UTL_HIDE_FROM_ABI auto has_native_or_impl(
    int) noexcept -> __UTL conjunction<is_boolean<decltype(__UTL declval<T>() || true)>,
    is_boolean<decltype(false || __UTL declval<T>())>, bool_constant<(true || __UTL declval<T>())>>;

template <typename T>
using has_native_or UTL_NODEBUG = decltype(has_native_or_impl<T>(0));

template <typename T>
struct disjunctable : conjunction<no_adl_or<T>, no_member_or<T>, has_native_or<T>> {};

template <typename T>
__UTL_HIDE_FROM_ABI auto no_adl_neg_impl(float) noexcept -> __UTL true_type;

template <typename T>
__UTL_HIDE_FROM_ABI auto no_adl_neg_impl(int) noexcept
    -> __UTL always_false_type<decltype(operator!(__UTL declval<T>()))>;

template <typename T>
using no_adl_neg UTL_NODEBUG = decltype(no_adl_neg_impl<T>(0));

template <typename T>
__UTL_HIDE_FROM_ABI auto no_member_neg_impl(float) noexcept -> __UTL true_type;

template <typename T>
__UTL_HIDE_FROM_ABI auto no_member_neg_impl(int) noexcept
    -> __UTL always_false_type<decltype(__UTL declval<T>().operator!())>;

template <typename T>
using no_member_neg UTL_NODEBUG = decltype(no_member_neg_impl<T>(0));

template <typename T>
__UTL_HIDE_FROM_ABI auto has_native_neg_impl(float) noexcept -> __UTL false_type;

template <typename T>
__UTL_HIDE_FROM_ABI auto has_native_neg_impl(int) noexcept
    -> __UTL always_true_type<decltype(!__UTL declval<T>())>;

template <typename T>
using has_native_neg UTL_NODEBUG = decltype(has_native_neg_impl<T>(0));

template <typename T>
struct negatable : conjunction<no_adl_neg<T>, no_member_neg<T>, has_native_neg<T>> {};

template <typename T>
__UTL_HIDE_FROM_ABI auto castable_impl(float) noexcept -> __UTL false_type;
template <typename T>
__UTL_HIDE_FROM_ABI auto castable_impl(int) noexcept
    -> __UTL always_true_type<decltype(static_cast<bool>(__UTL declval<T>()))>;

template <typename T>
using castable UTL_NODEBUG = decltype(castable_impl<T>(0));

template <typename T>
struct nothrow_castable : bool_constant<noexcept(static_cast<bool>(__UTL declval<T>()))> {};
template <typename T>
struct nothrow_negatable : bool_constant<noexcept(!__UTL declval<T>())> {};
template <typename T>
struct nothrow_conjunctable :
    bool_constant<noexcept(true && __UTL declval<T>()) && noexcept(__UTL declval<T>() && false) &&
        noexcept(__UTL declval<T>() && __UTL declval<T>())> {};
template <typename T>
struct nothrow_disjunctable :
    bool_constant<noexcept(false || __UTL declval<T>()) && noexcept(__UTL declval<T>() || true) &&
        noexcept(__UTL declval<T>() || __UTL declval<T>())> {};

} // namespace bool_testable
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_boolean_testable :
    conjunction<details::bool_testable::castable<T>, details::bool_testable::negatable<T>,
        details::bool_testable::conjunctable<T>, details::bool_testable::disjunctable<T>> {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_boolean_testable<bool> : true_type {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_boolean_testable :
    conjunction<is_boolean_testable<T>, details::bool_testable::nothrow_castable<T>,
        details::bool_testable::nothrow_negatable<T>,
        details::bool_testable::nothrow_conjunctable<T>,
        details::bool_testable::nothrow_disjunctable<T>> {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_boolean_testable<bool> : true_type {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_boolean_testable_v = is_boolean_testable<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_boolean_testable_v = is_nothrow_boolean_testable<T>::value;
#endif

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_boolean_testable(...) __UTL is_boolean_testable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_boolean_testable(...) \
      __UTL is_nothrow_boolean_testable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_boolean_testable(...) __UTL is_boolean_testable<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_boolean_testable(...) \
      __UTL is_nothrow_boolean_testable<__VA_ARGS__>::value
#endif
