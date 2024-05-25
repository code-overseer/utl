// Copyright 2023-2024 Bryan Wong

#pragma once

/**
 * Standard Tuple implementation from C++20 onwards
 */

#ifndef UTL_TUPLE_PRIVATE_HEADER_GUARD
#  error "Private header accessed"
#endif

#ifndef UTL_CXX20
#  error "Header compiled with invalid standard"
#endif

#include "utl/compare/utl_compare_traits.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/memory/utl_uses_allocator.h"
#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_pragma.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/tuple/utl_tuple_concepts.h"
#include "utl/type_traits/utl_common_reference.h"
#include "utl/type_traits/utl_common_type.h"
#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_has_type.h"
#include "utl/type_traits/utl_template_list.h"
#include "utl/type_traits/utl_unwrap_reference.h"
#include "utl/type_traits/utl_variadic_proxy.h"
#include "utl/type_traits/utl_variadic_traits.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_ignore.h"
#include "utl/utility/utl_move.h"
#include "utl/utility/utl_sequence.h"
#include "utl/utility/utl_swap.h"

#define TT_SCOPE UTL_SCOPE tuple_traits::

// TODO: if std is included or forward declared use std, else use UTL
namespace std {
template <typename... Ts, typename... Us>
requires (... && requires { typename UTL_SCOPE common_type<Ts, Us>::type; })
struct common_type<UTL_SCOPE tuple<Ts...>, UTL_SCOPE tuple<Us...>> {
    using type = UTL_SCOPE tuple<common_type_t<Ts, Us>...>;
};
template <typename... Ts, typename... Us, template <typename> class TQual,
    template <typename> class UQual>
requires (... && requires { typename UTL_SCOPE common_reference<TQual<Ts>, UQual<Us>>::type; })
struct basic_common_reference<UTL_SCOPE tuple<Ts...>, UTL_SCOPE tuple<Us...>, TQual, UQual> {
    using type = UTL_SCOPE tuple<UTL_SCOPE common_reference_t<TQual<Ts>, UQual<Us>>...>;
};
} // namespace std

UTL_NAMESPACE_BEGIN

namespace utility {
namespace details {
template <typename F, size_t... Is>
consteval decltype(auto) disjunction_sequence(F&& func, index_sequence<Is...>) noexcept {
    return (... || func(integral_constant<size_t, Is>{}));
}

template <typename F, size_t... Is>
consteval decltype(auto) conjunction_sequence(F&& func, index_sequence<Is...>) noexcept {
    return (... && func(integral_constant<size_t, Is>{}));
}
} // namespace details

template <size_t N, typename F>
consteval decltype(auto) disjunction_sequence(F&& func) noexcept {
    return details::disjunction_sequence(forward<F>(func), make_index_sequence<N>());
}

template <size_t N, typename F>
consteval decltype(auto) conjunction_sequence(F&& func) noexcept {
    return details::conjunction_sequence(forward<F>(func), make_index_sequence<N>());
}
} // namespace utility

namespace details {
namespace tuple {

template <typename...>
struct invalid_swap_t {
    inline constexpr void swap(invalid_swap_t& other) const noexcept {}
    inline constexpr void swap(invalid_swap_t const& other) const noexcept {}

private:
    ~invalid_swap_t() = default;
};

template <typename...>
struct storage;
template <size_t I, typename T, typename = void>
struct offset_impl;
struct invalid_t {
private:
    ~invalid_t() = default;
};

template <>
struct storage<> {
    constexpr storage() noexcept = default;
    constexpr storage& operator=(storage const&) noexcept = default;
    constexpr storage& operator=(storage&&) noexcept = default;
    constexpr void swap(storage<>& other) noexcept {}
};

template <typename T>
struct storage<T> : variadic_traits<T> {
public:
    template <typename U>
    using not_this = is_same<storage<T>, remove_cvref_t<U>>;
    using traits = variadic_traits<T>;
    using head_type = T;
    using move_assign_t = conditional_t<traits::is_move_assignable, invalid_t, storage>;
    using move_construct_t = conditional_t<traits::is_move_constructible, invalid_t, storage>;

    constexpr storage() noexcept(traits::is_nothrow_default_constructible) = default;
    constexpr storage(storage const&) noexcept(traits::is_nothrow_copy_constructible) = default;
    constexpr storage& operator=(storage const&) noexcept(
        traits::is_nothrow_copy_assignable) = default;

#if UTL_ENFORCE_NONMOVABILIITY
    constexpr storage(move_construct_t&&) noexcept(traits::is_nothrow_move_constructible) = delete;
    constexpr storage& operator=(move_assign_t&&) noexcept(
        traits::is_nothrow_move_assignable) = delete;
#else
    constexpr storage(storage&&) noexcept(traits::is_nothrow_move_constructible) = default;
    constexpr storage& operator=(storage&&) noexcept(traits::is_nothrow_move_assignable) = default;
#endif

    template <typename U>
    requires (!not_this<U>::value)
    constexpr storage(U&& head) noexcept(traits::template is_nothrow_constructible<U>::value)
        : head(forward<U>(head)) {}

    template <typename Alloc>
    requires UTL_SCOPE
    uses_allocator<T, Alloc>::value&&
        UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>::value constexpr storage(
            allocator_arg_t, Alloc const& alloc) noexcept(requires(Alloc const& a) {
            { T(allocator_arg, a) } noexcept;
        })
        : head(allocator_arg, alloc) {}

    template <typename Alloc>
    requires UTL_SCOPE
    uses_allocator<T, Alloc>::value &&
        (!UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>::value) &&
        UTL_SCOPE is_constructible<T, Alloc const&>::value
        constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(requires(Alloc const& a) {
            { T(a) } noexcept;
        })
        : head(alloc) {}

