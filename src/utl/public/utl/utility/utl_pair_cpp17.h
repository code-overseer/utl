// Copyright 2023-2024 Bryan Wong

#pragma once

/**
 * Standard pair implementation up to and including C++17
 */

#if !defined(UTL_PAIR_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/preprocessor/utl_config.h"

#include "utl/utility/utl_pair_fwd.h"

#include "utl/compare/utl_compare_traits.h"
#include "utl/concepts/utl_common_with.h"
#include "utl/tuple/utl_tuple_concepts.h"
#include "utl/tuple/utl_tuple_get_element.h"
#include "utl/tuple/utl_tuple_traits.h"
#include "utl/type_traits/utl_common_reference.h"
#include "utl/type_traits/utl_common_type.h"
#include "utl/type_traits/utl_is_equality_comparable.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"
#include "utl/utility/utl_pair_details.h"
#include "utl/utility/utl_sequence.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_pair_like : bool_constant<is_tuple_like<T>::value && tuple_size<T>::value == 2> {};

template <typename T0, typename T1>
struct pair {
private:
    template <typename V, typename...>
    using defer UTL_NODEBUG = V;

    template <typename U0, typename U1>
    using construct_explicitly UTL_NODEBUG =
        disjunction<is_explicit_constructible<T0, U0>, is_explicit_constructible<T1, U1>>;
    template <typename U0, typename U1>
    using construct_implicitly UTL_NODEBUG =
        conjunction<is_implicit_constructible<T0, U0>, is_implicit_constructible<T1, U1>>;
    template <typename U0, typename U1>
    using nothrow_construct UTL_NODEBUG =
        conjunction<is_nothrow_constructible<T0, U0>, is_nothrow_constructible<T1, U1>>;
    template <typename U0, typename U1>
    using reference_dangles UTL_NODEBUG = disjunction<reference_constructs_from_temporary<T0, U0>,
        reference_constructs_from_temporary<T1, U1>>;
    template <typename U0, typename U1>
    using reference_valid UTL_NODEBUG = negation<reference_dangles<U0, U1>>;
    template <typename U0, typename U1>
    using assign_from_pair UTL_NODEBUG =
        conjunction<is_assignable<T0&, U0>, is_assignable<T1&, U1>>;
    template <typename U0, typename U1>
    using const_assign_from_pair UTL_NODEBUG =
        conjunction<is_assignable<T0 const&, U0>, is_assignable<T1 const&, U1>>;
    template <typename U0, typename U1>
    using nothrow_assign_from_pair UTL_NODEBUG =
        conjunction<is_nothrow_assignable<T0&, U0>, is_nothrow_assignable<T1&, U1>>;
    template <typename U0, typename U1>
    using nothrow_const_assign_from_pair UTL_NODEBUG =
        conjunction<is_nothrow_assignable<T0 const&, U0>, is_nothrow_assignable<T1 const&, U1>>;

    template <typename U0, typename U1>
    using swappable UTL_NODEBUG =
        conjunction<is_swappable_with<T0&, U0&>, is_swappable_with<T1&, U1&>>;
    template <typename U0, typename U1>
    using const_swappable UTL_NODEBUG =
        conjunction<is_swappable_with<T0 const&, U0&>, is_swappable_with<T1 const&, U1&>>;
    template <typename U0, typename U1>
    using nothrow_swappable UTL_NODEBUG =
        conjunction<is_nothrow_swappable_with<T0&, U0&>, is_nothrow_swappable_with<T1&, U1&>>;
    template <typename U0, typename U1>
    using nothrow_const_swappable UTL_NODEBUG =
        conjunction<is_nothrow_swappable_with<T0 const&, U0&>,
            is_nothrow_swappable_with<T1 const&, U1&>>;

    using swap_type UTL_NODEBUG =
        conditional_t<swappable<T0&, T1&>::value, pair, details::pair::invalid_swap_t<T0, T1>>;
    using const_swap_type UTL_NODEBUG = conditional_t<const_swappable<T0 const&, T1 const&>::value,
        pair, details::pair::invalid_swap_t<T0, T1>> const;

