// Copyright 2023-2024 Bryan Wong

#pragma once

/** TODO cleanup this implementation and support allocator_arg **/

#if UTL_USE_STDPAIR

#  include "utl/preprocessor/utl_namespace.h"

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::get;
using std::make_pair;
using std::pair;
using std::piecewise_construct;
using std::piecewise_construct_t;
using std::tuple_element;
using std::tuple_size;

UTL_NAMESPACE_END

#else // UTL_USE_STDPAIR

#  include "utl/preprocessor/utl_config.h"

#  include "utl/compare/utl_compare_traits.h"
#  include "utl/concepts/utl_common_with.h"
#  include "utl/tuple/utl_tuple_concepts.h"
#  include "utl/tuple/utl_tuple_get_element.h"
#  include "utl/tuple/utl_tuple_traits.h"
#  include "utl/type_traits/utl_common_reference.h"
#  include "utl/type_traits/utl_common_type.h"
#  include "utl/type_traits/utl_decay.h"
#  include "utl/type_traits/utl_is_equality_comparable.h"
#  include "utl/type_traits/utl_template_list.h"
#  include "utl/type_traits/utl_unwrap_reference.h"
#  include "utl/type_traits/utl_variadic_traits.h"
#  include "utl/utility/utl_forward.h"
#  include "utl/utility/utl_move.h"
#  include "utl/utility/utl_sequence.h"

UTL_NAMESPACE_BEGIN

template <typename T0, typename T1>
struct pair;

template <typename...>
class tuple;

template <typename T>
struct UTL_PUBLIC_TEMPLATE is_pair : false_type {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_pair<T&&> : is_pair<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_pair<T&> : is_pair<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_pair<T const> : is_pair<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_pair<T volatile> : is_pair<T> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_pair<T const volatile> : is_pair<T> {};
template <typename T, typename U>
struct UTL_PUBLIC_TEMPLATE is_pair<pair<T, U>> : true_type {};

struct UTL_ABI_PUBLIC piecewise_construct_t {
    UTL_HIDE_FROM_ABI explicit constexpr piecewise_construct_t() noexcept = default;
};

/// Tag for piecewise construction of pair objects.
UTL_INLINE_CXX17 constexpr piecewise_construct_t piecewise_construct{};

namespace details {
namespace pair {
template <size_t I, typename P> UTL_REQUIRES_CXX20(I == 0 && requires(P&& p) {
        requires is_pair<P>::value;
        UTL_SCOPE forward<P>(p).first;
    })
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get(P&& p UTL_LIFETIMEBOUND) noexcept
    -> UTL_ENABLE_IF_CXX11(decltype(UTL_SCOPE declval<P>().first), (I == 0 && UTL_SCOPE is_pair<P>::value)) {
    return UTL_SCOPE forward<P>(p).first;
}

template <size_t I, typename P> UTL_REQUIRES_CXX20(I == 1 && requires(P&& p) {
        requires is_pair<P>::value;
        UTL_SCOPE forward<P>(p).second;
    })
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get(P&& p UTL_LIFETIMEBOUND) noexcept
    -> UTL_ENABLE_IF_CXX11(decltype(UTL_SCOPE declval<P>().second), (I == 0 && UTL_SCOPE is_pair<P>::value)) {
    return UTL_SCOPE forward<P>(p).second;
}
} // namespace pair
} // namespace details

template <size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get(pair<T0, T1>&& pair UTL_LIFETIMEBOUND) noexcept -> decltype(details::pair::get<I>(move(pair))) {
    return details::pair::get<I>(move(pair));
}

template <size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get(pair<T0, T1> const&& pair UTL_LIFETIMEBOUND) noexcept -> decltype(details::pair::get<I>(move(pair))) {
    return details::pair::get<I>(move(pair));
}

template <size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get(pair<T0, T1> const& pair UTL_LIFETIMEBOUND) noexcept -> decltype(details::pair::get<I>(pair)) {
    return details::pair::get<I>(move(pair));
}

template <size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get(
    pair<T0, T1>& pair UTL_LIFETIMEBOUND) noexcept -> decltype(details::pair::get<I>(pair)) {
    return details::pair::get<I>(move(pair));
}

template <typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr T0&& get(
    pair<T0, T1>&& pair UTL_LIFETIMEBOUND) noexcept {
    return move(pair).first;
}

template <typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr T0 const&& get(
    pair<T0, T1> const&& pair UTL_LIFETIMEBOUND) noexcept {
    return move(pair).first;
}

template <typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr T0 const& get(
    pair<T0, T1> const& pair UTL_LIFETIMEBOUND) noexcept {
    return pair.first;
}

template <typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr T0& get(
    pair<T0, T1>& pair UTL_LIFETIMEBOUND) noexcept {
    return pair.first;
}

template <typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr T1&& get(
    pair<T0, T1>&& pair UTL_LIFETIMEBOUND) noexcept {
    return move(pair).second;
}

template <typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr T1 const&& get(
    pair<T0, T1> const&& pair UTL_LIFETIMEBOUND) noexcept {
    return move(pair).second;
}

template <typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr T1 const& get(
    pair<T0, T1> const& pair UTL_LIFETIMEBOUND) noexcept {
    return pair.second;
}

template <typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr T1& get(
    pair<T0, T1>& pair UTL_LIFETIMEBOUND) noexcept {
    return pair.second;
}

namespace details {
namespace pair {

template <typename, typename>
struct invalid_swap_t {
    UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(invalid_swap_t& other) const noexcept {}
    UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(invalid_swap_t const& other) const noexcept {}
};

} // namespace pair
} // namespace details

template <typename T0, typename T1>
struct pair {
private:
    using traits UTL_NODEBUG = variadic_traits<T0, T1>;
    using this_type UTL_NODEBUG = pair<T0, T1>;
    using swap_type UTL_NODEBUG =
        conditional_t<traits::is_swappable, pair, details::pair::invalid_swap_t<T0, T1>>;
    using const_swap_type UTL_NODEBUG = conditional_t<traits::is_const_swappable, pair,
        details::pair::invalid_swap_t<T0, T1>> const;

