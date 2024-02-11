// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/type_traits/utl_boolean_testable.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/compare/utl_partial_ordering.h"
#include "utl/concepts.h"

UTL_NAMESPACE_BEGIN

namespace compare_ops {
template <typename T, typename U = T, typename = void>
struct has_eq : false_type {};
template <typename T, typename U>
struct has_eq<T, U, enable_if_t<is_boolean_testable<decltype(declval<T>() == declval<U>())>::value>> : true_type {};

template <typename T, typename U = T, typename = void>
struct has_nothrow_eq : false_type {};
template <typename T, typename U>
struct has_nothrow_eq<T, U, enable_if_t<has_eq<T, U>::value>> : bool_constant<noexcept(declval<T>() == declval<U>())> {};

template <typename T, typename U = T, typename = void>
struct has_neq : false_type {};
template <typename T, typename U>
struct has_neq<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() != declval<U>())>::value>> : true_type {};
template <typename T, typename U = T, typename = void>
struct has_nothrow_neq : false_type {};
template <typename T, typename U>
struct has_nothrow_neq<T, U, enable_if_t<has_neq<T, U>::value>> : bool_constant<noexcept(declval<T>() != declval<U>())> {};

template <typename T, typename U = T, typename = void>
struct has_lt : false_type {};
template <typename T, typename U>
struct has_lt<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() < declval<U>())>::value>> : true_type {};
template <typename T, typename U = T, typename = void>
struct has_nothrow_lt : false_type {};
template <typename T, typename U>
struct has_nothrow_lt<T, U, enable_if_t<has_lt<T, U>::value>> : bool_constant<noexcept(declval<T>() < declval<U>())> {};

template <typename T, typename U = T, typename = void>
struct has_gt : false_type {};
template <typename T, typename U>
struct has_gt<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() > declval<U>())>::value>> : true_type {};
template <typename T, typename U = T, typename = void>
struct has_nothrow_gt : false_type {};
template <typename T, typename U>
struct has_nothrow_gt<T, U, enable_if_t<has_gt<T, U>::value>> : bool_constant<noexcept(declval<T>() > declval<U>())> {};

template <typename T, typename U = T, typename = void>
struct has_lteq : false_type {};
template <typename T, typename U>
struct has_lteq<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() <= declval<U>())>::value>> : true_type {};
template <typename T, typename U = T, typename = void>
struct has_nothrow_lteq : false_type {};
template <typename T, typename U>
struct has_nothrow_lteq<T, U, enable_if_t<has_lteq<T, U>::value>> : bool_constant<noexcept(declval<T>() <= declval<U>())> {};

template <typename T, typename U = T, typename = void>
struct has_gteq : false_type {};
template <typename T, typename U>
struct has_gteq<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() >= declval<U>())>::value>> : true_type {};
template <typename T, typename U = T, typename = void>
struct has_nothrow_gteq : false_type {};
template <typename T, typename U>
struct has_nothrow_gteq<T, U, enable_if_t<has_gteq<T, U>::value>> : bool_constant<noexcept(declval<T>() >= declval<U>())> {};
}    // namespace operable

template <typename T, typename U = T, typename = void>
struct is_equality_comparable : false_type {};

template <typename T, typename U>
struct is_equality_comparable<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() == declval<U>())>::value>> : true_type {};

template <typename T, typename U = T, typename = void>
struct is_nonequality_comparable : false_type {};

template <typename T, typename U>
struct is_nonequality_comparable<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() != declval<U>())>::value>> : true_type {};

template <typename T, typename U = T, typename = void>
struct is_less_comparable : false_type {};

template <typename T, typename U>
struct is_less_comparable<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() < declval<U>())>::value>> : true_type {};

template <typename T, typename U = T, typename = void>
struct is_greater_comparable : false_type {};

template <typename T, typename U>
struct is_greater_comparable<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() > declval<U>())>::value>> : true_type {};

template <typename T, typename U = T, typename = void>
struct is_less_equal_comparable : false_type {};

template <typename T, typename U>
struct is_less_equal_comparable<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() <= declval<U>())>::value>> : true_type {};

template <typename T, typename U = T, typename = void>
struct is_greater_equal_comparable : false_type {};