    template <typename Alloc>
    requires (!UTL_SCOPE uses_allocator<T, Alloc>::value)
    constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(requires {
        { T() } noexcept;
    }) : head() {}

    template <typename Alloc, typename U>
    requires UTL_SCOPE
    uses_allocator<T, Alloc>::value&&
        UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&, U>::value constexpr storage(
            allocator_arg_t, Alloc const& alloc,
            U&& other_head) noexcept(requires(U&& u, Alloc const& a) {
            { T(allocator_arg, a, forward<U>(u)) } noexcept;
        })
        : head(allocator_arg, alloc, forward<U>(other_head)) {}

    template <typename Alloc, typename U>
    requires UTL_SCOPE
    uses_allocator<T, Alloc>::value &&
        (!UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&, U>::value) &&
        UTL_SCOPE is_constructible<T, Alloc const&, U>::value constexpr storage(allocator_arg_t,
            Alloc const& alloc, U&& other_head) noexcept(requires(U&& u, Alloc const& a) {
            { T(forward<U>(u), a) } noexcept;
        })
        : head(forward<U>(other_head), alloc) {}

    template <typename Alloc, typename U>
    requires (!UTL_SCOPE uses_allocator<T, Alloc>::value)
    constexpr storage(allocator_arg_t, Alloc const& alloc, U&& other_head) noexcept(
        requires(U&& u) {
            { T(forward<U>(u)) } noexcept;
        })
        : head(forward<U>(other_head)) {}

    template <typename U>
    constexpr void swap(storage<U>& other) noexcept(
        traits::template is_nothrow_swappable_with<U&>::value) {
        UTL_SCOPE utility::swap(head, other.head);
    }

    template <typename U>
    constexpr void swap(storage<U> const& other) noexcept(
        traits::template is_nothrow_swappable_with<U const&>::value) {
        UTL_SCOPE utility::swap(head, other.head);
    }

    template <typename U>
    constexpr void swap(storage<U>& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<U&>::value) {
        UTL_SCOPE utility::swap(head, other.head);
    }

    template <typename U>
    constexpr void swap(storage<U> const& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<U const&>::value) {
        UTL_SCOPE utility::swap(head, other.head);
    }

    template <typename U>
    constexpr storage& assign(U&& other) noexcept(requires(T& h, U&& u) {
        { h = forward<U>(u) } noexcept;
    }) {
        head = forward<U>(other);
        return *this;
    }

