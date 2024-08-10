// Copyright 2023-2024 Bryan Wong

#pragma once

/**
 * Standard Tuple implementation from C++20 onwards
 */

#ifndef UTL_TUPLE_PRIVATE_HEADER_GUARD
#  error "Private header accessed"
#endif

#if !UTL_CXX20
#  error "Header compiled with invalid standard"
#endif

#include "utl/preprocessor/utl_config.h"

#include "utl/compare/utl_compare_traits.h"
#include "utl/compare/utl_strong_ordering.h"
#include "utl/concepts/utl_allocator_type.h"
#include "utl/concepts/utl_assignable_from.h"
#include "utl/concepts/utl_assignable_to.h"
#include "utl/concepts/utl_common_reference_with.h"
#include "utl/concepts/utl_common_with.h"
#include "utl/concepts/utl_constructible_as.h"
#include "utl/concepts/utl_constructible_from.h"
#include "utl/concepts/utl_not_resolvable_to.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/concepts/utl_swappable.h"
#include "utl/concepts/utl_three_way_comparable.h"
#include "utl/concepts/utl_variadic_match.h"
#include "utl/memory/utl_uses_allocator.h"
#include "utl/ranges/utl_swap.h"
#include "utl/tuple/utl_tuple_compare_traits.h"
#include "utl/tuple/utl_tuple_concepts.h"
#include "utl/tuple/utl_tuple_details.h"
#include "utl/tuple/utl_tuple_get_element.h"
#include "utl/type_traits/utl_common_comparison_category.h"
#include "utl/type_traits/utl_common_reference.h"
#include "utl/type_traits/utl_common_type.h"
#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_explicit_constructible.h"
#include "utl/type_traits/utl_is_implicit_constructible.h"
#include "utl/type_traits/utl_is_move_assignable.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_standard_layout.h"
#include "utl/type_traits/utl_reference_constructs_from_temporary.h"
#include "utl/type_traits/utl_template_list.h"
#include "utl/type_traits/utl_unwrap_reference.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_ignore.h"
#include "utl/utility/utl_move.h"
#include "utl/utility/utl_sequence.h"