    template <typename V, typename...>
    using defer UTL_NODEBUG = V;

    template <typename U0, typename U1>
    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE)
    UTL_CONSTEXPR_CXX14 pair& assign(U0&& other_first,
        U1&& other_second) noexcept(traits::template is_nothrow_assignable<U0, U1>::value) {
        first = forward<U0>(other_first);
        second = forward<U1>(other_second);
        return *this;
    }

    template <typename U0, typename U1>
    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE) UTL_CONSTEXPR_CXX14 pair const& assign(
        U0&& other_first, U1&& other_second) const
        noexcept(traits::template is_nothrow_const_assignable<U0, U1>::value) {
        first = forward<U0>(other_first);
        second = forward<U1>(other_second);
        return *this;
    }

    template <typename L0, typename L1, size_t... Is, size_t... Js>
    UTL_REQUIRES_CXX20(tuple_like<remove_reference_t<L0>> && tuple_like<remove_reference_t<L1>>)
    UTL_HIDE_FROM_ABI constexpr pair(L0& l0, L1& l1, index_sequence<Is...>,
        index_sequence<Js...>) noexcept(conjunction<details::tuple::is_all_nothrow_gettable<L0&>,
        details::tuple::is_all_nothrow_gettable<L1&>,
        UTL_SCOPE is_nothrow_constructible<T0, tuple_element_t<Is, L0>...>,
        UTL_SCOPE is_nothrow_constructible<T1, tuple_element_t<Js, L1>...>>::value)
        : first(UTL_SCOPE forward<tuple_element_t<Is, L0>>(UTL_SCOPE get_element<Is>(l0))...)
        , second(UTL_SCOPE forward<tuple_element_t<Js, L1>>(UTL_SCOPE get_element<Js>(l1))...) {}