    template <typename U>
    constexpr storage const& assign(U&& other) const noexcept(requires(T const& h, U&& u) {
        { h = forward<U>(u) } noexcept;
    }) {
        head = forward<U>(other);
        return *this;
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() && noexcept -> T&& {
        return move(head);
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() & noexcept -> T& {
        return head;
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() const&& noexcept -> T const&& {
        return move(head);
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() const& noexcept -> T const& {
        return head;
    }

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) head_type head;
};

template <typename T, typename... Tail>
struct storage<T, Tail...> : variadic_traits<T, Tail...> {
    using traits = variadic_traits<T, Tail...>;
    using head_type = T;
    using tail_type = storage<Tail...>;
    using move_assign_t = conditional_t<traits::is_move_assignable, invalid_t, storage>;
    using move_construct_t = conditional_t<traits::is_move_constructible, invalid_t, storage>;

    constexpr storage() noexcept(traits::is_nothrow_default_constructible) = default;
    constexpr storage(storage const&) noexcept(traits::is_nothrow_copy_constructible) = default;
    constexpr storage& operator=(storage const&) noexcept(
        traits::is_nothrow_copy_assignable) = default;

#if UTL_ENFORCE_NONMOVABILIITY
    constexpr storage(move_construct_t&&) noexcept(traits::is_nothrow_move_constructible) = delete;
    constexpr storage& operator=(move_assign_t&&) noexcept(
        traits::is_nothrow_move_assignable) = delete;
#else
    constexpr storage(storage&&) noexcept(traits::is_nothrow_move_constructible) = default;
    constexpr storage& operator=(storage&&) noexcept(traits::is_nothrow_move_assignable) = default;
#endif

    template <typename UHead, typename... UTail>
    requires (sizeof...(UTail) == sizeof...(Tail))
    constexpr storage(UHead&& other_head, UTail&&... other_tail) noexcept(
        requires(UHead&& h, UTail&&... ts) {
            { head_type(forward<UHead>(h)) } noexcept;
            { tail_type(forward<UTail>(ts)...) } noexcept;
        })
        : head(forward<UHead>(other_head))
        , tail(forward<UTail>(other_tail)...) {}

    template <typename Alloc>
    requires UTL_SCOPE
    uses_allocator<T, Alloc>::value&&
        UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>::value constexpr storage(
            allocator_arg_t, Alloc const& alloc) noexcept(requires(Alloc const& a) {
            { head_type(allocator_arg, a) } noexcept;
            { tail_type(allocator_arg, a) } noexcept;
        })
        : head(allocator_arg, alloc)
        , tail(allocator_arg, alloc) {}

    template <typename Alloc>
    requires UTL_SCOPE
    uses_allocator<T, Alloc>::value &&
        (!UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>::value) &&
        UTL_SCOPE is_constructible<T, Alloc const&>::value
        constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(requires(Alloc const& a) {
            { head_type(a) } noexcept;
            { tail_type(allocator_arg, a) } noexcept;
        })
        : head(alloc)
        , tail(allocator_arg, alloc) {}

    template <typename Alloc>
    requires (!UTL_SCOPE uses_allocator<T, Alloc>::value)
    constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(requires(Alloc const& a) {
        { head_type() } noexcept;
        { tail_type(allocator_arg, a) } noexcept;
    }) : head()
       , tail(allocator_arg, alloc) {}

    template <typename Alloc, typename UHead, typename... UTail>
    requires UTL_SCOPE
    uses_allocator<T, Alloc>::value&& UTL_SCOPE
        is_constructible<T, allocator_arg_t, Alloc const&, UHead>::value constexpr storage(
            allocator_arg_t, Alloc const& alloc, UHead&& other_head,
            UTail&&... other_tail) noexcept(requires(Alloc const& a, UHead&& h, UTail&&... ts) {
            { head_type(allocator_arg, a, forward<UHead>(h)) } noexcept;
            { tail_type(allocator_arg, a, forward<UTail>(ts)...) } noexcept;
        })
        : head(allocator_arg, alloc, forward<UHead>(other_head))
        , tail(allocator_arg, alloc, forward<UTail>(other_tail)...) {}

    template <typename Alloc, typename UHead, typename... UTail>
    requires UTL_SCOPE
    uses_allocator<T, Alloc>::value &&
        (!UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&, UHead>::value) &&
        UTL_SCOPE is_constructible<T, UHead, Alloc const&>::value
        constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
            UTail&&... other_tail) noexcept(requires(Alloc const& a, UHead&& h, UTail&&... ts) {
            { head_type(forward<UHead>(h), a) } noexcept;
            { tail_type(allocator_arg, a, forward<UTail>(ts)...) } noexcept;
        })
        : head(forward<UHead>(other_head), alloc)
        , tail(allocator_arg, alloc, forward<UTail>(other_tail)...) {}

    template <typename Alloc, typename UHead, typename... UTail>
    requires (!UTL_SCOPE uses_allocator<T, Alloc>::value)
    constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(requires(Alloc const& a, UHead&& h, UTail&&... ts) {
        { head_type(forward<UHead>(h)) } noexcept;
        { tail_type(allocator_arg, a, forward<UTail>(ts)...) } noexcept;
    })
        : head(forward<UHead>(other_head))
        , tail(allocator_arg, alloc, forward<UTail>(other_tail)...) {}

    template <typename... Us>
    constexpr void swap(storage<Us&...>& other) noexcept(
        traits::template is_nothrow_swappable_with<Us&...>::value) {
        UTL_SCOPE utility::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename... Us>
    constexpr void swap(storage<Us...> const& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<Us const&...>::value) {
        UTL_SCOPE utility::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename... Us>
    constexpr void swap(storage<Us...>& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<Us&...>::value) {
        UTL_SCOPE utility::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename UHead, typename... UTail>
    constexpr storage& assign(UHead&& other_head, UTail&&... other_tail) noexcept(
        requires(T& h, tail_type& tail, UHead&& u, UTail&&... ts) {
            { h = forward<UHead>(u) } noexcept;
            { tail.assign(forward<UTail>(ts)...) } noexcept;
        }) {
        head = forward<UHead>(other_head);
        tail.assign(forward<UTail>(other_tail)...);
        return *this;
    }

    template <typename UHead, typename... UTail>
    constexpr storage const& assign(UHead&& other_head, UTail&&... other_tail) const
        noexcept(requires(T const& h, tail_type const& tail, UHead&& u, UTail&&... ts) {
            { h = forward<UHead>(u) } noexcept;
            { tail.assign(forward<UTail>(ts)...) } noexcept;
        }) {
        head = forward<UHead>(other_head);
        tail.assign(forward<UTail>(other_tail)...);
        return *this;
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() && noexcept UTL_ATTRIBUTE(LIFETIMEBOUND)
        -> T&& {
        return move(head);
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() & noexcept UTL_ATTRIBUTE(LIFETIMEBOUND)
        -> T& {
        return head;
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() const&& noexcept
        UTL_ATTRIBUTE(LIFETIMEBOUND) -> T const&& {
        return move(head);
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() const& noexcept
        UTL_ATTRIBUTE(LIFETIMEBOUND) -> T const& {
        return head;
    }

    template <size_t I>
    requires (I > 0) && (I < traits::size)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) constexpr auto get() && noexcept
        UTL_ATTRIBUTE(LIFETIMEBOUND) -> template_element_t<I, storage>&& {
        return move(tail).template get<I - 1>();
    }

    template <size_t I>
    requires (I > 0) && (I < traits::size)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) constexpr auto get() & noexcept
        UTL_ATTRIBUTE(LIFETIMEBOUND) -> template_element_t<I, storage>& {
        return tail.template get<I - 1>();
    }

    template <size_t I>
    requires (I > 0) && (I < traits::size)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) constexpr auto get() const&& noexcept
        UTL_ATTRIBUTE(LIFETIMEBOUND) -> template_element_t<I, storage> const&& {
        return move(tail).template get<I - 1>();
    }

    template <size_t I>
    requires (I > 0) && (I < traits::size)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) constexpr auto get() const& noexcept
        UTL_ATTRIBUTE(LIFETIMEBOUND) -> template_element_t<I, storage> const& {
        return tail.template get<I - 1>();
    }

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) head_type head;
    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) tail_type tail;
};

template <typename... Ts>
struct offset_impl<0, storage<Ts...>> {
    using type = storage<Ts...>;
    static_assert(is_standard_layout<type>::value, "Must be standard layout");
    static constexpr size_t value = offsetof(type, head);
};

template <size_t I, typename T0, typename... Ts>
struct offset_impl<I, storage<T0, Ts...>, enable_if_t<(I > 0)>> {
    using type = storage<T0, Ts...>;
    static_assert(is_standard_layout<type>::value, "Must be standard layout");
    static_assert(I < type::size, "Index out of bounds");
    static constexpr size_t value =
        offset_impl<I - 1, storage<Ts...>>::value + offsetof(type, tail);
};

} // namespace tuple
} // namespace details

template <>
class tuple<> : private details::tuple::storage<> {
public:
    using storage::storage;
    using storage::operator=;
    constexpr void swap(tuple const& other) const noexcept {}
    friend inline constexpr void swap(tuple const&, tuple const&) noexcept {}
};

template <typename... Types>
class tuple : private details::tuple::storage<Types...> {
private:
    template <size_t I, typename T>
    friend struct tuple_element_offset;