// TODO: if std is included or forward declared use std, else use UTL
namespace std {
template <typename... Ts, UTL_SCOPE common_with<Ts>... Us>
struct UTL_PUBLIC_TEMPLATE common_type<UTL_SCOPE tuple<Ts...>, UTL_SCOPE tuple<Us...>> {
    using type UTL_NODEBUG = UTL_SCOPE tuple<UTL_SCOPE common_type_t<Ts, Us>...>;
};
template <typename... Ts, typename... Us, template <typename> class TQual,
    template <typename> class UQual>
requires (... && UTL_SCOPE common_reference_with<TQual<Ts>, UQual<Us>>)
struct UTL_PUBLIC_TEMPLATE
    basic_common_reference<UTL_SCOPE tuple<Ts...>, UTL_SCOPE tuple<Us...>, TQual, UQual> {
    using type UTL_NODEBUG = UTL_SCOPE tuple<UTL_SCOPE common_reference_t<TQual<Ts>, UQual<Us>>...>;
};
} // namespace std

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {
template <typename T>
using add_const_lvalue_reference UTL_NODEBUG = add_lvalue_reference<T const>;
template <typename T>
using add_const_rvalue_reference UTL_NODEBUG = add_rvalue_reference<T const>;

template <typename T>
concept unrecognized = tuple_like<remove_reference_t<T>> && !is_tuple_v<T>;

template <typename...>
struct invalid_swap_t {
    UTL_HIDE_FROM_ABI inline constexpr void swap(invalid_swap_t& other) const noexcept {}
    UTL_HIDE_FROM_ABI inline constexpr void swap(invalid_swap_t const& other) const noexcept {}

private:
    UTL_HIDE_FROM_ABI ~invalid_swap_t() = default;
};

template <typename...>
struct storage;
template <size_t I, typename T, typename = void>
struct offset_impl;
struct invalid_t {
private:
    UTL_HIDE_FROM_ABI ~invalid_t() = default;
};

template <typename TupleLike, size_t... Is>
requires tuple_like<remove_reference_t<TupleLike>>
UTL_HIDE_FROM_ABI auto nothrow_accessible(index_sequence<Is...>) noexcept
    -> bool_constant<(...&& noexcept(UTL_SCOPE get_element<Is>(UTL_SCOPE declval<TupleLike>())))>;

template <typename TupleLike>
requires tuple_like<remove_reference_t<TupleLike>>
inline constexpr bool is_nothrow_accessible_v = decltype(nothrow_accessible<TupleLike>(
    make_index_sequence<tuple_size<TupleLike>::value>{}))::value;

template <unrecognized TupleLike, size_t... Is>
UTL_HIDE_FROM_ABI constexpr auto forward_unrecognized(
    TupleLike&& t, index_sequence<Is...>) noexcept(is_nothrow_accessible_v<TupleLike>)
    -> UTL_SCOPE tuple<decltype(UTL_SCOPE details::tuple::decl_element<Is, TupleLike>())...> {
    return {UTL_SCOPE get_element<Is>(UTL_SCOPE forward<TupleLike>(t))...};
}

template <unrecognized TupleLike>
UTL_ATTRIBUTES(HIDE_FROM_ABI, FLATTEN) constexpr auto forward_unrecognized(TupleLike&& t) noexcept(
    is_nothrow_accessible_v<TupleLike>) {
    return forward_unrecognized(t, make_index_sequence<tuple_size<TupleLike>::value>{});
}

template <>
struct storage<> {
    UTL_HIDE_FROM_ABI constexpr storage() noexcept = default;
    UTL_HIDE_FROM_ABI constexpr storage& operator=(storage const&) noexcept = default;
    UTL_HIDE_FROM_ABI constexpr storage& operator=(storage&&) noexcept = default;
    UTL_HIDE_FROM_ABI constexpr void swap(storage<>& other) noexcept {}
};

template <typename T>
struct storage<T> {
public:
    using head_type UTL_NODEBUG = T;
    using move_assign_t UTL_NODEBUG =
        UTL_SCOPE conditional_t<UTL_SCOPE is_move_assignable_v<T>, invalid_t, storage>;
    using move_construct_t UTL_NODEBUG =
        UTL_SCOPE conditional_t<UTL_SCOPE is_move_constructible_v<T>, invalid_t, storage>;

    UTL_HIDE_FROM_ABI constexpr storage() noexcept(is_nothrow_default_constructible_v<T>) = default;
    UTL_HIDE_FROM_ABI constexpr storage(storage const&) noexcept(
        is_nothrow_copy_constructible_v<T>) = default;
    UTL_HIDE_FROM_ABI constexpr storage& operator=(storage const&) noexcept(
        is_nothrow_copy_assignable_v<T>) = default;

#if UTL_ENFORCE_NONMOVABILIITY
    constexpr storage(move_construct_t&&) noexcept(is_nothrow_move_constructible_v<T>) = delete;
    constexpr storage& operator=(move_assign_t&&) noexcept(
        is_nothrow_move_assignable_v<T>) = delete;
#else
    UTL_HIDE_FROM_ABI constexpr storage(storage&&) noexcept(
        is_nothrow_move_constructible_v<T>) = default;
    UTL_HIDE_FROM_ABI constexpr storage& operator=(storage&&) noexcept(
        is_nothrow_move_assignable_v<T>) = default;
#endif

    template <constructible_as<T> U>
    UTL_HIDE_FROM_ABI constexpr storage(U&& head) noexcept(is_nothrow_constructible_v<T, U>)
        : head(forward<U>(head)) {}

    template <allocator_usable_with<T> Alloc>
    requires constructible_from<T, allocator_arg_t, Alloc const&>
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        is_nothrow_constructible_v<T, allocator_arg_t, Alloc const&>)
        : head(allocator_arg, alloc) {}

    template <allocator_usable_with<T> Alloc>
    requires (!constructible_from<T, allocator_arg_t, Alloc const&> &&
        constructible_from<T, Alloc const&>)
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        is_nothrow_constructible_v<T, Alloc const&>)
        : head(alloc) {}

    template <allocator_type Alloc>
    requires constructible_from<T>
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        is_nothrow_default_constructible_v<T>)
        : head() {}

    template <allocator_usable_with<T> Alloc, typename U>
    requires constructible_from<T, allocator_arg_t, Alloc const&, U>
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc,
        U&& other_head) noexcept(is_nothrow_constructible_v<T, allocator_arg_t, Alloc const&, U>)
        : head(allocator_arg, alloc, UTL_SCOPE forward<U>(other_head)) {}

    template <allocator_usable_with<T> Alloc, typename U>
    requires (!constructible_from<T, allocator_arg_t, Alloc const&, U> &&
        constructible_from<T, U, Alloc const&>)
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc,
        U&& other_head) noexcept(is_nothrow_constructible_v<T, U, Alloc const&>)
        : head(UTL_SCOPE forward<U>(other_head), alloc) {}

    template <allocator_type Alloc, typename U>
    requires constructible_from<T, U>
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc,
        U&& other_head) noexcept(is_nothrow_constructible_v<T, U>)
        : head(UTL_SCOPE forward<U>(other_head)) {}

    template <typename U>
    UTL_HIDE_FROM_ABI constexpr void swap(storage<U>& other) noexcept(
        is_nothrow_swappable_with_v<T&, U&>) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U>
    UTL_HIDE_FROM_ABI constexpr void swap(storage<U> const& other) noexcept(
        is_nothrow_swappable_with_v<T&, U const&>) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U>
    UTL_HIDE_FROM_ABI constexpr void swap(storage<U>& other) const
        noexcept(is_nothrow_swappable_with_v<T const&, U&>) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U>
    UTL_HIDE_FROM_ABI constexpr void swap(storage<U> const& other) const
        noexcept(is_nothrow_swappable_with_v<T const&, U const&>) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U>
    UTL_HIDE_FROM_ABI constexpr storage& assign(U&& other) noexcept(
        is_nothrow_assignable_v<T&, U>) {
        head = UTL_SCOPE forward<U>(other);
        return *this;
    }

    template <typename U>
    UTL_HIDE_FROM_ABI constexpr storage const& assign(U&& other) const
        noexcept(is_nothrow_assignable_v<T const&, U>) {
        head = UTL_SCOPE forward<U>(other);
        return *this;
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get() && noexcept -> T&& {
        return UTL_SCOPE move(head);
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get() & noexcept -> T& {
        return head;
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get() const&& noexcept -> T const&& {
        return UTL_SCOPE move(head);
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get() const& noexcept -> T const& {
        return head;
    }

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) head_type head;
};

template <typename T, typename... Tail>
struct storage<T, Tail...> {
private:
    static constexpr bool move_assignable =
        (is_move_assignable_v<T> && ... && is_move_assignable_v<Tail>);
    static constexpr bool move_constructible =
        (is_move_constructible_v<T> && ... && is_move_constructible_v<Tail>);

public:
    static constexpr size_t element_count = sizeof...(Tail) + 1;
    using head_type UTL_NODEBUG = T;
    using tail_type UTL_NODEBUG = storage<Tail...>;
    using move_assign_t UTL_NODEBUG = conditional_t<move_assignable, invalid_t, storage>;
    using move_construct_t UTL_NODEBUG = conditional_t<move_constructible, invalid_t, storage>;

    UTL_HIDE_FROM_ABI constexpr storage() noexcept((is_nothrow_default_constructible_v<T> && ... &&
        is_nothrow_default_constructible_v<Tail>)) = default;
    UTL_HIDE_FROM_ABI constexpr storage(storage const&) noexcept(
        (is_nothrow_copy_constructible_v<T> && ... &&
            is_nothrow_copy_constructible_v<Tail>)) = default;
    UTL_HIDE_FROM_ABI constexpr storage& operator=(storage const&) noexcept(
        (is_nothrow_copy_assignable_v<T> && ... && is_nothrow_copy_assignable_v<Tail>)) = default;

#if UTL_ENFORCE_NONMOVABILIITY
    constexpr storage(move_construct_t&&) noexcept((is_nothrow_move_constructible_v<T> && ... &&
        is_nothrow_move_constructible_v<Tail>)) = delete;
    constexpr storage& operator=(move_assign_t&&) noexcept(
        (is_nothrow_move_assignable_v<T> && ... && is_nothrow_move_assignable_v<Tail>)) = delete;
#else
    UTL_HIDE_FROM_ABI constexpr storage(storage&&) noexcept((is_nothrow_move_constructible_v<T> &&
        ... && is_nothrow_move_constructible_v<Tail>)) = default;
    UTL_HIDE_FROM_ABI constexpr storage& operator=(storage&&) noexcept(
        (is_nothrow_move_assignable_v<T> && ... && is_nothrow_move_assignable_v<Tail>)) = default;
#endif

    template <constructible_as<T> UHead, constructible_as<Tail>... UTail>
    UTL_HIDE_FROM_ABI constexpr storage(UHead&& other_head, UTail&&... other_tail) noexcept(
        is_nothrow_constructible_v<head_type, UHead> &&
        is_nothrow_constructible_v<tail_type, UTail...>)
        : head(UTL_SCOPE forward<UHead>(other_head))
        , tail(UTL_SCOPE forward<UTail>(other_tail)...) {}

    template <allocator_usable_with<T> Alloc>
    requires constructible_from<T, allocator_arg_t, Alloc const&>
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        is_nothrow_constructible_v<head_type, allocator_arg_t, Alloc const&> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&>)
        : head(allocator_arg, alloc)
        , tail(allocator_arg, alloc) {}

    template <allocator_usable_with<T> Alloc>
    requires (!constructible_from<T, allocator_arg_t, Alloc const&> &&
                 constructible_from<T, Alloc const&>)
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        is_nothrow_constructible_v<head_type, Alloc const&> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&>)
        : head(alloc)
        , tail(allocator_arg, alloc) {}

    template <allocator_type Alloc>
    requires constructible_from<T>
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        is_nothrow_default_constructible_v<head_type> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&>)
        : head()
        , tail(allocator_arg, alloc) {}

    template <allocator_usable_with<T> Alloc, typename UHead, typename... UTail>
    requires constructible_from<T, allocator_arg_t, Alloc const&, UHead>
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(is_nothrow_constructible_v<head_type, allocator_arg_t,
                                            Alloc const&, UHead> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&, UTail...>)
        : head(allocator_arg, alloc, UTL_SCOPE forward<UHead>(other_head))
        , tail(allocator_arg, alloc, UTL_SCOPE forward<UTail>(other_tail)...) {}

    template <allocator_usable_with<T> Alloc, typename UHead, typename... UTail>
    requires (!constructible_from<T, allocator_arg_t, Alloc const&, UHead> &&
                 constructible_from<T, UHead, Alloc const&>)
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(is_nothrow_constructible_v<head_type, UHead,
                                            Alloc const&> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&, UTail...>)
        : head(UTL_SCOPE forward<UHead>(other_head), alloc)
        , tail(allocator_arg, alloc, UTL_SCOPE forward<UTail>(other_tail)...) {}

    template <allocator_type Alloc, constructible_as<T> UHead, typename... UTail>
    UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(is_nothrow_constructible_v<head_type, UHead> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&, UTail...>)
        : head(UTL_SCOPE forward<UHead>(other_head))
        , tail(allocator_arg, alloc, UTL_SCOPE forward<UTail>(other_tail)...) {}

    template <typename U, typename... Us>
    UTL_HIDE_FROM_ABI constexpr void swap(storage<U, Us...>& other) noexcept(
        (is_nothrow_swappable_with_v<T&, U&> && ... && is_nothrow_swappable_with_v<Tail&, Us&>)) {
        UTL_SCOPE ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename U, typename... Us>
    UTL_HIDE_FROM_ABI constexpr void swap(storage<U, Us...>& other) const
        noexcept((is_nothrow_swappable_with_v<T const&, U&> && ... &&
            is_nothrow_swappable_with_v<Tail const&, Us&>)) {
        UTL_SCOPE ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename U, typename... Us>
    UTL_HIDE_FROM_ABI constexpr void swap(storage<U, Us...> const& other) noexcept(
        (is_nothrow_swappable_with_v<T&, U const&> && ... &&
            is_nothrow_swappable_with_v<Tail&, Us const&>)) {
        UTL_SCOPE ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename U, typename... Us>
    UTL_HIDE_FROM_ABI constexpr void swap(storage<U, Us...> const& other) const
        noexcept((is_nothrow_swappable_with_v<T const&, U const&> && ... &&
            is_nothrow_swappable_with_v<Tail const&, Us const&>)) {
        UTL_SCOPE ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename UHead, typename... UTail>
    UTL_HIDE_FROM_ABI constexpr storage& assign(UHead&& other_head, UTail&&... other_tail) noexcept(
        is_nothrow_assignable_v<T&, UHead>&& noexcept(tail.assign(UTL_SCOPE declval<UTail>()...))) {
        head = UTL_SCOPE forward<UHead>(other_head);
        tail.assign(UTL_SCOPE forward<UTail>(other_tail)...);
        return *this;
    }

    template <typename UHead, typename... UTail>
    UTL_HIDE_FROM_ABI constexpr storage const& assign(
        UHead&& other_head, UTail&&... other_tail) const
        noexcept(is_nothrow_assignable_v<T const&, UHead>&& noexcept(
            tail.assign(UTL_SCOPE declval<UTail>()...))) {
        head = UTL_SCOPE forward<UHead>(other_head);
        tail.assign(UTL_SCOPE forward<UTail>(other_tail)...);
        return *this;
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get() && noexcept UTL_LIFETIMEBOUND
    -> T&& {
        return UTL_SCOPE move(head);
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get() & noexcept UTL_LIFETIMEBOUND
    -> T& {
        return head;
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get() const&& noexcept UTL_LIFETIMEBOUND -> T const&& {
        return UTL_SCOPE move(head);
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr auto get() const& noexcept UTL_LIFETIMEBOUND -> T const& {
        return head;
    }

    template <size_t I>
    requires (I > 0) && (I < element_count)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, HIDE_FROM_ABI) constexpr auto get() && noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND)
    -> template_element_t<I, storage>&& {
        return UTL_SCOPE move(tail).template get<I - 1>();
    }

    template <size_t I>
    requires (I > 0) && (I < element_count)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, HIDE_FROM_ABI) constexpr auto get() & noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND)
    -> template_element_t<I, storage>& {
        return tail.template get<I - 1>();
    }

    template <size_t I>
    requires (I > 0) && (I < element_count)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, HIDE_FROM_ABI) constexpr auto get() const&& noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) -> template_element_t<I, storage> const&& {
        return UTL_SCOPE move(tail).template get<I - 1>();
    }

    template <size_t I>
    requires (I > 0) && (I < element_count)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, HIDE_FROM_ABI) constexpr auto get() const& noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) -> template_element_t<I, storage> const& {
        return tail.template get<I - 1>();
    }

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) head_type head;
    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) tail_type tail;
};

