// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_USE_STD_TYPE_TRAITS
#  include <type_traits>

UTL_NAMESPACE_BEGIN
using std::common_type;
template <typename... T>
using common_type_t = typename common_type<T...>::type;
UTL_NAMESPACE_END

#else // UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_decay.h"
#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_same.h"
#  include "utl/type_traits/utl_remove_reference.h"
#  include "utl/type_traits/utl_template_list.h"
#  include "utl/type_traits/utl_void_t.h"

UTL_STD_NAMESPACE_BEGIN
/* UTL_UNDEFINED_BEHAVIOUR */
template <typename...>
struct common_type;
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN

template <typename... Ts>
struct common_type;
template <typename... T>
using common_type_t = typename common_type<T...>::type;

template <>
struct __UTL_PUBLIC_TEMPLATE common_type<> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE common_type<T> : common_type<T, T> {};

namespace details {
namespace _common_type {

template <typename T, typename U>
using ternary_result_t UTL_NODEBUG =
    __UTL decay_t<decltype(false ? __UTL declval<T>() : __UTL declval<U>())>;

template <typename T, typename U, typename = void>
struct impl_2 {};
template <typename T, typename U, typename = void>
struct impl_1 : impl_2<T, U> {};
template <typename T, typename U, typename = void>
struct impl_0 : impl_1<T, U> {};

template <typename T, typename U>
struct impl_0<T, U,
    enable_if_t<!UTL_TRAIT_is_same(T, decay_t<T>) || !UTL_TRAIT_is_same(U, decay_t<U>)>> :
    __UTL common_type<decay_t<T>, decay_t<U>> {};

template <typename T, typename U>
struct impl_1<T, U, void_t<ternary_result_t<T, U>>> {
    using type = ternary_result_t<T, U>;
};

template <typename T, typename U>
struct impl_2<T, U,
    void_t<ternary_result_t<remove_reference_t<T> const&, remove_reference_t<U> const&>>> {
    using type = ternary_result_t<remove_reference_t<T> const&, remove_reference_t<U> const&>;
};

template <typename List, typename = void>
struct sfinae_gt_2 {};

template <typename T, typename U, typename... Vs>
struct sfinae_gt_2<type_list<T, U, Vs...>, void_t<__UTL common_type_t<T, U>>> :
    __UTL common_type<__UTL common_type_t<T, U>, Vs...> {};

template <typename T, typename U, typename... Vs>
using impl_gt_2 = sfinae_gt_2<type_list<T, U, Vs...>>;

template <typename T, typename U, size_t = sizeof(::std::common_type<T, U>)>
__UTL_HIDE_FROM_ABI ::std::common_type<T, U> resolve_common_type(int);

template <typename T, typename U>
__UTL_HIDE_FROM_ABI impl_0<T, U> resolve_common_type(float);

template <typename T, typename U, typename R = decltype(resolve_common_type<T, U>(0))>
using impl UTL_NODEBUG = R;

} // namespace _common_type
} // namespace details

template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE common_type<T, U> : details::_common_type::impl<T, U> {};

template <typename T, typename U, typename... Vs>
struct __UTL_PUBLIC_TEMPLATE common_type<T, U, Vs...> :
    details::_common_type::impl_gt_2<T, U, Vs...> {};

UTL_NAMESPACE_END
#endif

#define UTL_TRAIT_SUPPORTED_common_type 1