    template <typename L0, typename L1, size_t... Is, size_t... Js>
    UTL_HIDE_FROM_ABI constexpr pair(L0& l0, L1& l1, index_sequence<Is...>,
        index_sequence<Js...>) noexcept(conjunction<details::tuple::is_all_nothrow_gettable<L0&>,
        details::tuple::is_all_nothrow_gettable<L1&>,
        UTL_SCOPE is_nothrow_constructible<T0, tuple_element_t<Is, L0>...>,
        UTL_SCOPE is_nothrow_constructible<T1, tuple_element_t<Js, L1>...>>::value)
        : first(UTL_SCOPE forward<tuple_element_t<Is, L0>>(UTL_SCOPE get_element<Is>(l0))...)
        , second(UTL_SCOPE forward<tuple_element_t<Js, L1>>(UTL_SCOPE get_element<Js>(l1))...) {}

    template <typename U0, typename U1>
    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE)
    UTL_CONSTEXPR_CXX14 pair& assign(U0&& u0, U1&& u1) noexcept(
        nothrow_assign_from_pair<U0&&, U1&&>::value) {
        first = UTL_SCOPE forward<U0>(u0);
        second = UTL_SCOPE forward<U1>(u1);
        return *this;
    }

    template <typename U0, typename U1>
    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE) UTL_CONSTEXPR_CXX14 pair const& assign(U0&& u0, U1&& u1) const
        noexcept(nothrow_const_assign_from_pair<U0&&, U1&&>::value) {
        first = UTL_SCOPE forward<U0>(u0);
        second = UTL_SCOPE forward<U1>(u1);
        return *this;
    }

public:
    using first_type = T0;
    using second_type = T1;
    T0 first;
    T1 second;

    UTL_HIDE_FROM_ABI constexpr pair(pair const&) noexcept(
        nothrow_construct<T0 const&, T1 const&>::value) = default;
    UTL_HIDE_FROM_ABI constexpr pair(pair&&) noexcept(
        nothrow_construct<T0&&, T1&&>::value) = default;
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 pair& operator=(pair const&) noexcept(
        nothrow_assign_from_pair<T0 const&, T1 const&>::value) = default;
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 pair& operator=(pair&&) noexcept(
        nothrow_assign_from_pair<T0&&, T1&&>::value) = default;

    template <typename U0 = T0, typename U1 = T1,
        typename V = defer<const_assign_from_pair<T0 const&, T1 const&>, U0, U1>>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<V::value, pair const&> operator=(
        pair const& other) const
        noexcept(nothrow_const_assign_from_pair<T0 const&, T1 const&>::value) {
        return assign(other.first, other.second);
    }

    template <typename U0 = T0, typename U1 = T1,
        typename V = defer<const_assign_from_pair<T0&&, T1&&>, U0, U1>>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<V::value, pair const&> operator=(pair&& other) const
        noexcept(nothrow_const_assign_from_pair<T0&&, T1&&>::value) {
        return assign(UTL_SCOPE move(other).first, UTL_SCOPE move(other).second);
    }

public:
    template <typename U0 = T0, typename U1 = T1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<swappable<U0&, U1&>::value> swap(
        pair<U0, U1>& other) noexcept(nothrow_swappable<U0&, U1&>::value) {
        UTL_SCOPE ranges::swap(first, other.first);
        UTL_SCOPE ranges::swap(second, other.second);
    }
    template <typename U0 = T0, typename U1 = T1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<swappable<U0 const&, U1 const&>::value> swap(
        pair<U0, U1> const& other) noexcept(nothrow_swappable<U0 const&, U1 const&>::value) {
        UTL_SCOPE ranges::swap(first, other.first);
        UTL_SCOPE ranges::swap(second, other.second);
    }

    template <typename U0 = T0, typename U1 = T1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<const_swappable<U0 const&, U1 const&>::value> swap(
        pair<U0, U1> const& other) const
        noexcept(nothrow_const_swappable<U0 const&, U1 const&>::value) {
        UTL_SCOPE ranges::swap(first, other.first);
        UTL_SCOPE ranges::swap(second, other.second);
    }

    template <typename U0 = T0, typename U1 = T1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<const_swappable<U0&, U1&>::value> swap(
        pair<U0, U1>& other) const noexcept(nothrow_const_swappable<U0&, U1&>::value) {
        UTL_SCOPE ranges::swap(first, other.first);
        UTL_SCOPE ranges::swap(second, other.second);
    }

    UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void swap(swap_type& l, swap_type& r) noexcept(
        nothrow_swappable<T0&, T1&>::value) {
        l.swap(r);
    }

    UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void swap(const_swap_type& l,
        const_swap_type& r) noexcept(nothrow_const_swappable<T0 const&, T1 const&>::value) {
        l.swap(r);
    }

