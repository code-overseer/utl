// Copyright 2023-2024 Bryan Wong

#pragma once

/** TODO cleanup this implementation and support allocator_arg **/

#if defined(UTL_USE_STDPAIR) && UTL_USE_STDPAIR

#include "utl/preprocessor/utl_namespace.h"
#include <utility>

UTL_NAMESPACE_BEGIN

using std::pair;
using std::make_pair;
using std::get;
using std::tuple_size;
using std::tuple_element;
using std::piecewise_construct_t;
using std::piecewise_construct;

UTL_NAMESPACE_END

#else    // defined(UTL_USE_STDPAIR) && UTL_USE_STDPAIR

#include "utl/compare/utl_compare_traits.h"
#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/tuple/utl_tuple_traits.h"
#include "utl/type_traits/utl_common_reference.h"
#include "utl/type_traits/utl_common_type.h"
#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_template_list.h"
#include "utl/type_traits/utl_unwrap_reference.h"
#include "utl/type_traits/utl_variadic_traits.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"
#include "utl/utility/utl_sequence.h"

#define TT_SCOPE UTL_SCOPE tuple_traits::

UTL_NAMESPACE_BEGIN

template<typename T0, typename T1>
struct pair;

template<typename...>
class tuple;

template<typename T>
struct is_pair : false_type {};
template<typename T>
struct is_pair<T&&> : is_pair<T> {};
template<typename T>
struct is_pair<T&> : is_pair<T> {};
template<typename T>
struct is_pair<T const> : is_pair<T> {};
template<typename T>
struct is_pair<T volatile> : is_pair<T> {};
template<typename T>
struct is_pair<T const volatile> : is_pair<T> {};
template<typename T, typename U>
struct is_pair<pair<T, U>> : true_type {};

struct piecewise_construct_t { explicit constexpr piecewise_construct_t() noexcept = default; };

/// Tag for piecewise construction of pair objects.
UTL_INLINE_CXX17 constexpr piecewise_construct_t piecewise_construct = piecewise_construct_t();

UTL_BEGIN_REGION(pair_get)

