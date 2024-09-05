// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/compare/utl_compare_fwd.h"

#include "utl/type_traits/utl_add_const.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include <compare>

UTL_NAMESPACE_BEGIN

using std::common_comparison_category;
using std::common_comparison_category_t;
UTL_NAMESPACE_END

#else // UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

UTL_NAMESPACE_BEGIN
namespace details {
namespace comparison_category {

template <typename... Ts>
struct impl {
    using type UTL_NODEBUG = void;
};

template <typename... Ts>
struct impl<::std::partial_ordering, Ts...> {
    using type UTL_NODEBUG = ::std::partial_ordering;
};
template <typename... Ts>
struct impl<::std::strong_ordering, Ts...> : impl<Ts...> {};
template <>
struct impl<> {
    using type UTL_NODEBUG = ::std::strong_ordering;
};

template <typename... Ts>
struct weak_branch {
    using type UTL_NODEBUG = void;
};
template <>
struct weak_branch<> {
    using type UTL_NODEBUG = ::std::weak_ordering;
};
template <typename... Ts>
struct weak_branch<::std::partial_ordering, Ts...> {
    using type UTL_NODEBUG = ::std::partial_ordering;
};
template <typename... Ts>
struct weak_branch<::std::weak_ordering, Ts...> : weak_branch<Ts...> {};
template <typename... Ts>
struct weak_branch<::std::strong_ordering, Ts...> : weak_branch<Ts...> {};
template <typename... Ts>
struct impl<::std::weak_ordering, Ts...> : weak_branch<Ts...> {};

} // namespace comparison_category
} // namespace details

template <typename... Ts>
using common_comparison_category_t = typename details::comparison_category::impl<Ts...>::type;

template <typename... Ts>
struct __UTL_PUBLIC_TEMPLATE common_comparison_category :
    details::comparison_category::impl<Ts...> {};

UTL_NAMESPACE_END

#endif // UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#define UTL_TRAIT_SUPPORTED_common_comparison_category 1