public:
#if UTL_COMPILER_CLANG_AT_LEAST(10, 0, 0) | UTL_COMPILER_MSVC_AT_LEAST(1924)
    UTL_DISABLE_WARNING_PUSH()
#  if UTL_COMPILER_CLANG
    UTL_DISABLE_WARNING("-Wc++20-extensions")
#  elif UTL_COMPILER_MSVC
    UTL_DISABLE_WARNING(5053)
#  endif
    explicit(UTL_TRAIT_is_explicit_constructible(T0) ||
        UTL_TRAIT_is_explicit_constructible(T1)) constexpr pair() noexcept = default;
    UTL_DISABLE_WARNING_POP()
#else
    template <typename U0 = T0, typename U1 = T1 UTL_REQUIRES_CXX11(defer<bool_constant<
            UTL_TRAIT_is_implicit_constructible(T0) && UTL_TRAIT_is_implicit_constructible(T1)>,
        U0, U1>::value)>
    UTL_HIDE_FROM_ABI constexpr pair() noexcept(UTL_TRAIT_is_nothrow_default_constructible(T0) &&
        UTL_TRAIT_is_nothrow_default_constructible(T1))
        : first()
        , second() {}

    template <typename U0 = T0, typename U1 = T1 UTL_REQUIRES_CXX11(defer<bool_constant<
            UTL_TRAIT_is_explicit_constructible(T0) || UTL_TRAIT_is_explicit_constructible(T1)>,
        U0, U1>::value)>
    UTL_HIDE_FROM_ABI explicit constexpr pair() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T0) &&
        UTL_TRAIT_is_nothrow_default_constructible(T1))
        : first()
        , second() {}
#endif

public:
    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<construct_implicitly<U0 const&, U1 const&>::value, bool> = true>
    UTL_HIDE_FROM_ABI constexpr pair(T0 const& t0, T1 const& t1) noexcept(
        nothrow_construct<T0, T1>::value)
        : first(t0)
        , second(t1) {}

    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<construct_explicitly<U0 const&, U1 const&>::value, bool> = false>
    UTL_HIDE_FROM_ABI explicit constexpr pair(T0 const& t0, T1 const& t1) noexcept(
        nothrow_construct<T0, T1>::value)
        : first(t0)
        , second(t1) {}

public:
    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<construct_implicitly<U0&&, U1&&>::value && reference_valid<U0&&, U1&&>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(U0&& u0, U1&& u1) noexcept(
        nothrow_construct<U0&&, U1&&>::value)
        : first(UTL_SCOPE forward<U0>(u0))
        , second(UTL_SCOPE forward<U1>(u1)) {}

    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<construct_explicitly<U0&&, U1&&>::value && reference_valid<U0&&, U1&&>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(U0&& u0, U1&& u1) noexcept(
        nothrow_construct<U0&&, U1&&>::value)
        : first(UTL_SCOPE forward<U0>(u0))
        , second(UTL_SCOPE forward<U1>(u1)) {}

    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<construct_implicitly<U0&&, U1&&>::value && reference_dangles<U0&&, U1&&>::value,
            int> = 2>
    constexpr pair(U0&& u0, U1&& u1) noexcept(nothrow_construct<U0&&, U1&&>::value) = delete;

    template <typename U0 = T0, typename U1 = T1,
        enable_if_t<construct_explicitly<U0&&, U1&&>::value && reference_dangles<U0&&, U1&&>::value,
            int> = 3>
    explicit constexpr pair(U0&& u0, U1&& u1) noexcept(
        nothrow_construct<U0&&, U1&&>::value) = delete;

