/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/type_traits/utl_common.h"

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_convertible;

namespace details {
namespace convertible {
template <typename T>
void implicit_conv(T) noexcept;
template <typename From, typename To>
auto nothrow_test(int) -> bool_constant<noexcept(implicit_conv<To>(declval<From>()))>;
template <typename, typename>
auto nothrow_test(float) -> false_type;

template <typename From, typename To>
using is_nothrow = decltype(nothrow_test<From, To>(0));
} /* namespace convertible */
} /* namespace details */

template <typename From, typename To>
struct is_nothrow_convertible : details::convertible::is_nothrow<From, To> {};
template <typename To>
struct is_nothrow_convertible<To, To> : true_type {};

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_convertible 1
#define UTL_TRAIT_SUPPORTED_is_nothrow_convertible 1