template <typename... Ts>
struct offset_impl<0, storage<Ts...>> {
    using type UTL_NODEBUG = storage<Ts...>;
    static_assert(is_standard_layout<type>::value, "Must be standard layout");
    static constexpr size_t value = offsetof(type, head);
};

template <size_t I, typename T0, typename... Ts>
struct offset_impl<I, storage<T0, Ts...>, enable_if_t<(I > 0)>> {
    using type UTL_NODEBUG = storage<T0, Ts...>;
    static_assert(is_standard_layout<type>::value, "Must be standard layout");
    static_assert(I < type::element_count, "Index out of bounds");
    static constexpr size_t value =
        offset_impl<I - 1, storage<Ts...>>::value + offsetof(type, tail);
};

} // namespace tuple
} // namespace details

template <>
class UTL_PUBLIC_TEMPLATE tuple<> : private details::tuple::storage<> {
public:
    using storage::storage;
    using storage::operator=;
    UTL_HIDE_FROM_ABI constexpr void swap(tuple const& other) const noexcept {}
    UTL_HIDE_FROM_ABI friend inline constexpr void swap(tuple const&, tuple const&) noexcept {}
};

template <typename... Types>
class UTL_PUBLIC_TEMPLATE tuple : private details::tuple::storage<Types...> {
private:
    template <size_t I, typename T>
    friend struct tuple_element_offset;
    static constexpr struct private_tag_t {
    } private_tag = {};