    using base_type = details::tuple::storage<Types...>;
    using traits = typename base_type::traits;
    using swap_type =
        conditional_t<traits::is_swappable, tuple, details::tuple::invalid_swap_t<Types...>>;
    using const_swap_type = conditional_t<traits::is_const_swappable, tuple,
        details::tuple::invalid_swap_t<Types...>> const;
    using move_assign_t =
        conditional_t<traits::is_move_assignable, details::tuple::invalid_t, tuple>;
    using move_construct_t =
        conditional_t<traits::is_move_constructible, details::tuple::invalid_t, tuple>;
    template <typename T>
    using not_this = negation<is_same<T, tuple>>;
    using base_type::get;

    template <typename TupleLike, size_t... Is>
    constexpr tuple(TupleLike&& other, index_sequence<Is...>) noexcept(requires(TupleLike&& t) {
        { base_type(UTL_TUPLE_GET(Is, forward<TupleLike>(t))...) } noexcept;
    }) : base_type(UTL_TUPLE_GET(Is, forward<TupleLike>(other))...) {}

    template <typename Alloc, typename TupleLike, size_t... Is>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other,
        index_sequence<Is...>) noexcept(requires(Alloc const& a, TupleLike&& t) {
        { base_type(allocator_arg, a, UTL_TUPLE_GET(Is, forward<TupleLike>(t))...) } noexcept;
    })
        : base_type(allocator_arg, alloc, UTL_TUPLE_GET(Is, forward<TupleLike>(other))...) {}

    template <typename TupleLike, size_t... Is>
    constexpr tuple& assign(TupleLike&& other, index_sequence<Is...>) noexcept(
        requires(base_type& b, TupleLike&& t) {
            { b.assign(UTL_TUPLE_GET(Is, forward<TupleLike>(t))...) } noexcept;
        }) {
        return (tuple&)base_type::assign(UTL_TUPLE_GET(Is, forward<TupleLike>(other))...);
    }

    template <typename TupleLike, size_t... Is>
    constexpr tuple const& assign(TupleLike&& other, index_sequence<Is...>) const
        noexcept(requires(base_type const& b, TupleLike&& t) {
            { b.assign(UTL_TUPLE_GET(Is, forward<TupleLike>(t))...) } noexcept;
        }) {
        return (tuple const&)base_type::assign(UTL_TUPLE_GET(Is, forward<TupleLike>(other))...);
    }

public:
    constexpr tuple(tuple const&) noexcept(traits::is_nothrow_copy_constructible) = default;
    constexpr tuple& operator=(tuple const&) noexcept(traits::is_nothrow_copy_assignable) = default;
    constexpr tuple(tuple&&) noexcept(traits::is_nothrow_move_constructible) = default;
    constexpr tuple& operator=(tuple&&) noexcept(traits::is_nothrow_move_assignable) = default;

    UTL_CONSTEXPR_CXX20 ~tuple() = default;

    template <same_as<tuple> U = tuple>
    constexpr tuple const& operator=(U const& other) const
        noexcept(noexcept(assign(other, index_sequence_for<Types...>{}))) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <same_as<tuple> U = tuple>
    constexpr tuple const& operator=(U&& other) const
        noexcept(noexcept(assign(declval<U>(), index_sequence_for<Types...>{}))) {
        return assign(move(other), index_sequence_for<Types...>{});
    }

public:
    template <same_as<Types>... Us>
    requires traits::template
    is_swappable_with<Us&...>::value constexpr void swap(tuple<Us...>& other) noexcept(
        traits::template is_nothrow_swappable_with<Us&...>::value) {
        static_assert(is_base_of<details::tuple::storage<Us...>, tuple<Us...>>::value,
            "tuple must inherit from storage");
        base_type::swap((details::tuple::storage<Us...>&)other);
    }

    template <same_as<Types>... Us>
    requires traits::template
    is_const_swappable_with<Us const&...>::value constexpr void swap(
        tuple<Us...> const& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<Us const&...>::value) {
        static_assert(is_base_of<details::tuple::storage<Us...>, tuple<Us...>>::value,
            "tuple must inherit from storage");
        base_type::swap((details::tuple::storage<Us...> const&)other);
    }

    friend inline constexpr void swap(swap_type& l, swap_type& r) noexcept(
        traits::is_nothrow_swappable) {
        l.swap(r);
    }

    friend inline constexpr void swap(const_swap_type& l, const_swap_type& r) noexcept(
        traits::is_nothrow_const_swappable) {
        l.swap(r);
    }

public:
    explicit(traits::is_explicit_default_constructible) constexpr tuple() noexcept(
        utility::conjunction_sequence<sizeof...(Types)>([](auto i) {
            return requires {
                { tuple_element_t<decltype(i)::value, tuple>() } noexcept;
            };
        })) = default;

public:
    explicit(traits::template is_explicit_constructible<Types const&...>::value) constexpr tuple(
        Types const&... args) noexcept(traits::is_nothrow_copy_constructible)
        : base_type(args...) {}

public:
    template <typename UHead, typename... UTail>
    requires (sizeof...(Types) == sizeof...(UTail) + 1) && traits::template
    is_constructible<UHead, UTail...>::value &&
        (!traits::template is_dangling<UHead&&, UTail&&...>::value) explicit(
            traits::template is_explicit_constructible<UHead,
                UTail...>::value) constexpr tuple(UHead&& head,
            UTail&&... tail) noexcept(requires(UHead&& head, UTail&&... tail) {
            { base_type(forward<UHead>(head), forward<UTail>(tail)...) } noexcept;
        })
        : base_type(forward<UHead>(head), forward<UTail>(tail)...) {}