namespace details {
namespace pair {
template<size_t I, typename P>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr auto get(P&& p UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept -> enable_if_t<!I && is_pair<P>::value, copy_cvref_t<decltype(declval<P>()), decltype(p.first)>> {
    return forward<P>(p).first;
}

template<size_t I, typename P>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr auto get(P&& p UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept -> enable_if_t<(I == 1) && is_pair<P>::value, copy_cvref_t<decltype(declval<P>()), decltype(p.second)>> {
    return forward<P>(p).second;
}
}   // namespace pair
}   // namespace details

template<size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr auto get(pair<T0,T1>&& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept -> decltype(details::pair::get<I>(move(pair))) {
    return details::pair::get<I>(move(pair));
}

template<size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr auto get(pair<T0,T1> const&& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept -> decltype(details::pair::get<I>(move(pair))) {
    return details::pair::get<I>(move(pair));
}

template<size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr auto get(pair<T0,T1> const& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept -> decltype(details::pair::get<I>(pair)) {
    return details::pair::get<I>(move(pair));
}

template<size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr auto get(pair<T0,T1>& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept -> decltype(details::pair::get<I>(pair)) {
    return details::pair::get<I>(move(pair));
}

template<typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T0&& get(pair<T0,T1>&& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept {
    return move(pair).first;
}

template<typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T0 const&& get(pair<T0,T1> const&& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept {
    return move(pair).first;
}

template<typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T0 const& get(pair<T0,T1> const& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept {
    return pair.first;
}

template<typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T0& get(pair<T0,T1>& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept {
    return pair.first;
}

template<typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T1&& get(pair<T0,T1>&& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept {
    return move(pair).second;
}

template<typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T1 const&& get(pair<T0,T1> const&& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept {
    return move(pair).second;
}

template<typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T1 const& get(pair<T0,T1> const& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept {
    return pair.second;
}

template<typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T1& get(pair<T0,T1>& pair UTL_ATTRIBUTES(LIFETIMEBOUND)) noexcept {
    return pair.second;
}

UTL_END_REGION(pair_get)

namespace details {
namespace pair {

template<typename, typename>
struct invalid_swap_t {
    inline UTL_CONSTEXPR_CXX14 void swap(invalid_swap_t& other) const noexcept {}
    inline UTL_CONSTEXPR_CXX14 void swap(invalid_swap_t const& other) const noexcept {}
};

}   // namespace pair
}   // namespace details

template<typename T0, typename T1>
struct pair : private variadic_traits<T0,T1> {
private:
    using traits = variadic_traits<T0,T1>;
    using this_type = pair<T0,T1>;
    using swap_type = conditional_t<traits::template is_swappable, pair, details::pair::invalid_swap_t<T0,T1>>;
    using const_swap_type = conditional_t<traits::template is_const_swappable, pair, details::pair::invalid_swap_t<T0,T1>> const;

    template <typename V, typename...>
    using defer = V;

    template<typename U0, typename U1>
    UTL_CONSTEXPR_CXX14 pair& assign(U0&& other_first, U1&& other_second) noexcept(traits::template is_nothrow_assignable<U0, U1>::value) {
        first = forward<U0>(other_first);
        second = forward<U1>(other_second);
        return *this;
    }

    template<typename U0, typename U1>
    UTL_CONSTEXPR_CXX14 pair const& assign(U0&& other_first, U1&& other_second) const noexcept(traits::template is_nothrow_const_assignable<U0, U1>::value) {
        first = forward<U0>(other_first);
        second = forward<U1>(other_second);
        return *this;
    }

    template<template<typename...> class L0, template<typename...> class L1,
        typename... Args0, typename... Args1,
        size_t... Is, size_t... Js>
    constexpr pair(L0<Args0...>& l0, L1<Args1...>& l1, index_sequence<Is...>, index_sequence<Js...>) noexcept(
        conjunction<
            TT_SCOPE is_nothrow_gettable<Is, L0<Args0>&>...,
            TT_SCOPE is_nothrow_gettable<Js, L1<Args1>&>...,
            UTL_SCOPE is_nothrow_constructible<T0, Args0...>,
            UTL_SCOPE is_nothrow_constructible<T1, Args1...>
        >::value
    ) : first(forward<Args0>(TT_SCOPE get<Is>(l0))...), second(forward<Args1>(TT_SCOPE get<Js>(l1))...) {}

public:
    using first_type = T0;
    using second_type = T1;
    T0 first;
    T1 second;

    constexpr pair(pair const&) noexcept(traits::is_nothrow_copy_constructible) = default;
    constexpr pair(pair&&) noexcept(traits::is_nothrow_move_constructible) = default;
    UTL_CONSTEXPR_CXX14 pair& operator=(pair const&) noexcept(traits::is_nothrow_copy_assignable) = default;
    UTL_CONSTEXPR_CXX14 pair& operator=(pair&&) noexcept(traits::is_nothrow_move_assignable) = default;

    template<typename U0 = T0, typename U1 = T1, typename V = defer<bool_constant<traits::is_const_copy_assignable>, T0, T1>>
    UTL_CONSTEXPR_CXX14 enable_if_t<V::value,
    pair const&> operator=(pair const& other) const noexcept(traits::is_nothrow_const_copy_assignable) {
        return assign(other.first, other.second);
    }

    template<typename U0 = T0, typename U1 = T1, typename V = defer<bool_constant<traits::is_const_move_assignable>, T0, T1>>
    UTL_CONSTEXPR_CXX14 enable_if_t<V::value,
    pair const&> operator=(pair&& other) const noexcept(traits::is_nothrow_const_move_assignable) {
        return assign(other.first, other.second);
    }


public:
    template<typename U0 = T0, typename U1 = T1>
    UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_swappable_with<U0&, U1&>::value
    > swap(pair<U0, U1>& other) noexcept(
        traits::template is_nothrow_swappable_with<U0&, U1&>::value
    ) {
        using UTL_SCOPE swap;
        swap(first, other.first);
        swap(second, other.second);
    }
    template<typename U0 = T0, typename U1 = T1>
    UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_swappable_with<U0 const&, U1 const&>::value
    > swap(pair<U0, U1> const& other) noexcept(
        traits::template is_nothrow_swappable_with<U0 const&, U1 const&>::value
    ) {
        using UTL_SCOPE swap;
        swap(first, other.first);
        swap(second, other.second);
    }

    template<typename U0 = T0, typename U1 = T1>
    UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_const_swappable_with<U0 const&, U1 const&>::value
    > swap(pair<U0, U1> const& other) const noexcept(
        traits::template is_nothrow_const_swappable_with<U0 const&,U1 const&>::value
    ) {
        using UTL_SCOPE swap;
        swap(first, other.first);
        swap(second, other.second);
    }

    template<typename U0 = T0, typename U1 = T1>
    UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_const_swappable_with<U0&, U1&>::value
    > swap(pair<U0, U1>& other) const noexcept(
        traits::template is_nothrow_const_swappable_with<U0&,U1&>
    ) {
        using UTL_SCOPE swap;
        swap(first, other.first);
        swap(second, other.second);
    }

    friend inline UTL_CONSTEXPR_CXX14 void swap(swap_type& l, swap_type& r) noexcept(traits::is_nothrow_swappable) {
        l.swap(r);
    }

    friend inline UTL_CONSTEXPR_CXX14 void swap(const_swap_type& l, const_swap_type& r) noexcept(traits::is_nothrow_const_swappable) {
        l.swap(r);
    }


public:
    template<typename U0 = T0, typename U1 = T1,
        enable_if_t<
            defer<bool_constant<traits::is_implicit_default_constructible>, U0, U1>::value,
        int> = 0>
    constexpr pair() noexcept(
        traits::is_nothrow_default_constructible
    ) : first(), second() {}


    template<typename U0 = T0, typename U1 = T1,
        enable_if_t<
            defer<bool_constant<traits::is_explicit_default_constructible>, U0, U1>::value,
        int> = 1>
    explicit constexpr pair() noexcept(
        traits::is_nothrow_default_constructible
    ) : first(), second() {}

public:
    template<typename U0 = T0, typename U1 = T1,
        enable_if_t<
            traits::template is_implicit_constructible<U0 const&, U1 const&>::value,
        bool> = true
    >
    constexpr pair(T0 const& t0, T1 const& t1) noexcept(
        traits::is_nothrow_copy_constructible
    ) : first(t0), second(t1) {}

    template<typename U0 = T0, typename U1 = T1,
        enable_if_t<
            traits::template is_explicit_constructible<U0 const&, U1 const&>::value,
        bool> = false
    >
    explicit constexpr pair(T0 const& t0, T1 const& t1) noexcept(
        traits::is_nothrow_copy_constructible
    ) : first(t0), second(t1) {}


public:
    template<typename U0 = T0, typename U1 = T1,
        enable_if_t<
            traits::template is_implicit_constructible<U0&&, U1&&>::value,
        bool> = true,
        enable_if_t<!traits::template is_dangling<U0&&, U1&&>, bool> = true
    >
    constexpr pair(U0&& u0, U1&& u1) noexcept(
        traits::template is_nothrow_constructible<U0, U1>::value
    ) : first(forward<U0>(u0)), second(forward<U1>(u1)) {}

    template<typename U0 = T0, typename U1 = T1,
        enable_if_t<
            traits::template is_explicit_constructible<U0&&, U1&&>::value,
        bool> = false,
        enable_if_t<!traits::template is_dangling<U0&&, U1&&>, bool> = true
    >
    explicit constexpr pair(U0&& u0, U1&& u1) noexcept(
        traits::template is_nothrow_constructible<U0, U1>::value
    ) : first(forward<U0>(u0)), second(forward<U1>(u1)) {}

    template<typename U0 = T0, typename U1 = T1,
        enable_if_t<
            traits::template is_implicit_constructible<U0&&, U1&&>::value,
        bool> = true,
        enable_if_t<traits::template is_dangling<U0&&, U1&&>, bool> = false
    >
    constexpr pair(U0&& u0, U1&& u1) noexcept(
        traits::template is_nothrow_constructible<U0, U1>::value
    ) = delete;

    template<typename U0 = T0, typename U1 = T1,
        enable_if_t<
            traits::template is_explicit_constructible<U0&&, U1&&>::value,
        bool> = false,
        enable_if_t<traits::template is_dangling<U0&&, U1&&>, bool> = false
    >
    explicit constexpr pair(U0&& u0, U1&& u1) noexcept(
        traits::template is_nothrow_constructible<U0, U1>::value
    ) = delete;


public:
    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_implicit_constructible<U0&, U1&>::value &&
            !traits::template is_dangling<U0&, U1&>::value,
        int> = 0
    >
    constexpr pair(pair<U0, U1>& p) noexcept(
        traits::template is_nothrow_constructible<U0&, U1&>::value
    ) : first(p.first), second(p.second) {}

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_explicit_constructible<U0&, U1&>::value &&
            !traits::template is_dangling<U0&, U1&>::value,
        int> = 1
    >
    explicit constexpr pair(pair<U0, U1>& p) noexcept(
        traits::template is_nothrow_constructible<U0&, U1&>::value
    ) : first(p.first), second(p.second) {}

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_implicit_constructible<U0&, U1&>::value &&
            traits::template is_dangling<U0&, U1&>::value,
        int> = 2
    >
    constexpr pair(pair<U0, U1>& p) noexcept(
        traits::template is_nothrow_constructible<U0&, U1&>::value
    ) = delete;

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_explicit_constructible<U0&, U1&>::value &&
            traits::template is_dangling<U0&, U1&>::value,
        int> = 3
    >
    explicit constexpr pair(pair<U0, U1>& p) noexcept(
        traits::template is_nothrow_constructible<U0&, U1&>::value
    ) = delete;


public:
    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_implicit_constructible<U0 const&, U1 const&>::value &&
            !traits::template is_dangling<U0 const&, U1 const&>::value,
        int> = 0
    >
    constexpr pair(pair<U0, U1> const& p) noexcept(
        traits::template is_nothrow_constructible<U0 const&, U1 const&>::value
    ) : first(p.first), second(p.second) {}

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_explicit_constructible<U0 const&, U1 const&>::value &&
            !traits::template is_dangling<U0 const&, U1 const&>::value,
        int> = 1
    >
    explicit constexpr pair(pair<U0, U1> const& p) noexcept(
        traits::template is_nothrow_constructible<U0 const&, U1 const&>::value
    ) : first(p.first), second(p.second) {}

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_implicit_constructible<U0 const&, U1 const&>::value &&
            traits::template is_dangling<U0 const&, U1 const&>::value,
        int> = 2
    >
    constexpr pair(pair<U0, U1> const&) noexcept(
        traits::template is_nothrow_constructible<U0 const&, U1 const&>::value
    ) = delete;

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_explicit_constructible<U0 const&, U1 const&>::value &&
            traits::template is_dangling<U0 const&, U1 const&>::value,
        int> = 3
    >
    explicit constexpr pair(pair<U0, U1> const&) noexcept(
        traits::template is_nothrow_constructible<U0 const&, U1 const&>::value
    ) = delete;


public:
    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_implicit_constructible<U0&&, U1&&>::value &&
            !traits::template is_dangling<U0&&, U1&&>::value,
        int> = 0
    >
    constexpr pair(pair<U0, U1>&& p) noexcept(
        traits::template is_nothrow_constructible<U0&&, U1&&>::value
    ) : first(p.first), second(p.second) {}

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_explicit_constructible<U0&&, U1&&>::value &&
            !traits::template is_dangling<U0&&, U1&&>::value,
        int> = 1
    >
    explicit constexpr pair(pair<U0, U1>&& p) noexcept(
        traits::template is_nothrow_constructible<U0&&, U1&&>::value
    ) : first(p.first), second(p.second) {}

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_implicit_constructible<U0&&, U1&&>::value &&
            traits::template is_dangling<U0&&, U1&&>::value,
        int> = 2
    >
    constexpr pair(pair<U0, U1>&&) noexcept(
        traits::template is_nothrow_constructible<U0&&, U1&&>::value
    ) = delete;

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_explicit_constructible<U0&&, U1&&>::value &&
            traits::template is_dangling<U0&&, U1&&>::value,
        int> = 3
    >
    explicit constexpr pair(pair<U0, U1>&&) noexcept(
        traits::template is_nothrow_constructible<U0&&, U1&&>::value
    ) = delete;


public:
    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_implicit_constructible<U0 const&&, U1 const&&>::value &&
            !traits::template is_dangling<U0 const&&, U1 const&&>::value,
        int> = 0
    >
    constexpr pair(pair<U0, U1> const&& p) noexcept(
        traits::template is_nothrow_constructible<U0 const&&, U1 const&&>::value
    ) : first(p.first), second(p.second) {}

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_explicit_constructible<U0 const&&, U1 const&&>::value &&
            !traits::template is_dangling<U0 const&&, U1 const&&>::value,
        int> = 1
    >
    explicit constexpr pair(pair<U0, U1> const&& p) noexcept(
        traits::template is_nothrow_constructible<U0 const&&, U1 const&&>::value
    ) : first(p.first), second(p.second) {}

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_implicit_constructible<U0 const&&, U1 const&&>::value &&
            traits::template is_dangling<U0 const&&, U1 const&&>::value,
        int> = 2
    >
    constexpr pair(pair<U0, U1> const&&) noexcept(
        traits::template is_nothrow_constructible<U0 const&&, U1 const&&>::value
    ) = delete;

    template<typename U0, typename U1,
        enable_if_t<
            traits::template is_explicit_constructible<U0 const&&, U1 const&&>::value &&
            traits::template is_dangling<U0 const&&, U1 const&&>::value,
        int> = 3
    >
    explicit constexpr pair(pair<U0, U1> const&&) noexcept(
        traits::template is_nothrow_constructible<U0 const&&, U1 const&&>::value
    ) = delete;


public:
    template<typename P,
        enable_if_t<
            TT_SCOPE is_all_gettable<P, 2>::value &&
            TT_SCOPE rebind_references_t<traits::template is_implicit_constructible, P, 2>::value &&
            !TT_SCOPE rebind_references_t<traits::template is_dangling, P, 2>::value,
        int> = 0
    >
    constexpr pair(P&& p) noexcept(
        TT_SCOPE is_all_nothrow_gettable<P, 2>::value &&
        TT_SCOPE rebind_references_t<traits::template is_nothrow_constructible, P, 2>::value
    ) : first(TT_SCOPE get<0>(forward<P>(p))), second(TT_SCOPE get<1>(forward<P>(p))) {}

    template<typename P,
        enable_if_t<
            TT_SCOPE is_all_gettable<P, 2>::value &&
            TT_SCOPE rebind_references_t<traits::template is_explicit_constructible, P, 2>::value &&
            !TT_SCOPE rebind_references_t<traits::template is_dangling, P, 2>::value,
        int> = 1
    >
    explicit constexpr pair(P&& p) noexcept(
        TT_SCOPE is_all_nothrow_gettable<P, 2>::value &&
        TT_SCOPE rebind_references_t<traits::template is_nothrow_constructible, P, 2>::value
    ) : first(TT_SCOPE get<0>(forward<P>(p))), second(TT_SCOPE get<1>(forward<P>(p))) {}

    template<typename P,
        enable_if_t<
            TT_SCOPE is_all_gettable<P, 2>::value &&
            TT_SCOPE rebind_references_t<traits::template is_implicit_constructible, P, 2>::value &&
            TT_SCOPE rebind_references_t<traits::template is_dangling, P, 2>::value,
        int> = 2
    >
    constexpr pair(P&& p) noexcept(
        TT_SCOPE is_all_nothrow_gettable<P, 2>::value &&
        TT_SCOPE rebind_references_t<traits::template is_nothrow_constructible, P, 2>::value
    ) = delete;

    template<typename P,
        enable_if_t<
            TT_SCOPE is_all_gettable<P, 2>::value &&
            TT_SCOPE rebind_references_t<traits::template is_explicit_constructible, P, 2>::value &&
            TT_SCOPE rebind_references_t<traits::template is_dangling, P, 2>::value,
        int> = 3
    >
    explicit constexpr pair(P&& p) noexcept(
        TT_SCOPE is_all_nothrow_gettable<P, 2>::value &&
        TT_SCOPE rebind_references_t<traits::template is_nothrow_constructible, P, 2>::value
    ) = delete;


public:
    // TODO change to tuple-like
    template<template<typename...> class L0, template<typename...> class L1, typename... Args0, typename... Args1, typename =
        enable_if_t<
            UTL_SCOPE is_constructible<T0, Args0...>::value &&
            UTL_SCOPE is_constructible<T1, Args1...>::value
        >
    >
    constexpr pair(piecewise_construct_t, L0<Args0...> l0, L1<Args1...> l1)
        noexcept(
            UTL_SCOPE is_nothrow_constructible<pair,
                L0<Args0...>&, L1<Args1...>&, index_sequence_for<Args0...>, index_sequence_for<Args1...>>::value
        ) : pair(l0, l1, index_sequence_for<Args0...>{}, index_sequence_for<Args1...>{}) {}

    template<typename... Args0, typename... Args1, typename =
        enable_if_t<
            UTL_SCOPE is_constructible<T0, Args0...>::value &&
            UTL_SCOPE is_constructible<T1, Args1...>::value
        >
    >
    constexpr pair(piecewise_construct_t, tuple<Args0...> l0, tuple<Args1...> l1)
        noexcept(
            UTL_SCOPE is_nothrow_constructible<pair,
                tuple<Args0...>&, tuple<Args1...>&, index_sequence_for<Args0...>, index_sequence_for<Args1...>>::value
        );


public:
    template<typename U0, typename U1>
    UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_assignable<U0 const&, U1 const&>::value,
    pair&> operator=(pair<U0, U1> const& p) noexcept(
        traits::template is_nothrow_assignable<U0 const&, U1 const&>::value
    ) {
        return assign(p.first, p.second);
    }

    template<typename U0, typename U1>
    UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_const_assignable<U0 const&, U1 const&>::value,
    pair const&> operator=(pair<U0, U1> const& p) const noexcept(
        traits::template is_nothrow_const_assignable<U0 const&, U1 const&>::value
    ) {
        return assign(p.first, p.second);
    }


public:
    template<typename U0, typename U1>
    UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_assignable<U0&&, U1&&>::value,
    pair&> operator=(pair<U0, U1>&& p) noexcept(
        traits::template is_nothrow_assignable<U0&&, U1&&>::value
    ) {
        return assign(move(p).first, move(p).second);
    }

    template<typename U0, typename U1>
    UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_const_assignable<U0&&, U1&&>::value,
    pair const&> operator=(pair<U0, U1>&& p) const noexcept(
        traits::template is_nothrow_const_assignable<U0&&, U1&&>::value
    ) {
        return assign(move(p).first, move(p).second);
    }


public:
    template<typename P>
    UTL_CONSTEXPR_CXX14 enable_if_t<
        tuple_size<P>::value == 2 &&
        conjunction<
            TT_SCOPE is_all_gettable<P>,
            TT_SCOPE rebind_references_t<traits::template is_assignable, P>
        >::value,
    pair&> operator=(P&& p) noexcept(
        TT_SCOPE is_all_nothrow_gettable<P>::value &&
        TT_SCOPE rebind_references_t<traits::template is_nothrow_assignable, P>::value
    ) {
        return assign(TT_SCOPE get<0>(forward<P>(p)), TT_SCOPE get<1>(forward<P>(p)));
    }

    template<typename P>
    UTL_CONSTEXPR_CXX14 enable_if_t<
        tuple_size<P>::value == 2 &&
        conjunction<
            TT_SCOPE is_all_gettable<P>,
            TT_SCOPE rebind_references_t<traits::template is_const_assignable, P>
        >::value,
    pair const&> operator=(P&& p) const noexcept(
        TT_SCOPE is_all_nothrow_gettable<P, 2>::value &&
        TT_SCOPE rebind_references_t<traits::template is_nothrow_const_assignable, P, 2>::value
    ) {
        return assign(TT_SCOPE get<0>(forward<P>(p)), TT_SCOPE get<1>(forward<P>(p)));
    }

    UTL_CONSTEXPR_CXX20 ~pair() = default;
};  // pair

#define UTL_DEFINE_PAIR_PIECEWISE_CONSTRUCTOR() \
template<typename T0, typename T1> template<typename... Args0, typename... Args1, typename> \
constexpr pair<T0, T1>::pair(piecewise_construct_t, tuple<Args0...> l0, tuple<Args1...> l1) \
    noexcept(UTL_SCOPE is_nothrow_constructible<pair, \
        tuple<Args0...>&, tuple<Args1...>&, index_sequence_for<Args0...>, index_sequence_for<Args1...>>::value \
    ) : pair(l0, l1, index_sequence_for<Args0...>{}, index_sequence_for<Args1...>{}) {}

template<typename T0, typename T1, typename U0, typename U1>
UTL_NODISCARD constexpr enable_if_t<conjunction<
        is_equality_comparable<T0,U0>,
        is_equality_comparable<T1,U1>
    >::value,
bool> operator==(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(conjunction<
        is_nothrow_equality_comparable<T0,U0>,
        is_nothrow_equality_comparable<T1,U1>
    >::value) {
    return static_cast<bool>(l.first == r.first) && static_cast<bool>(l.second == r.second);
}

template<typename T0, typename T1, typename U0, typename U1>
UTL_NODISCARD constexpr enable_if_t<conjunction<
        is_equality_comparable<T0,U0>,
        is_less_comparable<T0,U0>,
        is_less_comparable<T1,U1>
    >::value,
bool> operator<(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(conjunction<
        is_nothrow_equality_comparable<T0,U0>,
        is_nothrow_less_comparable<T0,U0>,
        is_nothrow_less_comparable<T1,U1>
    >::value) {
    return static_cast<bool>(l.first < r.first) || (static_cast<bool>(l.first == r.first) && static_cast<bool>(l.second < r.second));
}

template<typename T0, typename T1, typename U0, typename U1>
UTL_NODISCARD constexpr enable_if_t<
    is_equality_comparable<pair<T0,T1>,pair<U0,U1>>::value,
bool> operator!=(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(
    is_nothrow_equality_comparable<pair<T0,T1>,pair<U0,U1>>::value) {
    return !static_cast<bool>(l == r);
}

template<typename T0, typename T1, typename U0, typename U1>
UTL_NODISCARD constexpr enable_if_t<
    is_less_comparable<pair<U0,U1>, pair<T0,T1>>::value,
bool> operator<=(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(
    is_nothrow_less_comparable<pair<U0,U1>, pair<T0,T1>>::value) {
    return !static_cast<bool>(r < l);
}

template<typename T0, typename T1, typename U0, typename U1>
UTL_NODISCARD constexpr enable_if_t<
    is_less_comparable<pair<U0,U1>, pair<T0,T1>>::value,
bool> operator>(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(
    is_nothrow_less_comparable<pair<U0,U1>, pair<T0,T1>>::value) {
    return static_cast<bool>(r < l);
}

template<typename T0, typename T1, typename U0, typename U1>
UTL_NODISCARD constexpr enable_if_t<
    is_less_comparable<pair<T0,T1>, pair<U0,U1>>::value,
bool> operator>=(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(
    is_nothrow_less_comparable<pair<T0,T1>, pair<U0,U1>>::value) {
    return !static_cast<bool>(l < r);
}

template<typename T, typename U>
constexpr pair<unwrap_reference_t<decay_t<T>>, unwrap_reference_t<decay_t<U>>> make_pair(T&& t, U&& u) {
    return pair<unwrap_reference_t<decay_t<T>>, unwrap_reference_t<decay_t<U>>>{ forward<T>(t), forward<U>(u) };
}

#endif    // defined(UTL_USE_STDPAIR) && UTL_USE_STDPAIR

UTL_NAMESPACE_END

#ifndef UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace pair {
    template<typename T0, typename T1, typename U0, typename U1, typename = void>
    struct common_type_impl {};

    template<typename T0, typename T1, typename U0, typename U1>
    struct common_type_impl<T0,T1,U0,U1, void_t<common_type_t<T0, U0>, common_type_t<T1, U1>>> {
        using type = UTL_SCOPE pair<common_type_t<T0, U0>, common_type_t<T1, U1>>;
    };

    template<typename T0, typename T1, typename U0, typename U1, template<typename> class TQual, template<typename> class UQual, typename = void>
    struct basic_ref_impl {};

    template<typename T0, typename T1, typename U0, typename U1, template<typename> class TQual, template<typename> class UQual>
    struct basic_ref_impl<T0,T1,U0,U1,TQual,UQual, void_t<
        common_reference_t<TQual<T0>, UQual<U0>>,
        common_reference_t<TQual<T1>, UQual<U1>>
    >> {
        using type = UTL_SCOPE pair<
            common_reference_t<TQual<T0>, UQual<U0>>,
            common_reference_t<TQual<T1>, UQual<U1>>
        >;
    };
}   // namespace pair
}   // namespace details

UTL_NAMESPACE_END

namespace std {

template<typename T0, typename T1, typename U0, typename U1>
struct common_type<pair<T0,T1>, pair<U0,U1>> : UTL_SCOPE details::pair::common_type_impl<T0,T1,U0,U1> {};

template<typename T0, typename T1, typename U0, typename U1, template<typename> class TQual, template<typename> class UQual>
struct basic_common_reference<pair<T0, T1>, pair<U0, U1>, TQual, UQual> : UTL_SCOPE details::pair::basic_ref_impl<T0,T1,U0,U1,TQual,UQual> {};    

}

#else   // ifndef UTL_CXX20

namespace std {
template<typename T0, typename T1, typename U0, typename U1>
requires requires {
    typename pair<
        common_type_t<T0, U0>,
        common_type_t<T1, U1>
    >;
}
struct common_type<pair<T0,T1>, pair<U0,U1>> {
    using type = pair<common_type_t<T0, U0>, common_type_t<T1, U1>>;
};

template<typename T0, typename T1, typename U0, typename U1, template<typename> class TQual, template<typename> class UQual>
requires requires {
    typename pair<
        common_reference_t<TQual<T0>, UQual<U0>>,
        common_reference_t<TQual<T1>, UQual<U1>>
    >;
}
struct basic_common_reference<pair<T0, T1>, pair<U0, U1>, TQual, UQual> {
    using type = pair<
        common_reference_t<TQual<T0>, UQual<U0>>,
        common_reference_t<TQual<T1>, UQual<U1>>
    >;
};
}
#endif  // ifndef UTL_CXX20

UTL_STD_NAMESPACE_BEGIN
template<typename T0, typename T1>
struct tuple_size<UTL_SCOPE pair<T0, T1>> : UTL_SCOPE template_size<UTL_SCOPE pair<T0, T1>> {};

template<size_t I, typename T0, typename T1>
struct tuple_element<I, UTL_SCOPE pair<T0, T1>> : UTL_SCOPE template_element<I, UTL_SCOPE pair<T0, T1>> {};
UTL_STD_NAMESPACE_END

#undef TT_SCOPE