    using base_type UTL_NODEBUG = details::tuple::storage<Types...>;
    using swap_type UTL_NODEBUG = conditional_t<conjunction<is_swappable<Types>...>::value, tuple,
        details::tuple::invalid_swap_t<Types...>>;
    using const_swap_type UTL_NODEBUG =
        conditional_t<conjunction<is_swappable<Types const>...>::value, tuple,
            details::tuple::invalid_swap_t<Types...>> const;
    using move_assign_t UTL_NODEBUG =
        conditional_t<conjunction<is_move_assignable<Types>...>::value, details::tuple::invalid_t,
            tuple>;
    using move_construct_t UTL_NODEBUG =
        conditional_t<conjunction<is_move_constructible<Types>...>::value,
            details::tuple::invalid_t, tuple>;

    using base_type::get;

    template <typename TupleLike, size_t... Is>
    requires tuple_like<remove_reference_t<TupleLike>>
    UTL_HIDE_FROM_ABI constexpr tuple(TupleLike&& other, index_sequence<Is...>) noexcept(
        details::tuple::is_nothrow_accessible_v<TupleLike> &&
        is_nothrow_constructible_v<base_type,
            decltype(UTL_SCOPE details::tuple::decl_element<Is, TupleLike>())...>)
        : base_type(UTL_SCOPE get_element<Is>(UTL_SCOPE forward<TupleLike>(other))...) {}

    template <allocator_type Alloc, typename TupleLike, size_t... Is>
    requires tuple_like<remove_reference_t<TupleLike>>
    UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other,
        index_sequence<Is...>) noexcept(details::tuple::is_nothrow_accessible_v<TupleLike> &&
        is_nothrow_constructible_v<base_type, allocator_arg_t, Alloc const&,
            decltype(UTL_SCOPE details::tuple::decl_element<Is, TupleLike>())...>)
        : base_type(allocator_arg, alloc,
              UTL_SCOPE get_element<Is>(UTL_SCOPE forward<TupleLike>(other))...) {}

    template <typename TupleLike, size_t... Is>
    requires tuple_like<remove_reference_t<TupleLike>>
    UTL_HIDE_FROM_ABI constexpr tuple& assign(TupleLike&& other, index_sequence<Is...>) noexcept(
        details::tuple::is_nothrow_accessible_v<TupleLike>&& noexcept(
            UTL_SCOPE declval<base_type&>().assign(
                UTL_SCOPE get_element<Is>(UTL_SCOPE declval<TupleLike>())...))) {
        return (tuple&)base_type::assign(
            UTL_SCOPE get_element<Is>(UTL_SCOPE forward<TupleLike>(other))...);
    }

    template <typename TupleLike, size_t... Is>
    requires tuple_like<remove_reference_t<TupleLike>>
    UTL_HIDE_FROM_ABI constexpr tuple const& assign(TupleLike&& other, index_sequence<Is...>) const
        noexcept(details::tuple::is_nothrow_accessible_v<TupleLike>&& noexcept(
            UTL_SCOPE declval<base_type const&>().assign(
                UTL_SCOPE get_element<Is>(UTL_SCOPE declval<TupleLike>())...))) {
        return (tuple const&)base_type::assign(
            UTL_SCOPE get_element<Is>(UTL_SCOPE forward<TupleLike>(other))...);
    }

public:
    UTL_HIDE_FROM_ABI constexpr tuple(tuple const&) noexcept(
        (... && is_nothrow_copy_constructible_v<Types>)) = default;
    UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple const&) noexcept(
        (... && is_nothrow_copy_assignable_v<Types>)) = default;
    UTL_HIDE_FROM_ABI constexpr tuple(tuple&&) noexcept(
        (... && is_nothrow_move_constructible_v<Types>)) = default;
    UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple&&) noexcept(
        (... && is_nothrow_move_assignable_v<Types>)) = default;

    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 ~tuple() = default;

    template <same_as<tuple> U = tuple>
    UTL_HIDE_FROM_ABI constexpr tuple const& operator=(U const& other) const
        noexcept(noexcept(assign(other, index_sequence_for<Types...>{}))) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <same_as<tuple> U = tuple>
    UTL_HIDE_FROM_ABI constexpr tuple const& operator=(U&& other) const
        noexcept(noexcept(assign(UTL_SCOPE declval<U>(), index_sequence_for<Types...>{}))) {
        return assign(UTL_SCOPE move(other), index_sequence_for<Types...>{});
    }