    template <typename UHead, typename... UTail>
    requires (sizeof...(Types) == sizeof...(UTail) + 1) && traits::template
    is_constructible<UHead, UTail...>::value&&
        traits::template is_dangling<UHead&&, UTail&&...>::value explicit(
            traits::template is_explicit_constructible<UHead,
                UTail...>::value) constexpr tuple(UHead&& head,
            UTail&&... tail) noexcept(traits::template is_nothrow_constructible<UHead,
            UTail...>::value) = delete;

public:
    template <typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible<UTypes&...>::value &&
        (!traits::template is_dangling<UTypes&...>::value) explicit(
            traits::template is_explicit_constructible<
                UTypes&...>::value) constexpr tuple(tuple<UTypes...>& other) noexcept((... &&
            requires(UTypes& t) {
                { Types(t) } noexcept;
            }))
        : tuple(other, index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible<UTypes&...>::value&& traits::template is_dangling<UTypes&...>::value explicit(
        traits::template is_explicit_constructible<
            UTypes&...>::value) constexpr tuple(tuple<UTypes...>& other) noexcept((... &&
        requires(UTypes& t) {
            { Types(t) } noexcept;
        })) = delete;

public:
    template <typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible<UTypes const&...>::value &&
        (!traits::template is_dangling<UTypes const&...>::value) explicit(
            traits::template is_explicit_constructible<UTypes const&...>::
                value) constexpr tuple(tuple<UTypes...> const& other) noexcept((... &&
            requires(UTypes const& t) {
                { Types(t) } noexcept;
            }))
        : tuple(other, index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible<UTypes const&...>::value&&
        traits::template is_dangling<UTypes const&...>::value explicit(
            traits::template is_explicit_constructible<
                UTypes const&...>::value) constexpr tuple(tuple<UTypes...> const&) noexcept((... &&
            requires(UTypes const& t) {
                { Types(t) } noexcept;
            })) = delete;

public:
    template <typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible<UTypes&&...>::value &&
        (!traits::template is_dangling<UTypes&&...>::value) explicit(
            traits::template is_explicit_constructible<
                UTypes&&...>::value) constexpr tuple(tuple<UTypes...>&&
                other) noexcept(requires(tuple<UTypes...>&& t) {
            { tuple(move(t), index_sequence_for<UTypes...>{}) } noexcept;
        })
        : tuple(move(other), index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible<UTypes&&...>::value&&
        traits::template is_dangling<UTypes&&...>::value explicit(
            traits::template is_explicit_constructible<
                UTypes&&...>::value) constexpr tuple(tuple<UTypes...>&&) noexcept((... &&
            requires(UTypes&& t) {
                { Types(move(t)) } noexcept;
            })) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) &&
        (!traits::template is_constructible<UTypes && ...>::value)
    constexpr tuple(tuple<UTypes...>&&) = delete;
#endif

public:
    template <typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible<UTypes const&&...>::value &&
        (!traits::template is_dangling<UTypes const&&...>::value) explicit(
            traits::template is_explicit_constructible<
                UTypes const&&...>::value) constexpr tuple(tuple<UTypes...> const&&
                other) noexcept(requires(tuple<UTypes...>&& t) {
            { tuple(move(t), index_sequence_for<UTypes...>{}) } noexcept;
        })
        : tuple(move(other), index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible<UTypes const&&...>::value&&
        traits::template is_dangling<UTypes const&&...>::value explicit(
            traits::template is_explicit_constructible<UTypes const&&...>::
                value) constexpr tuple(tuple<UTypes...> const&&) noexcept((... &&
            requires(UTypes const&& t) {
                { Types(move(t)) } noexcept;
            })) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) &&
        (!traits::template is_constructible<UTypes const && ...>::value)
    constexpr tuple(tuple<UTypes...> const&&) = delete;
#endif

public:
    template <tuple_like TupleLike>
    requires requires {
        requires tuple_size<TupleLike>::value == sizeof...(Types);
        requires !same_as<remove_cvref_t<TupleLike>, tuple>;
        requires utility::conjunction_sequence<sizeof...(Types)>([](auto i) {
            using constant_t = decltype(i);
            using element_t = tuple_element_t<constant_t::value, tuple>;
            return requires(TupleLike&& t) {
                { element_t(get<constant_t::value>(t)) };
            };
        });
        requires utility::conjunction_sequence<sizeof...(Types)>([](auto i) {
            using constant_t = decltype(i);
            using element_t = tuple_element_t<constant_t::value, tuple>;
            using param_t = decltype(TT_SCOPE decl_element<constant_t::value, TupleLike&&>());
            return !reference_constructs_from_temporary<element_t, param_t>::value;
        });
    }
    explicit(TT_SCOPE rebind_references_t<traits::template is_explicit_constructible, TupleLike,
        sizeof...(Types)>::value) constexpr tuple(TupleLike&& p) noexcept(requires(TupleLike&& t) {
        { tuple(forward<TupleLike>(t), index_sequence_for<Types...>{}) } noexcept;
    })
        : tuple(forward<TupleLike>(p), index_sequence_for<Types...>{}) {}

    template <tuple_like TupleLike>
    requires requires {
        requires tuple_size<TupleLike>::value == sizeof...(Types);
        requires !same_as<remove_cvref_t<TupleLike>, tuple>;
        requires TT_SCOPE rebind_references_t<traits::template is_constructible, TupleLike>::value;
        requires TT_SCOPE rebind_references_t<traits::template is_dangling, TupleLike>::value;
    }
    explicit(TT_SCOPE rebind_references_t<traits::template is_explicit_constructible, TupleLike,
        sizeof...(Types)>::value) constexpr tuple(TupleLike&& p) noexcept(requires(TupleLike&& t) {
        { tuple(forward<TupleLike>(t), index_sequence_for<Types...>{}) } noexcept;
    }) = delete;

public:
    template <typename Alloc>
    requires traits::template
    is_constructible_with_allocator<Alloc>::value explicit(
        traits::template is_explicit_constructible_with_allocator<
            Alloc>::value) constexpr tuple(allocator_arg_t,
        Alloc const&
            alloc) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc>::value)
        : base_type(allocator_arg, alloc) {}

public:
    template <typename Alloc>
    requires (... && traits::template is_constructible_with_allocator<Alloc, Types const&>::value)
    explicit(traits::template is_explicit_constructible_with_allocator<Alloc,
        Types const&>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
        Types const&... args) noexcept(traits::
            template is_nothrow_constructible_with_allocator<Alloc, Types const&...>::value)
        : base_type(allocator_arg, alloc, args...) {}

public:
    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes...>::value&&
        traits::template is_explicit_constructible_with_allocator<Alloc, UTypes...>::value &&
        (!traits::template is_dangling_without_allocator<Alloc,
            UTypes...>::value) explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
            UTypes&&... args) noexcept(requires(Alloc const& alloc, UTypes&&... args) {
            { base_type(allocator_arg, alloc, forward<UTypes>(args)...) } noexcept;
        })
        : base_type(allocator_arg, alloc, forward<UTypes>(args)...) {}

    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes...>::value&&
        traits::template is_implicit_constructible_with_allocator<Alloc, UTypes...>::value &&
        (!traits::template is_dangling_without_allocator<Alloc, UTypes...>::value) constexpr tuple(
            allocator_arg_t, Alloc const& alloc,
            UTypes&&... args) noexcept(requires(Alloc const& alloc, UTypes&&... args) {
            { base_type(allocator_arg, alloc, forward<UTypes>(args)...) } noexcept;
        })
        : base_type(allocator_arg, alloc, forward<UTypes>(args)...) {}

    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes...>::value&&
        traits::template is_explicit_constructible_with_allocator<Alloc, UTypes...>::value&&
            traits::template is_dangling_without_allocator<Alloc,
                UTypes...>::value explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
                UTypes&&... args) noexcept(requires(Alloc const& alloc, UTypes&&... args) {
                { base_type(allocator_arg, alloc, forward<UTypes>(args)...) } noexcept;
            }) = delete;

    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes...>::value&&
        traits::template is_implicit_constructible_with_allocator<Alloc, UTypes...>::value&&
            traits::template is_dangling_without_allocator<Alloc, UTypes...>::value constexpr tuple(
                allocator_arg_t, Alloc const& alloc,
                UTypes&&... args) noexcept(requires(Alloc const& alloc, UTypes&&... args) {
                { base_type(allocator_arg, alloc, forward<UTypes>(args)...) } noexcept;
            }) = delete;

public:
    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes&...>::value &&
        (!traits::template is_dangling_without_allocator<Alloc, UTypes&...>::value) explicit(
            traits::template is_explicit_constructible_with_allocator<Alloc,
                UTypes&...>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
            tuple<UTypes...>& other) noexcept(requires(Alloc const& alloc,
            tuple<UTypes...>& other) {
            { tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) } noexcept;
        })
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes&...>::value&&
        traits::template is_dangling_without_allocator<Alloc, UTypes&...>::value explicit(
            traits::template is_explicit_constructible_with_allocator<Alloc,
                UTypes&...>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
            tuple<UTypes...>& other) noexcept(requires(Alloc const& alloc,
            tuple<UTypes...>& other) {
            { tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) } noexcept;
        }) = delete;