public:
    using first_type UTL_NODEBUG = T0;
    using second_type UTL_NODEBUG = T1;
    T0 first;
    T1 second;

    UTL_HIDE_FROM_ABI constexpr pair(pair const&) noexcept(
        traits::is_nothrow_copy_constructible) = default;
    UTL_HIDE_FROM_ABI constexpr pair(pair&&) noexcept(
        traits::is_nothrow_move_constructible) = default;
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 pair& operator=(pair const&) noexcept(
        traits::is_nothrow_copy_assignable) = default;
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 pair& operator=(pair&&) noexcept(
        traits::is_nothrow_move_assignable) = default;

    template <typename U0 = T0, typename U1 = T1,
        typename V = defer<bool_constant<traits::is_const_copy_assignable>, T0, T1>>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<V::value, pair const&> operator=(
        pair const& other) const noexcept(traits::is_nothrow_const_copy_assignable) {
        return assign(other.first, other.second);
    }

    template <typename U0 = T0, typename U1 = T1,
        typename V = defer<bool_constant<traits::is_const_move_assignable>, T0, T1>>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<V::value, pair const&> operator=(pair&& other) const
        noexcept(traits::is_nothrow_const_move_assignable) {
        return assign(other.first, other.second);
    }

public:
    template <typename U0 = T0, typename U1 = T1>
    UTL_HIDE_FROM_ABI
        UTL_CONSTEXPR_CXX14 enable_if_t<traits::template is_swappable_with<U0&, U1&>::value>
    swap(pair<U0, U1>& other) noexcept(
        traits::template is_nothrow_swappable_with<U0&, U1&>::value) {
        using UTL_SCOPE swap;
        swap(first, other.first);
        swap(second, other.second);
    }
    template <typename U0 = T0, typename U1 = T1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_swappable_with<U0 const&, U1 const&>::value>
    swap(pair<U0, U1> const& other) noexcept(
        traits::template is_nothrow_swappable_with<U0 const&, U1 const&>::value) {
        using UTL_SCOPE swap;
        swap(first, other.first);
        swap(second, other.second);
    }

    template <typename U0 = T0, typename U1 = T1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_const_swappable_with<U0 const&, U1 const&>::value>
    swap(pair<U0, U1> const& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<U0 const&, U1 const&>::value) {
        using UTL_SCOPE swap;
        swap(first, other.first);
        swap(second, other.second);
    }

    template <typename U0 = T0, typename U1 = T1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_const_swappable_with<U0&, U1&>::value>
    swap(pair<U0, U1>& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<U0&, U1&>) {
        using UTL_SCOPE swap;
        swap(first, other.first);
        swap(second, other.second);
    }

    UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void swap(swap_type& l, swap_type& r) noexcept(
        traits::is_nothrow_swappable) {
        l.swap(r);
    }

    UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void swap(
        const_swap_type& l, const_swap_type& r) noexcept(traits::is_nothrow_const_swappable) {
        l.swap(r);
    }

public:
#  if !UTL_CXX20
    template <typename U0 = T0, typename U1 = T1 UTL_REQUIRES_CXX11(
            defer<bool_constant<traits::is_implicit_default_constructible>, U0, U1>::value)>
    UTL_HIDE_FROM_ABI constexpr pair() noexcept(traits::is_nothrow_default_constructible)
        : first()
        , second() {}

    template <typename U0 = T0, typename U1 = T1 UTL_REQUIRES_CXX11(
            defer<bool_constant<traits::is_explicit_default_constructible>, U0, U1>::value)>
    UTL_HIDE_FROM_ABI explicit constexpr pair() noexcept(traits::is_nothrow_default_constructible)
        : first()
        , second() {}
#  else
    UTL_HIDE_FROM_ABI explicit(traits::is_explicit_default_constructible) constexpr pair() noexcept(
        traits::is_nothrow_default_constructible) = default;
#  endif

public:
    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<traits::template is_implicit_constructible<U0 const&, U1 const&>::value, bool> =
            true>
    UTL_HIDE_FROM_ABI constexpr pair(T0 const& t0, T1 const& t1) noexcept(
        traits::is_nothrow_copy_constructible)
        : first(t0)
        , second(t1) {}

    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<traits::template is_explicit_constructible<U0 const&, U1 const&>::value, bool> =
            false>
    UTL_HIDE_FROM_ABI explicit constexpr pair(T0 const& t0, T1 const& t1) noexcept(
        traits::is_nothrow_copy_constructible)
        : first(t0)
        , second(t1) {}