public:
    template <same_as<Types>... Us>
    requires (... && swappable_with<Types&, Us&>)
    UTL_HIDE_FROM_ABI constexpr void swap(tuple<Us...>& other) noexcept(
        (... && is_nothrow_swappable_with_v<Types&, Us&>)) {
        static_assert(is_base_of<details::tuple::storage<Us...>, tuple<Us...>>::value,
            "tuple must inherit from storage");
        base_type::swap((details::tuple::storage<Us...>&)other);
    }

    template <same_as<Types>... Us>
    requires (... && swappable_with<Types const&, Us const&>)
    UTL_HIDE_FROM_ABI constexpr void swap(tuple<Us...> const& other) const
        noexcept((... && is_nothrow_swappable_with_v<Types const&, Us const&>)) {
        static_assert(is_base_of<details::tuple::storage<Us...>, tuple<Us...>>::value,
            "tuple must inherit from storage");
        base_type::swap((details::tuple::storage<Us...> const&)other);
    }

    UTL_ATTRIBUTES(HIDE_FROM_ABI, FLATTEN) friend inline constexpr void swap(
        swap_type& l, swap_type& r) noexcept((... && is_nothrow_swappable_v<Types>)) {
        l.swap(r);
    }

    UTL_ATTRIBUTES(HIDE_FROM_ABI, FLATTEN) friend inline constexpr void swap(const_swap_type& l,
        const_swap_type& r) noexcept((... && is_nothrow_swappable_v<Types const>)) {
        l.swap(r);
    }

public:
    UTL_HIDE_FROM_ABI explicit(
        (... || is_explicit_constructible_v<Types>)) constexpr tuple() noexcept((... &&
        is_nothrow_default_constructible_v<Types>)) = default;

public:
    UTL_HIDE_FROM_ABI explicit((... ||
        is_explicit_constructible_v<Types,
            Types const&>)) constexpr tuple(Types const&... args) noexcept((... &&
        is_nothrow_copy_constructible_v<Types>))
        : base_type(args...) {}

private:
    template <constructible_as<Types>... Us>
    requires (... && !reference_constructs_from_temporary_v<Types, Us>)
    UTL_HIDE_FROM_ABI constexpr tuple(private_tag_t, Us&&... args) noexcept(
        is_nothrow_constructible_v<base_type, Us...>)
        : base_type(UTL_SCOPE forward<Us>(args)...) {}

    template <constructible_as<Types>... Us>
    requires (... || reference_constructs_from_temporary_v<Types, Us>)
    constexpr tuple(private_tag_t, Us&&... args) noexcept(
        is_nothrow_constructible_v<base_type, Us...>) = delete;

    template <constructible_as<Types>... Us>
    static constexpr bool explicit_constructible_v =
        (... || is_explicit_constructible_v<Types, Us>);

public:
    template <not_resolvable_to<private_tag_t> UHead, typename... UTail>
    UTL_HIDE_FROM_ABI explicit(explicit_constructible_v<UHead, UTail...>) constexpr tuple(
        UHead&& head,
        UTail&&... tail) noexcept(is_nothrow_constructible_v<base_type, UHead, UTail...>)
        : tuple(private_tag, UTL_SCOPE forward<UHead>(head), UTL_SCOPE forward<UTail>(tail)...) {}

public:
    template <constructible_as<Types, add_lvalue_reference>... UTypes>
    requires (... && !reference_constructs_from_temporary_v<Types, UTypes&>)
    UTL_HIDE_FROM_ABI explicit(explicit_constructible_v<UTypes&...>) constexpr tuple(
        tuple<UTypes...>& other) noexcept((... && is_nothrow_constructible_v<Types, UTypes&>))
        : tuple(other, index_sequence_for<UTypes...>{}) {}

    template <constructible_as<Types, add_lvalue_reference>... UTypes>
    requires (... || reference_constructs_from_temporary_v<Types, UTypes&>)
    explicit(explicit_constructible_v<UTypes&...>) constexpr tuple(
        tuple<UTypes...>& other) noexcept((... &&
        is_nothrow_constructible_v<Types, UTypes&>)) = delete;

public:
    template <constructible_as<Types, details::tuple::add_const_lvalue_reference>... UTypes>
    requires (... && !reference_constructs_from_temporary_v<Types, UTypes const&>)
    UTL_HIDE_FROM_ABI explicit(explicit_constructible_v<UTypes const&...>) constexpr tuple(
        tuple<UTypes...> const& other) noexcept((... &&
        is_nothrow_constructible_v<Types, UTypes const&>))
        : tuple(other, index_sequence_for<UTypes...>{}) {}

    template <constructible_as<Types, details::tuple::add_const_lvalue_reference>... UTypes>
    requires (... || reference_constructs_from_temporary_v<Types, UTypes const&>)
    explicit(explicit_constructible_v<UTypes const&...>) constexpr tuple(
        tuple<UTypes...> const& other) noexcept((... &&
        is_nothrow_constructible_v<Types, UTypes const&>)) = delete;

public:
    template <constructible_as<Types, add_rvalue_reference>... UTypes>
    requires (... && !reference_constructs_from_temporary_v<Types, UTypes &&>)
    UTL_HIDE_FROM_ABI explicit(explicit_constructible_v<UTypes&&...>) constexpr tuple(
        tuple<UTypes...>&& other) noexcept((... && is_nothrow_constructible_v<Types, UTypes&&>))
        : tuple(UTL_SCOPE move(other), index_sequence_for<UTypes...>{}) {}

    template <constructible_as<Types, add_rvalue_reference>... UTypes>
    requires (... || reference_constructs_from_temporary_v<Types, UTypes &&>)
    explicit(explicit_constructible_v<UTypes&&...>) constexpr tuple(
        tuple<UTypes...>&& other) noexcept((... &&
        is_nothrow_constructible_v<Types, UTypes&&>)) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <variadic_match<Types>... UTypes>
    requires (... || !is_constructible_v<Types, UTypes>)
    constexpr tuple(tuple<UTypes...>&&) = delete;
#endif

public:
    template <constructible_as<Types, details::tuple::add_const_rvalue_reference>... UTypes>
    requires (... && !reference_constructs_from_temporary_v<Types, UTypes const &&>)
    UTL_HIDE_FROM_ABI explicit(explicit_constructible_v<UTypes const&&...>) constexpr tuple(
        tuple<UTypes...> const&& other) noexcept((... &&
        is_nothrow_constructible_v<Types, UTypes const&&>))
        : tuple(UTL_SCOPE move(other), index_sequence_for<UTypes...>{}) {}

    template <constructible_as<Types, details::tuple::add_const_rvalue_reference>... UTypes>
    requires (... || reference_constructs_from_temporary_v<Types, UTypes const &&>)
    explicit(explicit_constructible_v<UTypes const&&...>) constexpr tuple(
        tuple<UTypes...> const&& other) noexcept((... &&
        is_nothrow_constructible_v<Types, UTypes const&&>)) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <variadic_match<Types>... UTypes>
    requires (... || !is_constructible_v<Types, UTypes const &&>)
    constexpr tuple(tuple<UTypes...> const&&) = delete;
