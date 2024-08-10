// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"

#include "utl/tuple/utl_tuple_fwd.h"

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
struct tuple_size<UTL_SCOPE tuple<T...>> : UTL_SCOPE integral_constant<size_t, sizeof...(T)> {};
template <size_t I, typename... T>
struct tuple_element<I, UTL_SCOPE tuple<T...>> :
    UTL_SCOPE template_element<I, UTL_SCOPE tuple<T...>> {};
// TODO: if std is included or forward declared use std, else use UTL
// RANT: specializable std templates should provide forward declarations
template <typename... T, typename Alloc>
struct uses_allocator<UTL_SCOPE tuple<T...>, Alloc> : UTL_SCOPE true_type {};
} // namespace std

UTL_NAMESPACE_BEGIN
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_tuple : false_type {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_tuple<T&&> : is_tuple<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_tuple<T&> : is_tuple<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_tuple<T const> : is_tuple<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_tuple<T volatile> : is_tuple<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_tuple<T const volatile> : is_tuple<T> {};
template <typename... T>
struct UTL_PUBLIC_TEMPLATE is_tuple<tuple<T...>> : true_type {};

template <typename T>
using tuple_index_sequence = make_index_sequence<tuple_size<T>::value>;

namespace tuple_traits {
namespace details {
struct invalid_size_t {};

template <typename T UTL_REQUIRES_CXX11(sizeof(::std::tuple_size<T>))>
UTL_REQUIRES_CXX20(sizeof(::std::tuple_size<T>) > 0)
UTL_HIDE_FROM_ABI auto size_impl(int) noexcept -> ::std::tuple_size<T>;

template <typename T UTL_REQUIRES_CXX11(is_same<remove_cvref_t<T>, T>::value)>
UTL_REQUIRES_CXX20(is_same<remove_cvref_t<T>, T>::value)
UTL_HIDE_FROM_ABI auto size_impl(...) noexcept -> invalid_size_t;

struct invalid_element_t {};

template <size_t I, typename T UTL_REQUIRES_CXX11(sizeof(::std::tuple_element<I, T>))>
UTL_REQUIRES_CXX20(sizeof(::std::tuple_element<I, T>) > 0)
UTL_HIDE_FROM_ABI auto element_impl(int) noexcept -> ::std::tuple_element<I, T>;

template <size_t I, typename T>
UTL_HIDE_FROM_ABI auto element_impl(...) noexcept -> invalid_element_t;
} // namespace details
} // namespace tuple_traits

template <typename T>
struct UTL_PUBLIC_TEMPLATE tuple_size : decltype(tuple_traits::details::size_impl<T>(0)) {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE tuple_size<T const> : tuple_size<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE tuple_size<T volatile> : tuple_size<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE tuple_size<T const volatile> : tuple_size<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE tuple_size<T&&> : tuple_size<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE tuple_size<T&> : tuple_size<T> {};

template <size_t I, typename T>
struct UTL_PUBLIC_TEMPLATE tuple_element :
    decltype(tuple_traits::details::element_impl<I, T>(0)) {};
template <size_t I, typename T>
struct UTL_PUBLIC_TEMPLATE tuple_element<I, T const> : tuple_element<I, T> {};
template <size_t I, typename T>
struct UTL_PUBLIC_TEMPLATE tuple_element<I, T volatile> : tuple_element<I, T> {};
template <size_t I, typename T>
struct UTL_PUBLIC_TEMPLATE tuple_element<I, T const volatile> : tuple_element<I, T> {};
template <size_t I, typename T>
struct UTL_PUBLIC_TEMPLATE tuple_element<I, T&&> : tuple_element<I, T> {};
template <size_t I, typename T>
struct UTL_PUBLIC_TEMPLATE tuple_element<I, T&> : tuple_element<I, T> {};

namespace tuple_traits {
namespace details {
template <size_t I, typename T UTL_REQUIRES_CXX11((I < tuple_size<T>::value))>
UTL_REQUIRES_CXX20(I < tuple_size<T>::value)
using result_t UTL_NODEBUG = copy_cvref_t<T&&, tuple_element_t<I, T>>;

template <size_t I, typename T, typename = void>
struct is_member_invocable : false_type {};
template <size_t I, typename T>
struct is_member_invocable<I, T,
    enable_if_t<is_convertible<decltype(declval<T>().template get<I>()), result_t<I, T>>::value>> :
    true_type {};

template <size_t I, typename T>
UTL_HIDE_FROM_ABI enable_if_t<is_member_invocable<I, T>::value, result_t<I, T>> get(
    T&& t) noexcept {
    return t.template get<I>();
}

template <size_t I>
struct get_cpo_t {
    UTL_HIDE_FROM_ABI constexpr get_cpo_t() noexcept = default;

    template <typename T>
    UTL_ATTRIBUTES(NODISCARD, FLATTEN, HIDE_FROM_ABI) constexpr enable_if_t<sizeof(get<I>(declval<T>())) && !is_tuple<T>::value,
        result_t<I, T>>
    operator()(T&& t UTL_LIFETIMEBOUND) const noexcept(noexcept(get<I>(declval<T>()))) {
        return get<I>(forward<T>(t));
    }

    template <typename T>
    UTL_ATTRIBUTES(NODISCARD, FLATTEN, CONST, HIDE_FROM_ABI) constexpr enable_if_t<sizeof(get<I>(declval<T>())) && is_tuple<T>::value,
        result_t<I, T>>
    operator()(T&& t UTL_LIFETIMEBOUND) const noexcept {
        return get<I>(forward<T>(t));
    }
};

} // namespace details

template <size_t I, typename T>
UTL_HIDE_FROM_ABI details::result_t<I, T> decl_element() noexcept;

#if UTL_CXX14

template <size_t I>
UTL_INLINE_CXX17 constexpr details::get_cpo_t<I> get = {};

#else // if UTL_CXX14

/** 2-arity CPOs are only available in C++14 with template variables **/

template <size_t I, typename T>
UTL_ATTRIBUTES(NODISCARD, FLATTEN, HIDE_FROM_ABI) constexpr enable_if_t<!is_tuple<T>::value, result_t<I, T>>
get(T&& t UTL_LIFETIMEBOUND) noexcept(
    noexcept(details::get_cpo_t<I>()(UTL_SCOPE declval<T>()))) {
    return details::get_cpo_t<I>()(UTL_SCOPE forward<T>());
}

template <size_t I, typename T>
UTL_ATTRIBUTES(NODISCARD, FLATTEN, CONST, HIDE_FROM_ABI) constexpr enable_if_t<is_tuple<T>::value, result_t<I, T>>
get(T&& t UTL_LIFETIMEBOUND) noexcept {
    return details::get_cpo_t<I>()(UTL_SCOPE forward<T>());
}

#endif // if UTL_CXX14

#define UTL_TUPLE_GET(I, tuple) UTL_SCOPE tuple_traits::get<I>(tuple)

namespace details {
template <size_t I, typename T>
UTL_HIDE_FROM_ABI auto nothrow_test(int)
    -> bool_constant<noexcept(UTL_TUPLE_GET(I, UTL_SCOPE declval<T>()))>;
template <size_t I, typename>
UTL_HIDE_FROM_ABI auto nothrow_test(float) -> false_type;

template <size_t I, typename T, typename = void>
struct is_callable : false_type {};
template <size_t I, typename T>
struct is_callable<I, T, void_t<decltype(UTL_TUPLE_GET(I, UTL_SCOPE declval<T>()))>> : true_type {};

template <size_t I, typename T>
using is_nothrow = decltype(nothrow_test<I, T>(0));
} // namespace details

template <size_t I, typename T>
struct UTL_PUBLIC_TEMPLATE is_nothrow_gettable :
    conjunction<bool_constant<(I < tuple_size<T>::value)>, details::is_nothrow<I, T>> {};

template <size_t I, typename T>
struct UTL_PUBLIC_TEMPLATE is_gettable :
    conjunction<bool_constant<(I < tuple_size<T>::value)>, details::is_callable<I, T>> {};

template <typename TupleLike, size_t N = tuple_size<remove_cvref_t<TupleLike>>::value>
struct UTL_PUBLIC_TEMPLATE is_all_gettable :
    conjunction<is_gettable<N - 1, TupleLike>, is_all_gettable<TupleLike, N - 1>> {};

template <typename TupleLike>
struct UTL_PUBLIC_TEMPLATE is_all_gettable<TupleLike, 0> : is_gettable<0, TupleLike> {};

template <typename TupleLike, size_t N = tuple_size<remove_cvref_t<TupleLike>>::value>
struct UTL_PUBLIC_TEMPLATE is_all_nothrow_gettable :
    conjunction<is_nothrow_gettable<N - 1, TupleLike>, is_all_nothrow_gettable<TupleLike, N - 1>> {
};

template <typename TupleLike>
struct UTL_PUBLIC_TEMPLATE is_all_nothrow_gettable<TupleLike, 0> :
    is_nothrow_gettable<0, TupleLike> {};

namespace details {
namespace tuple {
template <size_t, typename T>
UTL_HIDE_FROM_ABI auto has_element_impl(float) noexcept -> UTL_SCOPE false_type;
template <size_t I, typename T>
UTL_HIDE_FROM_ABI auto has_element_impl(int) noexcept
    -> UTL_SCOPE always_true_type<UTL_SCOPE tuple_element_t<I, T>>;
template <size_t I, typename T>
using has_element UTL_NODEBUG = decltype(has_element_impl<I, T>(0));

template <typename T>
UTL_HIDE_FROM_ABI auto has_all_elements_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename T, size_t... Is>
UTL_HIDE_FROM_ABI auto has_all_elements_impl(UTL_SCOPE index_sequence<Is...>) noexcept
    -> UTL_SCOPE conjunction<UTL_SCOPE always_true_type<tuple_element_t<Is, T>>...>;
template <typename T>
UTL_HIDE_FROM_ABI auto has_all_elements_impl(int) noexcept -> decltype(has_all_elements_impl<T>(
    UTL_SCOPE make_index_sequence<UTL_SCOPE tuple_size<T>::value>{}));
template <typename T>
using has_all_elements UTL_NODEBUG = decltype(has_all_elements_impl<T>(0));

template <typename T>
UTL_HIDE_FROM_ABI auto is_tuple_like_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename T>
UTL_HIDE_FROM_ABI auto is_tuple_like_impl(int) noexcept
    -> conjunction<bool_constant<!is_reference<T>::value>,
        always_true_type<decltype(tuple_size<T>::value)>, has_all_elements<T>>;
template <typename T>
using tuple_like_impl UTL_NODEBUG = decltype(is_tuple_like_impl<T>(0));
} // namespace tuple
} // namespace details

template <typename T>
struct is_tuple_like : details::tuple::tuple_like_impl<T> {};

namespace details {

template <template <typename...> class Target, typename T, size_t... Is>
UTL_HIDE_FROM_ABI Target<decltype(UTL_SCOPE tuple_traits::decl_element<Is, T>())...>
rebuild_target_references(T&& gettable, index_sequence<Is...>) noexcept;

template <template <typename...> class Target, typename T, size_t... Is>
UTL_HIDE_FROM_ABI Target<typename UTL_SCOPE tuple_element<Is, T>::type...> rebuild_target_elements(
    T&& gettable, index_sequence<Is...>) noexcept;

} // namespace details

/**
 * Rebinds the result of `get` for a tuple-like type onto a variadic template type
 *
 * @tparam Target - the target variadic type that will contain the refereces
 * @tparam TupleLike - any tuple-like type or reference, may be cv-qualified
 * @tparam N - number of elements to consider, defaults to tuple_size<TupleLike>::value
 *
 * @return `Target<Ref...>`
 *  where:
 *    `Ref` is `decltype(get<I>(declval<TupleLike>()))` or
 * `decltype(declval<TupleLike>().get<I>())` where I is in the interval [0, N)
 */
template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
using rebind_references_t UTL_NODEBUG =
    decltype(tuple_traits::details::rebuild_target_references<Target>(
        declval<TupleLike>(), make_index_sequence<N>{}));

template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
struct UTL_PUBLIC_TEMPLATE rebind_references {
    using type UTL_NODEBUG = rebind_references_t<Target, TupleLike, N>;
};

/**
 * Rebinds the result of `tuple_element` for a tuple-like type onto a variadic template type
 *
 * @tparam Target - the target variadic type that will contain the refereces
 * @tparam TupleLike - any tuple-like type or reference, may be cv-qualified
 * @tparam N - number of elements to consider, defaults to tuple_size<TupleLike>::value
 *
 * @return `Target<Ref...>`
 *  where:
 *    `Ref` is `decltype(get<I>(declval<TupleLike>()))` or
 * `decltype(declval<TupleLike>().get<I>())` where I is in the interval [0, N)
 */
template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
using rebind_elements_t UTL_NODEBUG =
    decltype(tuple_traits::details::rebuild_target_elements<Target>(
        declval<TupleLike>(), make_index_sequence<N>{}));

template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
struct UTL_PUBLIC_TEMPLATE rebind_elements {
    using type UTL_NODEBUG = rebind_elements_t<Target, TupleLike, N>;
};

template <typename... Ts>
struct concat_elements;

template <typename... Ts>
using concat_elements_t = typename concat_elements<Ts...>::type;

namespace details {
template <typename T0, typename T1, size_t... Is, size_t... Js>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<Is...>, index_sequence<Js...>) noexcept
    -> UTL_SCOPE tuple<tuple_element_t<Is, T0>..., tuple_element_t<Js, T1>...>;
template <typename T0, typename T1, size_t... Js>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<>, index_sequence<Js...>) noexcept
    -> UTL_SCOPE tuple<tuple_element_t<Js, T1>...>;

template <typename T0, typename T1, size_t... Is>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<Is...>, index_sequence<>) noexcept
    -> UTL_SCOPE tuple<tuple_element_t<Is, T0>...>;

template <typename T0, typename T1>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<>, index_sequence<>) noexcept
    -> UTL_SCOPE tuple<>;