public:
    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes const&...>::value &&
        (!traits::template is_dangling_without_allocator<Alloc, UTypes const&...>::value) explicit(
            traits::template is_explicit_constructible_with_allocator<Alloc,
                UTypes const&...>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
            tuple<UTypes...> const& other) noexcept(requires(Alloc const& alloc,
            tuple<UTypes...> const& other) {
            { tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) } noexcept;
        })
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes const&...>::value&&
        traits::template is_dangling_without_allocator<Alloc, UTypes const&...>::value explicit(
            traits::template is_explicit_constructible_with_allocator<Alloc,
                UTypes const&...>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
            tuple<UTypes...> const& other) noexcept(requires(Alloc const& alloc,
            tuple<UTypes...> const& other) {
            { tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) } noexcept;
        }) = delete;

public:
    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes&&...>::value &&
        (!traits::template is_dangling_without_allocator<Alloc, UTypes&&...>::value) explicit(
            traits::template is_explicit_constructible_with_allocator<Alloc,
                UTypes&&...>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
            tuple<UTypes...>&& other) noexcept(requires(Alloc const& alloc,
            tuple<UTypes...>&& other) {
            { tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) } noexcept;
        })
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes&&...>::value&&
        traits::template is_dangling_without_allocator<Alloc, UTypes&&...>::value explicit(
            traits::template is_explicit_constructible_with_allocator<Alloc,
                UTypes&&...>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
            tuple<UTypes...>&& other) noexcept(requires(Alloc const& alloc,
            tuple<UTypes...>&& other) {
            { tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) } noexcept;
        }) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) &&
        (!traits::template is_constructible_with_allocator<Alloc, UTypes && ...>::value)
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...>&& other) = delete;
#endif