template <typename T, typename U>
struct is_greater_equal_comparable<T,U, enable_if_t<is_boolean_testable<decltype(declval<T>() >= declval<U>())>::value>> : true_type {};

namespace details {
namespace compare_traits {

template <typename T, typename U>
auto nothrow_equality_test(float) -> false_type;

template <typename T, typename U>
auto nothrow_equality_test(int) -> bool_constant<noexcept(static_cast<bool>(declval<T>() == declval<U>()))>;

template <typename T, typename U>
auto nothrow_nonequality_test(float) -> false_type;

template <typename T, typename U>
auto nothrow_nonequality_test(int) -> bool_constant<noexcept(static_cast<bool>(declval<T>() != declval<U>()))>;

template <typename T, typename U>
auto nothrow_less_test(float) -> false_type;

template <typename T, typename U>
auto nothrow_less_test(int) -> bool_constant<noexcept(static_cast<bool>(declval<T>() < declval<U>()))>;

template <typename T, typename U>
auto nothrow_greater_test(float) -> false_type;

template <typename T, typename U>
auto nothrow_greater_test(int) -> bool_constant<noexcept(static_cast<bool>(declval<T>() > declval<U>()))>;

template <typename T, typename U>
auto nothrow_less_equal_test(float) -> false_type;

template <typename T, typename U>
auto nothrow_less_equal_test(int) -> bool_constant<noexcept(static_cast<bool>(declval<T>() <= declval<U>()))>;

template <typename T, typename U>
auto nothrow_greater_equal_test(float) -> false_type;

template <typename T, typename U>
auto nothrow_greater_equal_test(int) -> bool_constant<noexcept(static_cast<bool>(declval<T>() >= declval<U>()))>;

}   // namespace compare_traits
}   // namespace details

template <typename T, typename U = T, typename R = decltype(details::compare_traits::nothrow_equality_test<T,U>(0))>
struct is_nothrow_equality_comparable : R {};

template <typename T, typename U = T, typename R = decltype(details::compare_traits::nothrow_nonequality_test<T,U>(0))>
struct is_nothrow_nonequality_comparable : R {};

template <typename T, typename U = T, typename R = decltype(details::compare_traits::nothrow_less_test<T,U>(0))>
struct is_nothrow_less_comparable : R {};

template <typename T, typename U = T, typename R = decltype(details::compare_traits::nothrow_greater_test<T,U>(0))>
struct is_nothrow_greater_comparable : R {};

template <typename T, typename U = T, typename R = decltype(details::compare_traits::nothrow_less_equal_test<T,U>(0))>
struct is_nothrow_less_equal_comparable : R {};

template <typename T, typename U = T, typename R = decltype(details::compare_traits::nothrow_greater_equal_test<T,U>(0))>
struct is_nothrow_greater_equal_comparable : R {};

#ifdef UTL_CXX20

namespace details {
namespace compare_traits {

template<typename T, typename U, typename Cat>
false_type three_way_comparable_with_test(...);

template<typename T, typename U, typename Cat>
auto three_way_comparable_with_test(int) -> is_convertible<decltype(declval<T>() <=> declval<U>()), Cat>;

template <typename T, typename U, typename Cat>
auto nothrow_three_way_comparable_with_test(...) -> false_type;

template <typename T, typename U, typename Cat>
auto nothrow_three_way_comparable_with_test(int) -> bool_constant<
decltype(three_way_comparable_with_test<T,U,Cat>(0))::value &&
noexcept(declval<T>() <=> declval<U>())>;

}
}

template <typename T, typename U = T, typename Cat = UTL_SCOPE partial_ordering>
struct is_three_way_comparable_with : decltype(details::compare_traits::three_way_comparable_with_test<T,U,Cat>(0)) {};

template <typename T, typename U = T, typename Cat = UTL_SCOPE partial_ordering>
struct is_nothrow_three_way_comparable_with : decltype(details::compare_traits::nothrow_three_way_comparable_with_test<T,U,Cat>(0)) {};

#else   // ifdef UTL_CXX20

template <typename T, typename U = T, typename Cat = void, typename = void>
struct is_all_three_way_comparable_with : false_type {};

template <typename T, typename U = T, typename Cat = void, typename = void>
struct is_all_nothrow_three_way_comparable_with : false_type {};

#endif  // ifdef UTL_CXX20

UTL_NAMESPACE_END