template <typename T0, size_t... Is>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<Is...>) noexcept
    -> UTL_SCOPE tuple<tuple_element_t<Is, T0>...>;

template <typename T0>
UTL_HIDE_FROM_ABI auto concat_elements_helper(index_sequence<>) noexcept -> UTL_SCOPE tuple<>;

template <typename T0, typename T1>
UTL_HIDE_FROM_ABI auto concat_elements_helper() noexcept -> decltype(concat_elements_helper<T0, T1>(
    tuple_index_sequence<T0>{}, tuple_index_sequence<T1>{}));

template <typename T0>
UTL_HIDE_FROM_ABI auto concat_elements_helper() noexcept
    -> decltype(concat_elements_helper<T0>(tuple_index_sequence<T0>{}));
} // namespace details

template <typename... Ts>
struct UTL_PUBLIC_TEMPLATE concat_elements<UTL_SCOPE tuple<Ts...>> {
    using type UTL_NODEBUG = UTL_SCOPE tuple<Ts...>;
};

template <typename T0>
struct UTL_PUBLIC_TEMPLATE concat_elements<T0> {
    static_assert(is_tuple_like<T0>::value, "Only tuple_like types can be concatenated");
    using type UTL_NODEBUG = decltype(details::concat_elements_helper<T0>());
};

template <typename T0, typename T1>
struct UTL_PUBLIC_TEMPLATE concat_elements<T0, T1> {
    static_assert(is_tuple_like<T0>::value, "Only tuple_like types can be concatenated");
    static_assert(is_tuple_like<T1>::value, "Only tuple_like types can be concatenated");
    using type UTL_NODEBUG = decltype(details::concat_elements_helper<T0, T1>());
};

template <typename T0, typename T1, typename... Ts>
struct UTL_PUBLIC_TEMPLATE concat_elements<T0, T1, Ts...> :
    concat_elements<concat_elements_t<T0, T1>, Ts...> {};
} // namespace tuple_traits

UTL_NAMESPACE_END