#endif

private:
    template <details::tuple::unrecognized TupleLike>
    static constexpr bool is_explicit_v = is_explicit_constructible_v<tuple,
        decltype(details::tuple::forward_unrecognized(UTL_SCOPE declval<TupleLike>()))>;

public:
    template <details::tuple::unrecognized TupleLike>
    requires (is_constructible_v<tuple,
        decltype(details::tuple::forward_unrecognized(UTL_SCOPE declval<TupleLike>()))>)
    UTL_HIDE_FROM_ABI explicit(is_explicit_v<TupleLike>) constexpr tuple(TupleLike&& t) noexcept(
        details::tuple::is_nothrow_accessible_v<TupleLike> &&
        is_nothrow_constructible_v<tuple,
            decltype(details::tuple::forward_unrecognized(UTL_SCOPE declval<TupleLike>()))>)
        : tuple(details::tuple::forward_unrecognized(UTL_SCOPE forward<TupleLike>(t))) {}

private:
    template <allocator_type Alloc>
    static constexpr bool is_ctor_with_alloc_v =
        (... || (is_constructible_with_allocator_v<Types, Alloc>));
    template <allocator_type Alloc>
    static constexpr bool is_explicit_with_alloc_v = (... ||
        (is_explicit_constructible_with_allocator_v<Types, Alloc> ||
            is_explicit_constructible_v<Types>));

    template <allocator_type Alloc, details::tuple::unrecognized TupleLike>
    static constexpr bool is_explicit_tuple_with_alloc_v =
        is_explicit_constructible_v<tuple, Alloc const&,
            decltype(details::tuple::forward_unrecognized(UTL_SCOPE declval<TupleLike>()))>;

    template <allocator_type Alloc, variadic_match<Types>... Args>
    static constexpr bool is_nothrow_with_alloc_v =
        is_nothrow_constructible_v<base_type, allocator_arg_t, Alloc const&, Args...>;

    template <allocator_type Alloc, variadic_match<Types>... Args>
    static constexpr bool is_ctor_with_alloc_args_v =
        (... || (is_constructible_with_allocator_v<Types, Alloc, Args>));

    template <allocator_type Alloc, variadic_match<Types>... Args>
    static constexpr bool is_explicit_with_alloc_args_v = (... ||
        (is_explicit_constructible_with_allocator_v<Types, Alloc, Args> ||
            is_explicit_constructible_v<Types, Args>));

    template <allocator_type Alloc, variadic_match<Types>... Args>
    static constexpr bool is_dangling_v = (... ||
        (!uses_allocator_v<Types, Alloc> && reference_constructs_from_temporary_v<Types, Args>));

public:
    template <allocator_type Alloc>
    requires (is_ctor_with_alloc_v<Alloc>)
    UTL_HIDE_FROM_ABI explicit(is_explicit_with_alloc_v<Alloc>) constexpr tuple(
        allocator_arg_t, Alloc const& alloc) noexcept(is_nothrow_with_alloc_v<Alloc>)
        : base_type(allocator_arg, alloc) {}

public:
    template <allocator_type Alloc>
    requires (is_ctor_with_alloc_args_v<Alloc, Types const&...>)
    UTL_HIDE_FROM_ABI explicit(
        is_explicit_with_alloc_args_v<Alloc, Types const&...>) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        Types const&... args) noexcept(is_nothrow_with_alloc_v<Alloc, Types const&...>)
        : base_type(allocator_arg, alloc, args...) {}

public:
    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (is_ctor_with_alloc_args_v<Alloc, UTypes...> && !is_dangling_v<Alloc, UTypes...>)
    UTL_HIDE_FROM_ABI explicit(is_explicit_with_alloc_args_v<Alloc, UTypes...>) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        UTypes&&... args) noexcept(is_nothrow_with_alloc_v<Alloc, UTypes...>)
        : base_type(allocator_arg, alloc, forward<UTypes>(args)...) {}

    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (is_ctor_with_alloc_args_v<Alloc, UTypes...> && is_dangling_v<Alloc, UTypes...>)
    explicit(is_explicit_with_alloc_args_v<Alloc, UTypes...>) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        UTypes&&... args) noexcept(is_nothrow_with_alloc_v<Alloc, UTypes...>) = delete;

public:
    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (is_ctor_with_alloc_args_v<Alloc, UTypes&...> && !is_dangling_v<Alloc, UTypes&...>)
    UTL_HIDE_FROM_ABI explicit(is_explicit_with_alloc_args_v<Alloc, UTypes&...>) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...>& other) noexcept(is_nothrow_with_alloc_v<Alloc, UTypes&...>)
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (is_ctor_with_alloc_args_v<Alloc, UTypes&...> && is_dangling_v<Alloc, UTypes&...>)
    explicit(is_explicit_with_alloc_args_v<Alloc, UTypes&...>) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        tuple<UTypes...>& other) noexcept(is_nothrow_with_alloc_v<Alloc, UTypes&...>) = delete;

public:
    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (is_ctor_with_alloc_args_v<Alloc, UTypes const&...> &&
        !is_dangling_v<Alloc, UTypes const&...>)
    UTL_HIDE_FROM_ABI explicit(
        is_explicit_with_alloc_args_v<Alloc, UTypes const&...>) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        tuple<UTypes...> const& other) noexcept(is_nothrow_with_alloc_v<Alloc, UTypes const&...>)
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (is_ctor_with_alloc_args_v<Alloc, UTypes const&...> &&
                 is_dangling_v<Alloc, UTypes const&...>)
    explicit(is_explicit_with_alloc_args_v<Alloc, UTypes const&...>) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...> const& other) noexcept(is_nothrow_with_alloc_v<Alloc, UTypes const&...>) =
        delete;

public:
    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (
        is_ctor_with_alloc_args_v<Alloc, UTypes && ...> && !is_dangling_v<Alloc, UTypes && ...>)
    UTL_HIDE_FROM_ABI explicit(is_explicit_with_alloc_args_v<Alloc, UTypes&&...>) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...>&& other) noexcept(is_nothrow_with_alloc_v<Alloc, UTypes&&...>)
        : tuple(allocator_arg, alloc, UTL_SCOPE move(other), index_sequence_for<UTypes...>{}) {}

    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (is_ctor_with_alloc_args_v<Alloc, UTypes && ...> &&
                 is_dangling_v<Alloc, UTypes && ...>)
    explicit(is_explicit_with_alloc_args_v<Alloc, UTypes&&...>) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        tuple<UTypes...>&& other) noexcept(is_nothrow_with_alloc_v<Alloc, UTypes&&...>) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (!is_ctor_with_alloc_args_v<Alloc, UTypes && ...>::value)
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...>&& other) = delete;
#endif