public:
    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes const&&...>::value &&
        (!traits::template is_dangling_without_allocator<Alloc, UTypes const&&...>::value) explicit(
            traits::template is_explicit_constructible_with_allocator<Alloc,
                UTypes const&&...>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
            tuple<UTypes...> const&& other) noexcept(requires(Alloc const& alloc,
            tuple<UTypes...> const&& other) {
            { tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) } noexcept;
        })
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) && traits::template
    is_constructible_with_allocator<Alloc, UTypes const&&...>::value&&
        traits::template is_dangling_without_allocator<Alloc, UTypes const&&...>::value explicit(
            traits::template is_explicit_constructible_with_allocator<Alloc,
                UTypes const&&...>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
            tuple<UTypes...> const&& other) noexcept(requires(Alloc const& alloc,
            tuple<UTypes...> const&& other) {
            { tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) } noexcept;
        }) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename Alloc, typename... UTypes>
    requires (sizeof...(Types) == sizeof...(UTypes)) &&
        (!traits::template is_constructible_with_allocator<Alloc, UTypes const && ...>::value)
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...> const&& other) = delete;
#endif

public:
    template <typename Alloc, tuple_like TupleLike>
    requires requires {
        requires tuple_size<TupleLike>::value == sizeof...(Types);
        requires !same_as<remove_cvref_t<TupleLike>, tuple>;
        requires TT_SCOPE rebind_references_t<
            variadic_proxy<traits::template is_constructible_with_allocator, Alloc>::template apply,
            TupleLike>::value;
        requires !TT_SCOPE rebind_references_t<
            variadic_proxy<traits::template is_dangling_without_allocator, Alloc>::template apply,
            TupleLike>::value;
    }
    explicit(TT_SCOPE rebind_references_t<
        variadic_proxy<traits::template is_explicit_constructible_with_allocator,
            Alloc>::template apply,
        TupleLike, sizeof...(Types)>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
        TupleLike&& other) noexcept(requires(Alloc const& alloc, TupleLike&& other) {
        {
            tuple(allocator_arg, alloc, forward<TupleLike>(other), index_sequence_for<Types...>{})
        } noexcept;
    })
        : tuple(allocator_arg, alloc, forward<TupleLike>(other), index_sequence_for<Types...>{}) {}

    template <typename Alloc, tuple_like TupleLike>
    requires requires {
        requires tuple_size<TupleLike>::value == sizeof...(Types);
        requires !same_as<remove_cvref_t<TupleLike>, tuple>;
        requires TT_SCOPE rebind_references_t<
            variadic_proxy<traits::template is_constructible_with_allocator, Alloc>::template apply,
            TupleLike>::value;
        requires TT_SCOPE rebind_references_t<
            variadic_proxy<traits::template is_dangling_without_allocator, Alloc>::template apply,
            TupleLike>::value;
    }
    explicit(TT_SCOPE rebind_references_t<
        variadic_proxy<traits::template is_explicit_constructible_with_allocator,
            Alloc>::template apply,
        TupleLike>::value) constexpr tuple(allocator_arg_t, Alloc const& alloc,
        TupleLike&& other) noexcept(requires(Alloc const& alloc, TupleLike&& other) {
        {
            tuple(allocator_arg, alloc, forward<TupleLike>(other), index_sequence_for<Types...>{})
        } noexcept;
    }) = delete;

public:
    template <typename Alloc>
    requires traits::template
    is_constructible_with_allocator<Alloc, Types const&...>::value constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        tuple const& other) noexcept(requires(Alloc const& alloc, tuple const& other) {
        { tuple(allocator_arg, alloc, other, index_sequence_for<Types...>{}) } noexcept;
    })
        : tuple(allocator_arg, alloc, other, index_sequence_for<Types...>{}) {}

    template <typename Alloc>
    requires traits::template
    is_constructible_with_allocator<Alloc, Types&&...>::value constexpr tuple(allocator_arg_t,
        Alloc const& alloc, tuple&& other) noexcept(requires(Alloc const& alloc, tuple&& other) {
        { tuple(allocator_arg, alloc, move(other), index_sequence_for<Types...>{}) } noexcept;
    })
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<Types...>{}) {}

    template <typename Alloc>
    requires traits::template
    is_constructible_with_allocator<Alloc, Types const&&...>::value constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        tuple const&& other) noexcept(requires(Alloc const& alloc, tuple const&& other) {
        { tuple(allocator_arg, alloc, move(other), index_sequence_for<Types...>{}) } noexcept;
    })
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<Types...>{}) {}

public:
    template <typename... UTypes>
    requires traits::template
    is_assignable<UTypes const&...>::value constexpr tuple& operator=(
        tuple<UTypes...> const& other) noexcept(requires(tuple& t, tuple<UTypes...> const& other) {
        { t.assign(other, index_sequence_for<Types...>{}) } noexcept;
    }) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <assignable_from<Types const&>... UTypes>
    requires traits::template
    is_const_assignable<UTypes const&...>::value constexpr tuple const& operator=(
        tuple<UTypes...> const& other) const
        noexcept(requires(tuple const& t, tuple<UTypes...> const& other) {
            { t.assign(other, index_sequence_for<Types...>{}) } noexcept;
        }) {
        return assign(other, index_sequence_for<Types...>{});
    }