public:
    template <typename U0, typename U1,
        enable_if_t<construct_implicitly<U0&, U1&>::value && reference_valid<U0&, U1&>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(pair<U0, U1>& p) noexcept(nothrow_construct<U0&, U1&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<construct_explicitly<U0&, U1&>::value && reference_valid<U0&, U1&>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(pair<U0, U1>& p) noexcept(
        nothrow_construct<U0&, U1&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<construct_implicitly<U0&, U1&>::value && reference_dangles<U0&, U1&>::value,
            int> = 2>
    constexpr pair(pair<U0, U1>& p) noexcept(nothrow_construct<U0&, U1&>::value) = delete;

    template <typename U0, typename U1,
        enable_if_t<construct_explicitly<U0&, U1&>::value && reference_dangles<U0&, U1&>::value,
            int> = 3>
    explicit constexpr pair(pair<U0, U1>& p) noexcept(nothrow_construct<U0&, U1&>::value) = delete;

public:
    template <typename U0, typename U1,
        enable_if_t<construct_implicitly<U0 const&, U1 const&>::value &&
                reference_valid<U0 const&, U1 const&>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(pair<U0, U1> const& p) noexcept(
        nothrow_construct<U0 const&, U1 const&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<construct_explicitly<U0 const&, U1 const&>::value &&
                reference_valid<U0 const&, U1 const&>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(pair<U0, U1> const& p) noexcept(
        nothrow_construct<U0 const&, U1 const&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1,
        enable_if_t<construct_implicitly<U0 const&, U1 const&>::value &&
                reference_dangles<U0 const&, U1 const&>::value,
            int> = 2>
    constexpr pair(pair<U0, U1> const& p) noexcept(
        nothrow_construct<U0 const&, U1 const&>::value) = delete;

    template <typename U0, typename U1,
        enable_if_t<construct_explicitly<U0 const&, U1 const&>::value &&
                reference_dangles<U0 const&, U1 const&>::value,
            int> = 3>
    explicit constexpr pair(pair<U0, U1> const& p) noexcept(
        nothrow_construct<U0 const&, U1 const&>::value) = delete;

public:
    template <typename U0, typename U1,
        enable_if_t<construct_implicitly<U0&&, U1&&>::value && reference_valid<U0&&, U1&&>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(pair<U0, U1>&& p) noexcept(
        nothrow_construct<U0&&, U1&&>::value)
        : first(UTL_SCOPE move(p).first)
        , second(UTL_SCOPE move(p).second) {}

    template <typename U0, typename U1,
        enable_if_t<construct_explicitly<U0&&, U1&&>::value && reference_valid<U0&&, U1&&>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(pair<U0, U1>&& p) noexcept(
        nothrow_construct<U0&&, U1&&>::value)
        : first(UTL_SCOPE move(p).first)
        , second(UTL_SCOPE move(p).second) {}

    template <typename U0, typename U1,
        enable_if_t<construct_implicitly<U0&&, U1&&>::value && reference_dangles<U0&&, U1&&>::value,
            int> = 2>
    constexpr pair(pair<U0, U1>&& p) noexcept(nothrow_construct<U0&&, U1&&>::value) = delete;

    template <typename U0, typename U1,
        enable_if_t<construct_explicitly<U0&&, U1&&>::value && reference_dangles<U0&&, U1&&>::value,
            int> = 3>
    explicit constexpr pair(pair<U0, U1>&& p) noexcept(
        nothrow_construct<U0&&, U1&&>::value) = delete;

public:
    template <typename U0, typename U1,
        enable_if_t<construct_implicitly<U0 const&&, U1 const&&>::value &&
                reference_valid<U0 const&&, U1 const&&>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(pair<U0, U1> const&& p) noexcept(
        nothrow_construct<U0 const&&, U1 const&&>::value)
        : first(UTL_SCOPE move(p).first)
        , second(UTL_SCOPE move(p).second) {}

    template <typename U0, typename U1,
        enable_if_t<construct_explicitly<U0 const&&, U1 const&&>::value &&
                reference_valid<U0 const&&, U1 const&&>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(pair<U0, U1> const&& p) noexcept(
        nothrow_construct<U0 const&&, U1 const&&>::value)
        : first(UTL_SCOPE move(p).first)
        , second(UTL_SCOPE move(p).second) {}

    template <typename U0, typename U1,
        enable_if_t<construct_implicitly<U0 const&&, U1 const&&>::value &&
                reference_dangles<U0 const&&, U1 const&&>::value,
            int> = 2>
    constexpr pair(pair<U0, U1> const&& p) noexcept(
        nothrow_construct<U0 const&&, U1 const&&>::value) = delete;

    template <typename U0, typename U1,
        enable_if_t<construct_explicitly<U0 const&&, U1 const&&>::value &&
                reference_dangles<U0 const&&, U1 const&&>::value,
            int> = 3>
    explicit constexpr pair(pair<U0, U1> const&& p) noexcept(
        nothrow_construct<U0 const&&, U1 const&&>::value) = delete;

public:
    template <typename P,
        enable_if_t<conjunction<is_pair_like<P>,
                        construct_implicitly<details::tuple::get_type_t<0, P>,
                            details::tuple::get_type_t<1, P>>,
                        reference_valid<details::tuple::get_type_t<0, P>,
                            details::tuple::get_type_t<1, P>>>::value,
            int> = 0>
    UTL_HIDE_FROM_ABI constexpr pair(P&& p) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<P>,
            nothrow_construct<details::tuple::get_type_t<0, P>,
                details::tuple::get_type_t<1, P>>>::value)
        : first(UTL_SCOPE get_key(UTL_SCOPE forward<P>(p)))
        , second(UTL_SCOPE get_value(UTL_SCOPE forward<P>(p))) {}

    template <typename P,
        enable_if_t<conjunction<is_pair_like<P>,
                        construct_explicitly<details::tuple::get_type_t<0, P>,
                            details::tuple::get_type_t<1, P>>,
                        reference_valid<details::tuple::get_type_t<0, P>,
                            details::tuple::get_type_t<1, P>>>::value,
            int> = 1>
    UTL_HIDE_FROM_ABI explicit constexpr pair(P&& p) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<P>,
            nothrow_construct<details::tuple::get_type_t<0, P>,
                details::tuple::get_type_t<1, P>>>::value)
        : first(UTL_SCOPE get_key(UTL_SCOPE forward<P>(p)))
        , second(UTL_SCOPE get_value(UTL_SCOPE forward<P>(p))) {}

    template <typename P,
        enable_if_t<conjunction<is_pair_like<P>,
                        construct_implicitly<details::tuple::get_type_t<0, P>,
                            details::tuple::get_type_t<1, P>>,
                        reference_dangles<details::tuple::get_type_t<0, P>,
                            details::tuple::get_type_t<1, P>>>::value,
            int> = 2>
    UTL_HIDE_FROM_ABI constexpr pair(P&& p) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<P>,
            nothrow_construct<details::tuple::get_type_t<0, P>,
                details::tuple::get_type_t<1, P>>>::value) = delete;

    template <typename P,
        enable_if_t<conjunction<is_pair_like<P>,
                        construct_explicitly<details::tuple::get_type_t<0, P>,
                            details::tuple::get_type_t<1, P>>,
                        reference_dangles<details::tuple::get_type_t<0, P>,
                            details::tuple::get_type_t<1, P>>>::value,
            int> = 3>
    UTL_HIDE_FROM_ABI explicit constexpr pair(P&& p) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<P>,
            nothrow_construct<details::tuple::get_type_t<0, P>,
                details::tuple::get_type_t<1, P>>>::value) = delete;