public:
    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (is_ctor_with_alloc_args_v<Alloc, UTypes const && ...> &&
        !is_dangling_v<Alloc, UTypes const && ...>)
    UTL_HIDE_FROM_ABI explicit(
        is_explicit_with_alloc_args_v<Alloc, UTypes const&&...>) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        tuple<UTypes...> const&& other) noexcept(is_nothrow_with_alloc_v<Alloc, UTypes const&&...>)
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (is_ctor_with_alloc_args_v<Alloc, UTypes const && ...> &&
                 is_dangling_v<Alloc, UTypes const && ...>)
    explicit(is_explicit_with_alloc_args_v<Alloc, UTypes const&&...>) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...> const&&
            other) noexcept(is_nothrow_with_alloc_v<Alloc, UTypes const&&...>) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <allocator_type Alloc, variadic_match<Types>... UTypes>
    requires (!is_ctor_with_alloc_args_v<Alloc, UTypes const && ...>::value)
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...> const&& other) = delete;
#endif

public:
    template <allocator_type Alloc, details::tuple::unrecognized TupleLike>
    requires (is_constructible_v<tuple, Alloc const&,
        decltype(details::tuple::forward_unrecognized(UTL_SCOPE declval<TupleLike>()))>)
    UTL_HIDE_FROM_ABI explicit(is_explicit_tuple_with_alloc_v<Alloc, TupleLike>) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        TupleLike&& other) noexcept(details::tuple::is_nothrow_accessible_v<TupleLike> &&
        is_nothrow_constructible_v<tuple, Alloc const&,
            decltype(details::tuple::forward_unrecognized(UTL_SCOPE declval<TupleLike>()))>)
        : tuple(allocator_arg, alloc, UTL_SCOPE forward<TupleLike>(other),
              index_sequence_for<Types...>{}) {}

public:
    template <allocator_type Alloc>
    requires (
        is_ctor_with_alloc_args_v<Alloc, Types const&...> && !is_dangling_v<Alloc, Types const&...>)
    UTL_HIDE_FROM_ABI explicit(
        is_explicit_with_alloc_args_v<Alloc, Types const&...>) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        tuple const& other) noexcept(is_nothrow_with_alloc_v<Alloc, Types const&...>)
        : tuple(allocator_arg, alloc, other, index_sequence_for<Types...>{}) {}

    template <allocator_type Alloc>
    requires (is_ctor_with_alloc_args_v<Alloc, Types && ...> && !is_dangling_v<Alloc, Types && ...>)
    UTL_HIDE_FROM_ABI explicit(is_explicit_with_alloc_args_v<Alloc, Types&&...>) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        tuple&& other) noexcept(is_nothrow_with_alloc_v<Alloc, Types&&...>)
        : tuple(allocator_arg, alloc, UTL_SCOPE move(other), index_sequence_for<Types...>{}) {}

public:
    template <assignable_to<Types&, details::tuple::add_const_lvalue_reference>... UTypes>
    UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple<UTypes...> const& other) noexcept(
        (... && is_nothrow_assignable_v<Types&, UTypes const&>)) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <assignable_to<Types const&, details::tuple::add_const_lvalue_reference>... UTypes>
    UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple<UTypes...> const& other) const
        noexcept((... && is_nothrow_assignable_v<Types const&, UTypes const&>)) {
        return assign(other, index_sequence_for<Types...>{});
    }

public:
    template <assignable_to<Types&, add_lvalue_reference>... UTypes>
    UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple<UTypes...>& other) noexcept(
        (... && is_nothrow_assignable_v<Types&, UTypes&>)) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <assignable_to<Types const&, add_lvalue_reference>... UTypes>
    UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple<UTypes...>& other) const
        noexcept((... && is_nothrow_assignable_v<Types const&, UTypes&>)) {
        return assign(other, index_sequence_for<Types...>{});
    }

public:
    template <assignable_to<Types&, add_rvalue_reference>... UTypes>
    UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple<UTypes...>&& other) noexcept(
        (... && is_nothrow_assignable_v<Types&, UTypes&&>)) {
        return assign(UTL_SCOPE move(other), index_sequence_for<Types...>{});
    }

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes>
    constexpr tuple& operator=(tuple<UTypes...>&& other) = delete;
#endif

public:
    template <assignable_to<Types const&, add_rvalue_reference>... UTypes>
    UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple<UTypes...>&& other) const
        noexcept((... && is_nothrow_assignable_v<Types const&, UTypes&&>)) {
        return assign(UTL_SCOPE move(other), index_sequence_for<Types...>{});
    }

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes>
    constexpr tuple const& operator=(tuple<UTypes...>&& other) const = delete;
#endif

public:
    template <assignable_to<Types&, details::tuple::add_const_rvalue_reference>... UTypes>
    UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple<UTypes...> const&& other) noexcept(
        (... && is_nothrow_assignable_v<Types&, UTypes const&&>)) {
        return assign(UTL_SCOPE move(other), index_sequence_for<Types...>{});
    }

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes>
    constexpr tuple& operator=(tuple<UTypes...> const&& other) = delete;
#endif

public:
    template <assignable_to<Types const&, details::tuple::add_const_rvalue_reference>... UTypes>
    UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple<UTypes...> const&& other) const
        noexcept((... && is_nothrow_assignable_v<Types const&, UTypes const&&>)) {
        return assign(UTL_SCOPE move(other), index_sequence_for<Types...>{});
    }

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes>
    constexpr tuple const& operator=(tuple<UTypes...> const&& other) const = delete;
#endif

