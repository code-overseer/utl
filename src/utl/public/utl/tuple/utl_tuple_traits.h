// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/tuple/utl_tuple_fwd.h"

#include "utl/tuple/utl_tuple_get_element.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_copy_cvref.h"
#include "utl/type_traits/utl_is_reference.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/type_traits/utl_template_list.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_sequence.h"

namespace std {
// @see tuple_fwd for note on tuple_size and tuple_element
template <typename... T>
struct tuple_size<__UTL tuple<T...>> : __UTL integral_constant<size_t, sizeof...(T)> {};
template <size_t I, typename... T>
struct tuple_element<I, __UTL tuple<T...>> : __UTL template_element<I, __UTL tuple<T...>> {};
// TODO: if std is included or forward declared use std, else use UTL
// RANT: specializable std templates should provide forward declarations
template <typename... T, typename Alloc>
struct uses_allocator<__UTL tuple<T...>, Alloc> : __UTL true_type {};
} // namespace std

UTL_NAMESPACE_BEGIN
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_tuple : false_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_tuple<T&&> : is_tuple<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_tuple<T&> : is_tuple<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_tuple<T const> : is_tuple<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_tuple<T volatile> : is_tuple<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_tuple<T const volatile> : is_tuple<T> {};
template <typename... T>
struct __UTL_PUBLIC_TEMPLATE is_tuple<tuple<T...>> : true_type {};

template <typename T>
using tuple_index_sequence = make_index_sequence<tuple_size<T>::value>;

namespace details {
namespace tuple {
struct invalid_size_t {};

template <typename T UTL_CONSTRAINT_CXX11(sizeof(::std::tuple_size<T>))>
UTL_CONSTRAINT_CXX20(sizeof(::std::tuple_size<T>) > 0)
__UTL_HIDE_FROM_ABI auto size_impl(int) noexcept -> ::std::tuple_size<T>;

template <typename T UTL_CONSTRAINT_CXX11(is_same<remove_cvref_t<T>, T>::value)>
UTL_CONSTRAINT_CXX20(is_same<remove_cvref_t<T>, T>::value)
__UTL_HIDE_FROM_ABI auto size_impl(...) noexcept -> invalid_size_t;

struct invalid_element_t {};

template <size_t I, typename T UTL_CONSTRAINT_CXX11(sizeof(::std::tuple_element<I, T>))>
UTL_CONSTRAINT_CXX20(sizeof(::std::tuple_element<I, T>) > 0)
__UTL_HIDE_FROM_ABI auto element_impl(int) noexcept -> ::std::tuple_element<I, T>;

template <size_t I, typename T>
__UTL_HIDE_FROM_ABI auto element_impl(...) noexcept -> invalid_element_t;

template <typename T>
using size_fallback = decltype(size_impl<T>(0));
template <size_t I, typename T>
using element_fallback = decltype(element_impl<I, T>(0));
} // namespace tuple
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_size : details::tuple::size_fallback<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_size<T const> : tuple_size<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_size<T volatile> : tuple_size<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_size<T const volatile> : tuple_size<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_size<T&&> : tuple_size<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_size<T&> : tuple_size<T> {};

template <size_t I, typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_element : details::tuple::element_fallback<I, T> {};
template <size_t I, typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_element<I, T const> : tuple_element<I, T> {};
template <size_t I, typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_element<I, T volatile> : tuple_element<I, T> {};
template <size_t I, typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_element<I, T const volatile> : tuple_element<I, T> {};
template <size_t I, typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_element<I, T&&> : tuple_element<I, T> {};
template <size_t I, typename T>
struct __UTL_PUBLIC_TEMPLATE tuple_element<I, T&> : tuple_element<I, T> {};

namespace details {
namespace tuple {
template <size_t, typename T>
__UTL_HIDE_FROM_ABI auto has_element_impl(float) noexcept -> __UTL false_type;
template <size_t I, typename T>
__UTL_HIDE_FROM_ABI auto has_element_impl(int) noexcept
    -> __UTL bool_constant<(I < tuple_size<T>::value) &&
        UTL_TRAIT_is_convertible(decltype(__UTL get_element<I>(__UTL declval<T>())),
            __UTL tuple_element_t<I, T> const&)>;
template <size_t I, typename T>
using has_element_trait UTL_NODEBUG = decltype(has_element_impl<I, T>(0));

template <typename T>
__UTL_HIDE_FROM_ABI auto has_all_elements_impl(float) noexcept -> __UTL false_type;
template <typename T, size_t... Is>
__UTL_HIDE_FROM_ABI auto has_all_elements_impl(__UTL index_sequence<Is...>) noexcept
    -> __UTL conjunction<__UTL always_true_type<tuple_element_t<Is, T>>...>;
template <typename T>
__UTL_HIDE_FROM_ABI auto has_all_elements_impl(int) noexcept
    -> decltype(has_all_elements_impl<T>(__UTL make_index_sequence<__UTL tuple_size<T>::value>{}));
template <typename T>
using has_all_elements_trait UTL_NODEBUG = decltype(has_all_elements_impl<T>(0));

template <typename T>
__UTL_HIDE_FROM_ABI auto is_tuple_like_impl(float) noexcept -> __UTL false_type;
template <typename T>
__UTL_HIDE_FROM_ABI auto is_tuple_like_impl(int) noexcept
    -> conjunction<bool_constant<!is_reference<T>::value>,
        always_true_type<decltype(tuple_size<T>::value)>, has_all_elements_trait<T>>;
template <typename T>
using tuple_like_trait UTL_NODEBUG = decltype(is_tuple_like_impl<T>(0));
} // namespace tuple
} // namespace details

template <typename T>
struct is_tuple_like : details::tuple::tuple_like_trait<T> {};

UTL_NAMESPACE_END