private:
    template <typename U, size_t... Is>
    UTL_HIDE_FROM_ABI static auto constructs_first_impl(index_sequence<Is...>) noexcept
        -> UTL_SCOPE is_constructible<T0, details::tuple::get_type_t<Is, U>...>;
    template <typename U, size_t... Is>
    UTL_HIDE_FROM_ABI static auto constructs_second_impl(index_sequence<Is...>) noexcept
        -> UTL_SCOPE is_constructible<T1, details::tuple::get_type_t<Is, U>...>;

    template <typename U>
    using constructs_first UTL_NODEBUG =
        decltype(constructs_first_impl<U>(tuple_index_sequence<U>{}));
    template <typename U>
    using constructs_second UTL_NODEBUG =
        decltype(constructs_second_impl<U>(tuple_index_sequence<U>{}));

public:
    template <typename L0, typename L1,
        enable_if_t<conjunction<is_tuple_like<remove_reference_t<L0>>,
                        is_tuple_like<remove_reference_t<L1>>, constructs_first<L0>,
                        constructs_second<L1>>::value,
            bool> = true>
    UTL_HIDE_FROM_ABI constexpr pair(piecewise_construct_t, L0 l0, L1 l1) noexcept(
        UTL_SCOPE is_nothrow_constructible<pair, L0&, L1&, tuple_index_sequence<L0>,
            tuple_index_sequence<L1>>::value)
        : pair(l0, l1, tuple_index_sequence<L0>{}, tuple_index_sequence<L1>{}) {}

    template <typename... Args0, typename... Args1,
        enable_if_t<conjunction<UTL_SCOPE is_constructible<T0, Args0...>,
                        UTL_SCOPE is_constructible<T1, Args1...>>::value,
            bool> = true>
    UTL_HIDE_FROM_ABI constexpr pair(piecewise_construct_t, tuple<Args0...> l0,
        tuple<Args1...> l1) noexcept(UTL_SCOPE is_nothrow_constructible<pair, tuple<Args0...>&,
        tuple<Args1...>&, index_sequence_for<Args0...>, index_sequence_for<Args1...>>::value)
        : pair(l0, l1, index_sequence_for<Args0...>{}, index_sequence_for<Args1...>{}) {}