public:
    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<traits::template is_implicit_constructible<U0&&, U1&&>::value, bool> = true,
        enable_if_t<!traits::template is_dangling<U0&&, U1&&>::value, bool> = true>
    UTL_HIDE_FROM_ABI constexpr pair(U0&& u0, U1&& u1) noexcept(
        traits::template is_nothrow_constructible<U0, U1>::value)
        : first(forward<U0>(u0))
        , second(forward<U1>(u1)) {}

    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<traits::template is_explicit_constructible<U0&&, U1&&>::value, bool> = false,
        enable_if_t<!traits::template is_dangling<U0&&, U1&&>::value, bool> = true>
    UTL_HIDE_FROM_ABI explicit constexpr pair(U0&& u0, U1&& u1) noexcept(
        traits::template is_nothrow_constructible<U0, U1>::value)
        : first(forward<U0>(u0))
        , second(forward<U1>(u1)) {}

    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<traits::template is_implicit_constructible<U0&&, U1&&>::value, bool> = true,
        enable_if_t<traits::template is_dangling<U0&&, U1&&>::value, bool> = false>
    constexpr pair(U0&& u0, U1&& u1) noexcept(
        traits::template is_nothrow_constructible<U0, U1>::value) = delete;

    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<traits::template is_explicit_constructible<U0&&, U1&&>::value, bool> = false,
        enable_if_t<traits::template is_dangling<U0&&, U1&&>::value, bool> = false>
    explicit constexpr pair(U0&& u0, U1&& u1) noexcept(
        traits::template is_nothrow_constructible<U0, U1>::value) = delete;