public:
    template <details::tuple::unrecognized TupleLike>
    requires (is_assignable_v<tuple&,
        decltype(details::tuple::forward_unrecognized(UTL_SCOPE declval<TupleLike>()))>)
    UTL_HIDE_FROM_ABI constexpr tuple& operator=(TupleLike&& t) noexcept(
        details::tuple::is_nothrow_accessible_v<TupleLike> &&
        is_nothrow_assignable_v<tuple&,
            decltype(details::tuple::forward_unrecognized(UTL_SCOPE declval<TupleLike>()))>) {
        return *this = details::tuple::forward_unrecognized(UTL_SCOPE forward<TupleLike>(t));
    }

    template <details::tuple::unrecognized TupleLike>
    requires (is_assignable_v<tuple const&,
        decltype(details::tuple::forward_unrecognized(UTL_SCOPE declval<TupleLike>()))>)
    UTL_HIDE_FROM_ABI constexpr tuple const& operator=(TupleLike&& t) const
        noexcept(details::tuple::is_nothrow_accessible_v<TupleLike> &&
            is_nothrow_assignable_v<tuple const&,
                decltype(details::tuple::forward_unrecognized(UTL_SCOPE declval<TupleLike>()))>) {
        return *this = details::tuple::forward_unrecognized(UTL_SCOPE forward<TupleLike>(t));
    }
};

template <size_t I, typename T>
struct tuple_element_offset;

template <size_t I, typename... Ts>
struct tuple_element_offset<I, tuple<Ts...>> :
    enable_if_t<is_base_of<details::tuple::storage<Ts...>, tuple<Ts...>>::value,
        details::tuple::offset_impl<I, details::tuple::storage<Ts...>>> {};

template <typename... Ts>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr tuple<unwrap_reference_t<decay_t<Ts>>...>
make_tuple(Ts&&... ts) noexcept(
    is_nothrow_constructible<tuple<unwrap_reference_t<decay_t<Ts>>...>, Ts...>::value) {
    return tuple<unwrap_reference_t<decay_t<Ts>>...>{forward<Ts>(ts)...};
}

namespace details {
namespace tuple {

template <tuple_like T, tuple_like U, size_t... Is>
UTL_HIDE_FROM_ABI auto three_way_result(T const& l, U const& r, index_sequence<Is...>) noexcept
    -> common_comparison_category_t<decltype(UTL_SCOPE get_element<Is>(l) <=>
        UTL_SCOPE get_element<Is>(r))...>;

template <tuple_like T, tuple_like U, size_t... Is>
UTL_HIDE_FROM_ABI auto is_nothrow_three_way(T const& l, U const& r, index_sequence<Is...>) noexcept
    -> bool_constant<(
        ...&& noexcept(UTL_SCOPE get_element<Is>(l) <=> UTL_SCOPE get_element<Is>(r)))>;

template <typename T, typename U>
requires tuple_like<remove_reference_t<T>> && tuple_like<remove_reference_t<U>>
using three_way_result_t UTL_NODEBUG = decltype(three_way_result(
    UTL_SCOPE declval<T>(), UTL_SCOPE declval<U>(), tuple_index_sequence<T>{}));

template <typename T, typename U>
requires tuple_like<remove_reference_t<T>> && tuple_like<remove_reference_t<U>>
UTL_INLINE_CXX17 constexpr bool is_nothrow_three_way_v = decltype(is_nothrow_three_way(
    UTL_SCOPE declval<T>(), UTL_SCOPE declval<U>(), tuple_index_sequence<T>{}))::value;

template <size_t I, tuple_like T, tuple_like U>
requires (I == tuple_size_v<T>)
UTL_ATTRIBUTES(CONST, HIDE_FROM_ABI) constexpr three_way_result_t<T, U> three_way(T const&, U const&) noexcept {
    return three_way_result_t<T, U>::equal;
}

template <size_t I, tuple_like T, tuple_like U>
requires (I < tuple_size_v<T>)
UTL_HIDE_FROM_ABI constexpr three_way_result_t<T, U> three_way(T const& l, U const& r) noexcept(
    is_nothrow_three_way_v<T, U>) {
    auto c = UTL_SCOPE get_element<I>(l) <=> UTL_SCOPE get_element<I>(r);
    return c != 0 ? c : three_way<I + 1>(l, r);
}

template <tuple_like T, tuple_like U>
UTL_HIDE_FROM_ABI constexpr three_way_result_t<T, U> three_way(T const& l, U const& r) noexcept(
    noexcept(three_way<0>(l, r))) {
    return three_way<0>(l, r);
}

template <size_t I, tuple_like T, tuple_like U>
requires (I == tuple_size_v<T>)
UTL_ATTRIBUTES(CONST, HIDE_FROM_ABI) constexpr bool equals(T const& l, U const& r) noexcept {
    return true;
}

template <size_t I, tuple_like T, tuple_like U>
requires (I < tuple_size_v<T>)
UTL_HIDE_FROM_ABI constexpr bool equals(T const& l, U const& r) noexcept(
    is_nothrow_accessible_v<T> && is_nothrow_accessible_v<U> &&
    compare_ops::all_have_nothrow_eq<T, U>::value) {
    return (UTL_SCOPE get_element<I>(l) == UTL_SCOPE get_element<I>(r)) && equals<I + 1>(l, r);
}

template <tuple_like T, tuple_like U>
UTL_ATTRIBUTES(FLATTEN, HIDE_FROM_ABI) inline constexpr bool equals(T const& l, U const& r) noexcept(
    noexcept(equals<0>(l, r))) {
    static_assert(compare_ops::all_have_eq<T, U>::value, "All elements must be comparable");
    return equals<0>(l, r);
}

} // namespace tuple
} // namespace details

template <typename... Ts, equality_comparable_with<Ts>... Us>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr bool operator==(
    tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(details::tuple::equals(l, r))) {
    return details::tuple::equals(l, r);
}

UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr bool operator==(tuple<> const&, tuple<> const&) noexcept {
    return true;
}

template <typename... Ts, three_way_comparable_with<Ts>... Us>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr auto operator<=>(tuple<Ts...> const& l,
    tuple<Us...> const& r) noexcept(noexcept(details::tuple::three_way(l, r))) {
    return details::tuple::three_way(l, r);
}

UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr UTL_SCOPE strong_ordering operator<=>(
    tuple<> const&, tuple<> const&) noexcept {
    return UTL_SCOPE strong_ordering::equal;
}

template <typename... Args>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr tuple<Args&...> tie(
    Args&... args UTL_LIFETIMEBOUND) noexcept {
    return {args...};
}

template <typename... Args>
UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) constexpr tuple<Args&&...> forward_as_tuple(
    Args&&... args UTL_LIFETIMEBOUND) noexcept {
    return {UTL_SCOPE forward<Args>(args)...};
}

UTL_NAMESPACE_END