public:
    template <typename U0, typename U1>
    UTL_HIDE_FROM_ABI
        UTL_CONSTEXPR_CXX14 enable_if_t<assign_from_pair<U0 const&, U1 const&>::value, pair&>
    operator=(pair<U0, U1> const& p) noexcept(
        nothrow_assign_from_pair<U0 const&, U1 const&>::value) {
        return assign(p.first, p.second);
    }

    template <typename U0, typename U1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<const_assign_from_pair<U0 const&, U1 const&>::value,
        pair const&>
    operator=(pair<U0, U1> const& p) const
        noexcept(nothrow_const_assign_from_pair<U0 const&, U1 const&>::value) {
        return assign(p.first, p.second);
    }

public:
    template <typename U0, typename U1>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<assign_from_pair<U0&&, U1&&>::value, pair&> operator=(
        pair<U0, U1>&& p) noexcept(nothrow_assign_from_pair<U0&&, U1&&>::value) {
        return assign(UTL_SCOPE move(p).first, UTL_SCOPE move(p).second);
    }

    template <typename U0, typename U1>
    UTL_HIDE_FROM_ABI
        UTL_CONSTEXPR_CXX14 enable_if_t<const_assign_from_pair<U0&&, U1&&>::value, pair const&>
    operator=(pair<U0, U1>&& p) const noexcept(nothrow_const_assign_from_pair<U0&&, U1&&>::value) {
        return assign(UTL_SCOPE move(p).first, UTL_SCOPE move(p).second);
    }

public:
    template <typename P>
    UTL_HIDE_FROM_ABI
        UTL_CONSTEXPR_CXX14 enable_if_t<conjunction<is_pair_like<P>,
                                                assign_from_pair<details::tuple::get_type_t<0, P>,
                                                    details::tuple::get_type_t<1, P>>>::value,
        pair&>
    operator=(P&& p) noexcept(conjunction<details::tuple::is_all_nothrow_gettable<P>,
        nothrow_assign_from_pair<details::tuple::get_type_t<0, P>,
            details::tuple::get_type_t<1, P>>>::value) {
        return assign(UTL_SCOPE get_key(UTL_SCOPE forward<P>(p)),
            UTL_SCOPE get_value(UTL_SCOPE forward<P>(p)));
    }

    template <typename P>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 enable_if_t<
        conjunction<is_pair_like<P>,
            const_assign_from_pair<details::tuple::get_type_t<0, P>,
                details::tuple::get_type_t<1, P>>>::value,
        pair const&>
    operator=(P&& p) const noexcept(conjunction<details::tuple::is_all_nothrow_gettable<P>,
        nothrow_const_assign_from_pair<details::tuple::get_type_t<0, P>,
            details::tuple::get_type_t<1, P>>>::value) {
        return assign(UTL_SCOPE get_key(UTL_SCOPE forward<P>(p)),
            UTL_SCOPE get_value(UTL_SCOPE forward<P>(p)));
    }

    UTL_HIDE_FROM_ABI ~pair() = default;
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
struct __UTL_PUBLIC_TEMPLATE common_type<UTL_SCOPE pair<T0, T1>, UTL_SCOPE pair<U0, U1>> :
    UTL_SCOPE details::pair::common_type_impl<T0, T1, U0, U1> {};

template <typename T0, typename T1, typename U0, typename U1, template <typename> class TQual,
    template <typename> class UQual>
struct __UTL_PUBLIC_TEMPLATE
    basic_common_reference<UTL_SCOPE pair<T0, T1>, UTL_SCOPE pair<U0, U1>, TQual, UQual> :
    UTL_SCOPE details::pair::basic_ref_impl<T0, T1, U0, U1, TQual, UQual> {};

} // namespace std