public:
    template <typename U0, typename U1,
        enable_if_t<traits::template is_implicit_constructible<U0&, U1&>::value &&
                !traits::template is_dangling<U0&, U1&>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(pair<U0, U1>& p) noexcept(
        traits::template is_nothrow_constructible<U0&, U1&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<traits::template is_explicit_constructible<U0&, U1&>::value &&
                !traits::template is_dangling<U0&, U1&>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(pair<U0, U1>& p) noexcept(
        traits::template is_nothrow_constructible<U0&, U1&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<traits::template is_implicit_constructible<U0&, U1&>::value &&
                traits::template is_dangling<U0&, U1&>::value,
            int> = 2>
    constexpr pair(pair<U0, U1>& p) noexcept(
        traits::template is_nothrow_constructible<U0&, U1&>::value) = delete;

    template <typename U0, typename U1,
        enable_if_t<traits::template is_explicit_constructible<U0&, U1&>::value &&
                traits::template is_dangling<U0&, U1&>::value,
            int> = 3>
    explicit constexpr pair(pair<U0, U1>& p) noexcept(
        traits::template is_nothrow_constructible<U0&, U1&>::value) = delete;

public:
    template <typename U0, typename U1,
        enable_if_t<traits::template is_implicit_constructible<U0 const&, U1 const&>::value &&
                !traits::template is_dangling<U0 const&, U1 const&>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(pair<U0, U1> const& p) noexcept(
        traits::template is_nothrow_constructible<U0 const&, U1 const&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<traits::template is_explicit_constructible<U0 const&, U1 const&>::value &&
                !traits::template is_dangling<U0 const&, U1 const&>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(pair<U0, U1> const& p) noexcept(
        traits::template is_nothrow_constructible<U0 const&, U1 const&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<traits::template is_implicit_constructible<U0 const&, U1 const&>::value &&
                traits::template is_dangling<U0 const&, U1 const&>::value,
            int> = 2>
    constexpr pair(pair<U0, U1> const&) noexcept(
        traits::template is_nothrow_constructible<U0 const&, U1 const&>::value) = delete;

    template <typename U0, typename U1,
        enable_if_t<traits::template is_explicit_constructible<U0 const&, U1 const&>::value &&
                traits::template is_dangling<U0 const&, U1 const&>::value,
            int> = 3>
    explicit constexpr pair(pair<U0, U1> const&) noexcept(
        traits::template is_nothrow_constructible<U0 const&, U1 const&>::value) = delete;

public:
    template <typename U0, typename U1,
        enable_if_t<traits::template is_implicit_constructible<U0&&, U1&&>::value &&
                !traits::template is_dangling<U0&&, U1&&>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(pair<U0, U1>&& p) noexcept(
        traits::template is_nothrow_constructible<U0&&, U1&&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<traits::template is_explicit_constructible<U0&&, U1&&>::value &&
                !traits::template is_dangling<U0&&, U1&&>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(pair<U0, U1>&& p) noexcept(
        traits::template is_nothrow_constructible<U0&&, U1&&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<traits::template is_implicit_constructible<U0&&, U1&&>::value &&
                traits::template is_dangling<U0&&, U1&&>::value,
            int> = 2>
    constexpr pair(pair<U0, U1>&&) noexcept(
        traits::template is_nothrow_constructible<U0&&, U1&&>::value) = delete;

    template <typename U0, typename U1,
        enable_if_t<traits::template is_explicit_constructible<U0&&, U1&&>::value &&
                traits::template is_dangling<U0&&, U1&&>::value,
            int> = 3>
    explicit constexpr pair(pair<U0, U1>&&) noexcept(
        traits::template is_nothrow_constructible<U0&&, U1&&>::value) = delete;

public:
    template <typename U0, typename U1,
        enable_if_t<traits::template is_implicit_constructible<U0 const&&, U1 const&&>::value &&
                !traits::template is_dangling<U0 const&&, U1 const&&>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(pair<U0, U1> const&& p) noexcept(
        traits::template is_nothrow_constructible<U0 const&&, U1 const&&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<traits::template is_explicit_constructible<U0 const&&, U1 const&&>::value &&
                !traits::template is_dangling<U0 const&&, U1 const&&>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(pair<U0, U1> const&& p) noexcept(
        traits::template is_nothrow_constructible<U0 const&&, U1 const&&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<traits::template is_implicit_constructible<U0 const&&, U1 const&&>::value &&
                traits::template is_dangling<U0 const&&, U1 const&&>::value,
            int> = 2>
    constexpr pair(pair<U0, U1> const&&) noexcept(
        traits::template is_nothrow_constructible<U0 const&&, U1 const&&>::value) = delete;

    template <typename U0, typename U1,
        enable_if_t<traits::template is_explicit_constructible<U0 const&&, U1 const&&>::value &&
                traits::template is_dangling<U0 const&&, U1 const&&>::value,
            int> = 3>
    explicit constexpr pair(pair<U0, U1> const&&) noexcept(
        traits::template is_nothrow_constructible<U0 const&&, U1 const&&>::value) = delete;

public:
    template <typename P,
        enable_if_t<details::tuple::is_all_gettable<P, 2>::value &&
                details::tuple::rebind_references_t<traits::template is_implicit_constructible, P,
                    2>::value &&
                !details::tuple::rebind_references_t<traits::template is_dangling, P, 2>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(P&& p) noexcept(
        details::tuple::is_all_nothrow_gettable<P, 2>::value &&
        details::tuple::rebind_references_t<traits::template is_nothrow_constructible, P, 2>::value)
        : first(UTL_SCOPE get_key(UTL_SCOPE forward<P>(p)))
        , second(UTL_SCOPE get_value(UTL_SCOPE forward<P>(p))) {}

    template <typename P,
        enable_if_t<details::tuple::is_all_gettable<P, 2>::value &&
                details::tuple::rebind_references_t<traits::template is_explicit_constructible, P,
                    2>::value &&
                !details::tuple::rebind_references_t<traits::template is_dangling, P, 2>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(P&& p) noexcept(
        details::tuple::is_all_nothrow_gettable<P, 2>::value &&
        details::tuple::rebind_references_t<traits::template is_nothrow_constructible, P, 2>::value)
        : first(UTL_SCOPE get_key(UTL_SCOPE forward<P>(p)))
        , second(UTL_SCOPE get_value(UTL_SCOPE forward<P>(p))) {}

    template <typename P,
        enable_if_t<details::tuple::is_all_gettable<P, 2>::value &&
                details::tuple::rebind_references_t<traits::template is_implicit_constructible, P,
                    2>::value &&
                details::tuple::rebind_references_t<traits::template is_dangling, P, 2>::value,
            int> = 2>
    constexpr pair(P&& p) noexcept(details::tuple::is_all_nothrow_gettable<P, 2>::value &&
        details::tuple::rebind_references_t<traits::template is_nothrow_constructible, P,
            2>::value) = delete;

    template <typename P,
        enable_if_t<details::tuple::is_all_gettable<P, 2>::value &&
                details::tuple::rebind_references_t<traits::template is_explicit_constructible, P,
                    2>::value &&
                details::tuple::rebind_references_t<traits::template is_dangling, P, 2>::value,
            int> = 3>
    explicit constexpr pair(P&& p) noexcept(details::tuple::is_all_nothrow_gettable<P, 2>::value &&
        details::tuple::rebind_references_t<traits::template is_nothrow_constructible, P,
            2>::value) = delete;

private:
    template <typename U, size_t... Is>
    UTL_HIDE_FROM_ABI static auto constructs_first_impl(index_sequence<Is...>) noexcept
        -> UTL_SCOPE is_constructible<T0, tuple_element_t<Is, U>...>;
    template <typename U, size_t... Is>
    UTL_HIDE_FROM_ABI static auto constructs_second_impl(index_sequence<Is...>) noexcept
        -> UTL_SCOPE is_constructible<T1, tuple_element_t<Is, U>...>;

    template <typename U>
    using constructs_first UTL_NODEBUG =
        decltype(constructs_first_impl<U>(tuple_index_sequence<U>{}));
    template <typename U>
    using constructs_second UTL_NODEBUG =
        decltype(constructs_second_impl<U>(tuple_index_sequence<U>{}));

public:
    template <typename L0,
        typename L1 UTL_REQUIRES_CXX11(conjunction<is_tuple_like<remove_reference_t<L0>>,
            is_tuple_like<remove_reference_t<L1>>, constructs_first<L0>,
            constructs_second<L1>>::value)>
    UTL_REQUIRES_CXX20(
        tuple_like<remove_reference_t<L0>>&&
            tuple_like<remove_reference_t<L1>>&& []<size_t... Is>(index_sequence<Is...>) {
                return constructible_from<T0, tuple_element_t<Is, L0>...>;
            }(tuple_index_sequence<L0>{}) &&
        []<size_t... Is>(index_sequence<Is...>) {
            return constructible_from<T1, tuple_element_t<Is, L1>...>;
        }(tuple_index_sequence<L1>{}))
    UTL_HIDE_FROM_ABI constexpr pair(piecewise_construct_t, L0 l0, L1 l1) noexcept(
        UTL_SCOPE is_nothrow_constructible<pair, L0&, L1&, tuple_index_sequence<L0>,
            tuple_index_sequence<L1>>::value)
        : pair(l0, l1, tuple_index_sequence<L0>{}, tuple_index_sequence<L1>{}) {}

    template <typename... Args0,
        typename... Args1 UTL_REQUIRES_CXX11(conjunction<UTL_SCOPE is_constructible<T0, Args0...>,
            UTL_SCOPE is_constructible<T1, Args1...>>::value)>
    UTL_REQUIRES_CXX20(constructible_from<T0, Args0...> && constructible_from<T1, Args1...>)
    UTL_HIDE_FROM_ABI constexpr pair(piecewise_construct_t, tuple<Args0...> l0,
        tuple<Args1...> l1) noexcept(UTL_SCOPE is_nothrow_constructible<pair, tuple<Args0...>&,
        tuple<Args1...>&, index_sequence_for<Args0...>, index_sequence_for<Args1...>>::value)
        : pair(l0, l1, index_sequence_for<Args0...>{}, index_sequence_for<Args1...>{}) {}

public:
    template <typename U0, typename U1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_assignable<U0 const&, U1 const&>::value, pair&>
    operator=(pair<U0, U1> const& p) noexcept(
        traits::template is_nothrow_assignable<U0 const&, U1 const&>::value) {
        return assign(p.first, p.second);
    }

    template <typename U0, typename U1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<
        traits::template is_const_assignable<U0 const&, U1 const&>::value, pair const&>
    operator=(pair<U0, U1> const& p) const
        noexcept(traits::template is_nothrow_const_assignable<U0 const&, U1 const&>::value) {
        return assign(p.first, p.second);
    }

public:
    template <typename U0, typename U1>
    UTL_HIDE_FROM_ABI
        UTL_CONSTEXPR_CXX14 enable_if_t<traits::template is_assignable<U0&&, U1&&>::value,
        pair&>
    operator=(pair<U0, U1>&& p) noexcept(
        traits::template is_nothrow_assignable<U0&&, U1&&>::value) {
        return assign(move(p).first, move(p).second);
    }

    template <typename U0, typename U1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<traits::template is_const_assignable<U0&&, U1&&>::value,
        pair const&>
    operator=(pair<U0, U1>&& p) const
        noexcept(traits::template is_nothrow_const_assignable<U0&&, U1&&>::value) {
        return assign(move(p).first, move(p).second);
    }

public:
    template <typename P>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<tuple_size<P>::value == 2 &&
            conjunction<details::tuple::is_all_gettable<P>,
                details::tuple::rebind_references_t<traits::template is_assignable, P>>::value,
        pair&>
    operator=(P&& p) noexcept(details::tuple::is_all_nothrow_gettable<P>::value &&
        details::tuple::rebind_references_t<traits::template is_nothrow_assignable, P>::value) {
        return assign(UTL_SCOPE get_key(UTL_SCOPE forward<P>(p)),
            UTL_SCOPE get_value(UTL_SCOPE forward<P>(p)));
    }

    template <typename P>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<tuple_size<P>::value == 2 &&
            conjunction<details::tuple::is_all_gettable<P>,
                details::tuple::rebind_references_t<traits::template is_const_assignable,
                    P>>::value,
        pair const&>
    operator=(P&& p) const noexcept(details::tuple::is_all_nothrow_gettable<P, 2>::value &&
        details::tuple::rebind_references_t<traits::template is_nothrow_const_assignable, P,
            2>::value) {
        return assign(UTL_SCOPE get_key(UTL_SCOPE forward<P>(p)),
            UTL_SCOPE get_value(UTL_SCOPE forward<P>(p)));
    }

    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 ~pair() = default;
}; // pair

template <typename T0, typename T1, typename U0, typename U1>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr enable_if_t<
    conjunction<is_equality_comparable_with<T0, U0>, is_equality_comparable_with<T1, U1>>::value,
    bool>
operator==(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(
    conjunction<is_nothrow_equality_comparable_with<T0, U0>,
        is_nothrow_equality_comparable_with<T1, U1>>::value) {
    return static_cast<bool>(l.first == r.first) && static_cast<bool>(l.second == r.second);
}

template <typename T0, typename T1, typename U0, typename U1>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr enable_if_t<
    conjunction<is_equality_comparable_with<T0, U0>, is_strict_subordinate_comparable_with<T0, U0>,
        is_strict_subordinate_comparable_with<T1, U1>>::value,
    bool>
operator<(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(
    conjunction<is_nothrow_equality_comparable_with<T0, U0>,
        is_nothrow_strict_subordinate_comparable_with<T0, U0>,
        is_nothrow_strict_subordinate_comparable_with<T1, U1>>::value) {
    return static_cast<bool>(l.first < r.first) ||
        (static_cast<bool>(l.first == r.first) && static_cast<bool>(l.second < r.second));
}

template <typename T0, typename T1, typename U0, typename U1>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr enable_if_t<
    is_equality_comparable_with<pair<T0, T1>, pair<U0, U1>>::value, bool>
operator!=(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(
    is_nothrow_equality_comparable_with<pair<T0, T1>, pair<U0, U1>>::value) {
    return !static_cast<bool>(l == r);
}

template <typename T0, typename T1, typename U0, typename U1>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr enable_if_t<
    is_strict_subordinate_comparable_with<pair<U0, U1>, pair<T0, T1>>::value, bool>
operator<=(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(
    is_nothrow_strict_subordinate_comparable_with<pair<U0, U1>, pair<T0, T1>>::value) {
    return !static_cast<bool>(r < l);
}

template <typename T0, typename T1, typename U0, typename U1>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr enable_if_t<
    is_strict_subordinate_comparable_with<pair<U0, U1>, pair<T0, T1>>::value, bool>
operator>(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(
    is_nothrow_strict_subordinate_comparable_with<pair<U0, U1>, pair<T0, T1>>::value) {
    return static_cast<bool>(r < l);
}

template <typename T0, typename T1, typename U0, typename U1>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr enable_if_t<
    is_strict_subordinate_comparable_with<pair<T0, T1>, pair<U0, U1>>::value, bool>
operator>=(pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(
    is_nothrow_strict_subordinate_comparable_with<pair<T0, T1>, pair<U0, U1>>::value) {
    return !static_cast<bool>(l < r);
}

template <typename T, typename U>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI, ALWAYS_INLINE) constexpr pair<unwrap_reference_t<decay_t<T>>, unwrap_reference_t<decay_t<U>>>
make_pair(T&& t, U&& u) {
    return pair<unwrap_reference_t<decay_t<T>>, unwrap_reference_t<decay_t<U>>>{
        forward<T>(t), forward<U>(u)};
}

#endif // UTL_USE_STDPAIR

UTL_NAMESPACE_END

#if !UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace pair {
template <typename T0, typename T1, typename U0, typename U1, typename = void>
struct common_type_impl {};

template <typename T0, typename T1, typename U0, typename U1>
struct common_type_impl<T0, T1, U0, U1, void_t<common_type_t<T0, U0>, common_type_t<T1, U1>>> {
    using type UTL_NODEBUG = UTL_SCOPE pair<common_type_t<T0, U0>, common_type_t<T1, U1>>;
};

template <typename T0, typename T1, typename U0, typename U1, template <typename> class TQual,
    template <typename> class UQual, typename = void>
struct basic_ref_impl {};

template <typename T0, typename T1, typename U0, typename U1, template <typename> class TQual,
    template <typename> class UQual>
struct basic_ref_impl<T0, T1, U0, U1, TQual, UQual,
    void_t<common_reference_t<TQual<T0>, UQual<U0>>, common_reference_t<TQual<T1>, UQual<U1>>>> {
    using type UTL_NODEBUG = UTL_SCOPE
        pair<common_reference_t<TQual<T0>, UQual<U0>>, common_reference_t<TQual<T1>, UQual<U1>>>;
};
} // namespace pair
} // namespace details

UTL_NAMESPACE_END

namespace std {

template <typename T0, typename T1, typename U0, typename U1>
struct UTL_PUBLIC_TEMPLATE common_type<UTL_SCOPE pair<T0, T1>, UTL_SCOPE pair<U0, U1>> :
    UTL_SCOPE details::pair::common_type_impl<T0, T1, U0, U1> {};

template <typename T0, typename T1, typename U0, typename U1, template <typename> class TQual,
    template <typename> class UQual>
struct UTL_PUBLIC_TEMPLATE
    basic_common_reference<UTL_SCOPE pair<T0, T1>, UTL_SCOPE pair<U0, U1>, TQual, UQual> :
    UTL_SCOPE details::pair::basic_ref_impl<T0, T1, U0, U1, TQual, UQual> {};

} // namespace std

#else  // if !UTL_CXX20

namespace std {
template <typename T0, typename T1, UTL_SCOPE common_with<T0> U0, UTL_SCOPE common_with<T1> U1>
struct UTL_PUBLIC_TEMPLATE common_type<UTL_SCOPE pair<T0, T1>, UTL_SCOPE pair<U0, U1>> {
    using type UTL_NODEBUG =
        UTL_SCOPE pair<UTL_SCOPE common_type_t<T0, U0>, UTL_SCOPE common_type_t<T1, U1>>;
};

template <typename T0, typename T1, typename U0, typename U1, template <typename> class TQual,
    template <typename> class UQual>
requires requires {
    typename UTL_SCOPE common_reference<TQual<T0>, UQual<U0>>::type;
    typename UTL_SCOPE common_reference<TQual<T1>, UQual<U1>>::type;
}
struct UTL_PUBLIC_TEMPLATE
    basic_common_reference<UTL_SCOPE pair<T0, T1>, UTL_SCOPE pair<U0, U1>, TQual, UQual> {
    using type UTL_NODEBUG = UTL_SCOPE pair<UTL_SCOPE common_reference_t<TQual<T0>, UQual<U0>>,
        UTL_SCOPE common_reference_t<TQual<T1>, UQual<U1>>>;
};
} // namespace std
#endif // if !UTL_CXX20

UTL_STD_NAMESPACE_BEGIN
template <typename T0, typename T1>
struct UTL_PUBLIC_TEMPLATE tuple_size<UTL_SCOPE pair<T0, T1>> :
    UTL_SCOPE template_size<UTL_SCOPE pair<T0, T1>> {};

template <size_t I, typename T0, typename T1>
struct UTL_PUBLIC_TEMPLATE tuple_element<I, UTL_SCOPE pair<T0, T1>> :
    UTL_SCOPE template_element<I, UTL_SCOPE pair<T0, T1>> {};
UTL_STD_NAMESPACE_END