public:
    template <typename... UTypes>
    requires traits::template
    is_assignable<UTypes&...>::value constexpr tuple& operator=(tuple<UTypes...>& other) noexcept(
        requires(tuple& t, tuple<UTypes...>& other) {
            { t.assign(other, index_sequence_for<Types...>{}) } noexcept;
        }) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <assignable_from<Types&>... UTypes>
    requires traits::template
    is_const_assignable<UTypes&...>::value constexpr tuple const& operator=(
        tuple<UTypes...>& other) const noexcept(requires(tuple const& t, tuple<UTypes...>& other) {
        { t.assign(other, index_sequence_for<Types...>{}) } noexcept;
    }) {
        return assign(other, index_sequence_for<Types...>{});
    }

public:
    template <typename... UTypes>
    requires traits::template
    is_assignable<UTypes&&...>::value constexpr tuple& operator=(tuple<UTypes...>&& other) noexcept(
        requires(tuple& t, tuple<UTypes...>&& other) {
            { t.assign(move(other), index_sequence_for<Types...>{}) } noexcept;
        }) {
        return assign(move(other), index_sequence_for<Types...>{});
    }

    /**
     * Non-standard overload
     */
    template <typename... UTypes>
    requires (!traits::template is_assignable<UTypes && ...>::value)
    constexpr tuple& operator=(tuple<UTypes...>&& other) = delete;

public:
    template <typename... UTypes>
    requires traits::template
    is_const_assignable<UTypes&&...>::value constexpr tuple const& operator=(
        tuple<UTypes...>&& other) const
        noexcept(requires(tuple const& t, tuple<UTypes...>&& other) {
            { t.assign(move(other), index_sequence_for<Types...>{}) } noexcept;
        }) {
        return assign(move(other), index_sequence_for<Types...>{});
    }

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes>
    requires (!traits::template is_const_assignable<UTypes && ...>::value)
    constexpr tuple const& operator=(tuple<UTypes...>&& other) const = delete;
#endif

public:
    template <typename... UTypes>
    requires traits::template
    is_assignable<UTypes const&&...>::value constexpr tuple& operator=(
        tuple<UTypes...> const&& other) noexcept(requires(tuple& t,
        tuple<UTypes...> const&& other) {
        { t.assign(move(other), index_sequence_for<Types...>{}) } noexcept;
    }) {
        return assign(move(other), index_sequence_for<Types...>{});
    }

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes>
    requires (!traits::template is_assignable<UTypes const && ...>::value)
    constexpr tuple& operator=(tuple<UTypes...> const&& other) = delete;
#endif

public:
    template <typename... UTypes>
    requires traits::template
    is_const_assignable<UTypes const&&...>::value constexpr tuple const& operator=(
        tuple<UTypes...> const&& other) const
        noexcept(requires(tuple const& t, tuple<UTypes...> const&& other) {
            { t.assign(move(other), index_sequence_for<Types...>{}) } noexcept;
        }) {
        return assign(move(other), index_sequence_for<Types...>{});
    }

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes>
    requires (!traits::template is_const_assignable<UTypes const && ...>::value)
    constexpr tuple const& operator=(tuple<UTypes...> const&& other) const = delete;
#endif

public:
    template <tuple_like TupleLike>
    requires requires {
        requires tuple_size<TupleLike>::value == sizeof...(Types);
        requires !same_as<remove_cvref_t<TupleLike>, tuple>;
        requires TT_SCOPE
            rebind_references_t<traits::template is_assignable, TupleLike, sizeof...(Types)>::value;
    }
    constexpr tuple& operator=(TupleLike&& other) noexcept(requires(tuple& t, TupleLike&& other) {
        { t.assign(forward<TupleLike>(other), index_sequence_for<Types...>{}) } noexcept;
    }) {
        return assign(forward<TupleLike>(other), index_sequence_for<Types...>{});
    }

    template <tuple_like TupleLike>
    requires requires {
        requires tuple_size<TupleLike>::value == sizeof...(Types);
        requires !same_as<remove_cvref_t<TupleLike>, tuple>;
        requires TT_SCOPE rebind_references_t<traits::template is_const_assignable, TupleLike,
            sizeof...(Types)>::value;
    }
    constexpr tuple const& operator=(TupleLike&& other) const
        noexcept(requires(tuple const& t, TupleLike&& other) {
            { t.assign(forward<TupleLike>(other), index_sequence_for<Types...>{}) } noexcept;
        }) {
        return assign(forward<TupleLike>(other), index_sequence_for<Types...>{});
    }
};

template <size_t I, typename T>
struct tuple_element_offset;

template <size_t I, typename... Ts>
struct tuple_element_offset<I, tuple<Ts...>> :
    enable_if_t<is_base_of<details::tuple::storage<Ts...>, tuple<Ts...>>::value,
        details::tuple::offset_impl<I, details::tuple::storage<Ts...>>> {};

template <typename... Ts>
constexpr tuple<unwrap_reference_t<decay_t<Ts>>...> make_tuple(Ts&&... ts) noexcept(
    is_nothrow_constructible<tuple<unwrap_reference_t<decay_t<Ts>>...>, Ts...>::value) {
    return tuple<unwrap_reference_t<decay_t<Ts>>...>{forward<Ts>(ts)...};
}

// TODO <=> cmp

template <typename... Args>
UTL_NODISCARD constexpr tuple<Args&...> tie(Args&... args UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return {args...};
}

template <typename... Args>
UTL_NODISCARD constexpr tuple<Args&&...> forward_as_tuple(
    Args&&... args UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return {forward<Args>(args)...};
}

UTL_NAMESPACE_END

#undef TT_SCOPE
