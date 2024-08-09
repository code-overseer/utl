// Copyright 2023-2024 Bryan Wong

#pragma once

/**
 * Standard Tuple implementation up to and including C++17
 */

#if !defined(UTL_TUPLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/preprocessor/utl_config.h"

#include "utl/tuple/utl_tuple_fwd.h"

#include "utl/memory/utl_uses_allocator.h"
#include "utl/ranges/utl_swap.h"
#include "utl/tuple/utl_tuple_compare_traits.h"
#include "utl/tuple/utl_tuple_traits.h"
#include "utl/type_traits/utl_common_reference.h"
#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_has_member_type.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_default_constructible.h"
#include "utl/type_traits/utl_is_explicit_constructible.h"
#include "utl/type_traits/utl_is_move_assignable.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_standard_layout.h"
#include "utl/type_traits/utl_template_list.h"
#include "utl/type_traits/utl_unwrap_reference.h"
#include "utl/type_traits/utl_variadic_proxy.h"
#include "utl/type_traits/utl_variadic_traits.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_ignore.h"
#include "utl/utility/utl_move.h"
#include "utl/utility/utl_sequence.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {
template <typename, typename, typename = void>
struct common_type_impl {};

template <typename... Ts, typename... Us>
struct common_type_impl<UTL_SCOPE tuple<Ts...>, UTL_SCOPE tuple<Us...>,
    void_t<common_type_t<Ts, Us>...>> {
    using type = UTL_SCOPE tuple<common_type_t<Ts, Us>...>;
};

template <typename, typename, template <typename> class TQual, template <typename> class UQual,
    typename = void>
struct basic_ref_impl {};

template <typename... Ts, typename... Us, template <typename> class TQual,
    template <typename> class UQual>
struct basic_ref_impl<UTL_SCOPE tuple<Ts...>, UTL_SCOPE tuple<Us...>, TQual, UQual,
    void_t<common_reference_t<TQual<Ts>, UQual<Us>>...>> {
    using type = UTL_SCOPE tuple<common_reference_t<TQual<Ts>, UQual<Us>>...>;
};
} // namespace tuple
} // namespace details

UTL_NAMESPACE_END

// TODO: if std is included or forward declared use std, else use UTL
namespace std {
template <typename... Ts, typename... Us>
struct common_type<UTL_SCOPE tuple<Ts...>, UTL_SCOPE tuple<Us...>> :
    UTL_SCOPE details::tuple::common_type_impl<UTL_SCOPE tuple<Ts...>, UTL_SCOPE tuple<Us...>> {};
template <typename... Ts, typename... Us, template <typename> class TQual,
    template <typename> class UQual>
struct basic_common_reference<UTL_SCOPE tuple<Ts...>, UTL_SCOPE tuple<Us...>, TQual, UQual> :
    UTL_SCOPE details::tuple::basic_ref_impl<UTL_SCOPE tuple<Ts...>, UTL_SCOPE tuple<Us...>, TQual,
        UQual> {};
} // namespace std

#define TT_SCOPE UTL_SCOPE tuple_traits::

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {
template <typename...>
struct invalid_swap_t {
    inline UTL_CONSTEXPR_CXX14 void swap(invalid_swap_t& other) const noexcept {}
    inline UTL_CONSTEXPR_CXX14 void swap(invalid_swap_t const& other) const noexcept {}

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
    UTL_CONSTEXPR_CXX14 void swap(storage<>& other) noexcept {}
};

template <typename T>
struct storage<T> : variadic_traits<T> {
public:
    using traits = variadic_traits<T>;
    using head_type = T;
    using move_assign_t = conditional_t<traits::is_move_assignable, invalid_t, storage>;
    using move_construct_t = conditional_t<traits::is_move_constructible, invalid_t, storage>;

    constexpr storage() noexcept(traits::is_nothrow_default_constructible) = default;
    constexpr storage(storage const&) noexcept(traits::is_nothrow_copy_constructible) = default;
    UTL_CONSTEXPR_CXX14 storage& operator=(storage const&) noexcept(
        traits::is_nothrow_copy_assignable) = default;

#if UTL_ENFORCE_NONMOVABILIITY
    constexpr storage(move_construct_t&&) noexcept(traits::is_nothrow_move_constructible) = delete;
    UTL_CONSTEXPR_CXX14 storage& operator=(move_assign_t&&) noexcept(
        traits::is_nothrow_move_assignable) = delete;
#else
    constexpr storage(storage&&) noexcept(traits::is_nothrow_move_constructible) = default;
    UTL_CONSTEXPR_CXX14 storage& operator=(storage&&) noexcept(traits::is_nothrow_move_assignable) = default;
#endif

    template <typename U, enable_if_t<!is_same<U, storage>::value, int> = 1>
    constexpr storage(U&& head) noexcept(traits::template is_nothrow_constructible<U>::value)
        : head(forward<U>(head)) {}

    template <typename Alloc,
        enable_if_t<conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
                        UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>>::value,
            int> = 0>
    constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        UTL_SCOPE is_nothrow_constructible<T, allocator_arg_t, Alloc const&>::value)
        : head(allocator_arg, alloc) {}

    template <typename Alloc,
        enable_if_t<conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
                        negation<UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>>,
                        UTL_SCOPE is_constructible<T, Alloc const&>>::value,
            int> = 1>
    constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        UTL_SCOPE is_nothrow_constructible<T, Alloc const&>::value)
        : head(alloc) {}

    template <typename Alloc, enable_if_t<!UTL_SCOPE uses_allocator<T, Alloc>::value, int> = 2>
    constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        UTL_SCOPE is_nothrow_constructible<T>::value)
        : head() {}

    template <typename Alloc, typename U,
        enable_if_t<conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
                        UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&, U>>::value,
            int> = 0>
    constexpr storage(allocator_arg_t, Alloc const& alloc, U&& other_head) noexcept(
        UTL_SCOPE is_nothrow_constructible<T, allocator_arg_t, Alloc const&, U>::value)
        : head(allocator_arg, alloc, forward<U>(other_head)) {}

    template <typename Alloc, typename U,
        enable_if_t<conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
                        negation<UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&, U>>,
                        UTL_SCOPE is_constructible<T, U, Alloc const&>>::value,
            int> = 1>
    constexpr storage(allocator_arg_t, Alloc const& alloc, U&& other_head) noexcept(
        UTL_SCOPE is_nothrow_constructible<T, U, Alloc const&>::value)
        : head(forward<U>(other_head), alloc) {}

    template <typename Alloc, typename U,
        enable_if_t<!UTL_SCOPE uses_allocator<T, Alloc>::value, int> = 2>
    constexpr storage(allocator_arg_t, Alloc const& alloc, U&& other_head) noexcept(
        UTL_SCOPE is_nothrow_constructible<T, U>::value)
        : head(forward<U>(other_head)) {}

    template <typename U = T>
    UTL_CONSTEXPR_CXX14 void swap(storage<U>& other) noexcept(
        traits::template is_nothrow_swappable_with<U&>::value) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U = T>
    UTL_CONSTEXPR_CXX14 void swap(storage<U> const& other) noexcept(
        traits::template is_nothrow_swappable_with<U const&>::value) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U = T>
    UTL_CONSTEXPR_CXX14 void swap(storage<U>& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<U&>::value) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U = T>
    UTL_CONSTEXPR_CXX14 void swap(storage<U> const& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<U const&>::value) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U>
    UTL_CONSTEXPR_CXX14 storage& assign(U&& other) noexcept(
        traits::template is_nothrow_assignable<U>::value) {
        head = forward<U>(other);
        return *this;
    }

    template <typename U>
    constexpr storage const& assign(U&& other) const
        noexcept(traits::template is_nothrow_const_assignable<U>::value) {
        head = forward<U>(other);
        return *this;
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST)
    UTL_CONSTEXPR_CXX14 auto get() && noexcept UTL_LIFETIMEBOUND
    -> enable_if_t<!I, T&&> {
        return move(head);
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST)
    UTL_CONSTEXPR_CXX14 auto get() & noexcept UTL_LIFETIMEBOUND
    -> enable_if_t<!I, T&> {
        return head;
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() const&& noexcept UTL_LIFETIMEBOUND
    -> enable_if_t<!I, T const&&> {
        return move(head);
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() const& noexcept UTL_LIFETIMEBOUND
    -> enable_if_t<!I, T const&> {
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
    UTL_CONSTEXPR_CXX14 storage& operator=(storage const&) noexcept(
        traits::is_nothrow_copy_assignable) = default;

#if UTL_ENFORCE_NONMOVABILIITY
    constexpr storage(move_construct_t&&) noexcept(traits::is_nothrow_move_constructible) = delete;
    UTL_CONSTEXPR_CXX14 storage& operator=(move_assign_t&&) noexcept(
        traits::is_nothrow_move_assignable) = delete;
#else
    constexpr storage(storage&&) noexcept(traits::is_nothrow_move_constructible) = default;
    UTL_CONSTEXPR_CXX14 storage& operator=(storage&&) noexcept(traits::is_nothrow_move_assignable) = default;
#endif

    template <typename UHead, typename... UTail,
        enable_if_t<sizeof...(UTail) == sizeof...(Tail), int> = 1>
    constexpr storage(UHead&& other_head, UTail&&... other_tail) noexcept(
        traits::template is_nothrow_constructible<UHead, UTail...>::value)
        : head(forward<UHead>(other_head))
        , tail(forward<UTail>(other_tail)...) {}

    template <typename Alloc,
        enable_if_t<conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
                        UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>>::value,
            int> = 0>
    constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        conjunction<UTL_SCOPE is_nothrow_constructible<T, allocator_arg_t, Alloc const&>,
            UTL_SCOPE is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&>>::value)
        : head(allocator_arg, alloc)
        , tail(allocator_arg, alloc) {}

    template <typename Alloc,
        enable_if_t<conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
                        negation<UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>>,
                        UTL_SCOPE is_constructible<T, Alloc const&>>::value,
            int> = 1>
    constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        conjunction<UTL_SCOPE is_nothrow_constructible<T, Alloc const&>,
            UTL_SCOPE is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&>>::value)
        : head(alloc)
        , tail(allocator_arg, alloc) {}

    template <typename Alloc, enable_if_t<!UTL_SCOPE uses_allocator<T, Alloc>::value, int> = 2>
    constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        conjunction<UTL_SCOPE is_nothrow_constructible<T>,
            UTL_SCOPE is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&>>::value)
        : head()
        , tail(allocator_arg, alloc) {}

    template <typename Alloc, typename UHead, typename... UTail,
        enable_if_t<conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
                        UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&, UHead>>::value,
            int> = 0>
    constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(conjunction<UTL_SCOPE is_nothrow_constructible<T,
                                                        allocator_arg_t, Alloc const&, UHead>,
        UTL_SCOPE
            is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&, UTail...>>::value)
        : head(allocator_arg, alloc, forward<UHead>(other_head))
        , tail(allocator_arg, alloc, forward<UTail>(other_tail)...) {}

    template <typename Alloc, typename UHead, typename... UTail,
        enable_if_t<conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
                        negation<UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>>,
                        UTL_SCOPE is_constructible<T, UHead, Alloc const&>>::value,
            int> = 1>
    constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(conjunction<UTL_SCOPE is_nothrow_constructible<T, UHead,
                                                        Alloc const&>,
        UTL_SCOPE
            is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&, UTail...>>::value)
        : head(forward<UHead>(other_head), alloc)
        , tail(allocator_arg, alloc, forward<UTail>(other_tail)...) {}

    template <typename Alloc, typename UHead, typename... UTail,
        enable_if_t<!UTL_SCOPE uses_allocator<T, Alloc>::value, int> = 2>
    constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(conjunction<UTL_SCOPE is_nothrow_constructible<T>,
        UTL_SCOPE
            is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&, UTail...>>::value)
        : head(forward<UHead>(other_head))
        , tail(allocator_arg, alloc, forward<UTail>(other_tail)...) {}

    template <typename... Us>
    UTL_ATTRIBUTE(ALWAYS_INLINE)
    UTL_CONSTEXPR_CXX14 void swap(storage<Us&...>& other) noexcept(
        traits::template is_nothrow_swappable_with<Us&...>::value) {
        UTL_SCOPE ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename... Us>
    UTL_ATTRIBUTE(ALWAYS_INLINE)
    UTL_CONSTEXPR_CXX14 void swap(storage<Us...> const& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<Us const&...>::value) {
        UTL_SCOPE ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename... Us>
    UTL_ATTRIBUTE(ALWAYS_INLINE)
    UTL_CONSTEXPR_CXX14 void swap(storage<Us...>& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<Us&...>::value) {
        UTL_SCOPE ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename UHead, typename... UTail>
    UTL_ATTRIBUTE(ALWAYS_INLINE)
    UTL_CONSTEXPR_CXX14 storage& assign(UHead&& other_head, UTail&&... other_tail) noexcept(
        traits::template is_nothrow_assignable<UHead, UTail...>::value) {
        head = forward<UHead>(other_head);
        tail.assign(forward<UTail>(other_tail)...);
        return *this;
    }

    template <typename UHead, typename... UTail>
    UTL_ATTRIBUTE(ALWAYS_INLINE) constexpr storage const& assign(
        UHead&& other_head, UTail&&... other_tail) const
        noexcept(traits::template is_nothrow_const_assignable<UHead, UTail...>::value) {
        head = forward<UHead>(other_head);
        tail.assign(forward<UTail>(other_tail)...);
        return *this;
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST)
    UTL_CONSTEXPR_CXX14 auto get() && noexcept UTL_LIFETIMEBOUND
    -> enable_if_t<!I, T&&> {
        return move(head);
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST)
    UTL_CONSTEXPR_CXX14 auto get() & noexcept UTL_LIFETIMEBOUND
    -> enable_if_t<!I, T&> {
        return head;
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() const&& noexcept UTL_LIFETIMEBOUND
    -> enable_if_t<!I, T const&&> {
        return move(head);
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST) constexpr auto get() const& noexcept UTL_LIFETIMEBOUND
    -> enable_if_t<!I, T const&> {
        return head;
    }

    template <size_t I>
    using result_type_t = enable_if_t<(I > 0 && I < traits::size), template_element_t<I, storage>>;

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN)
    UTL_CONSTEXPR_CXX14 auto get() && noexcept UTL_LIFETIMEBOUND
    -> result_type_t<I>&& {
        return move(tail).template get<I - 1>();
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN)
    UTL_CONSTEXPR_CXX14 auto get() & noexcept UTL_LIFETIMEBOUND
    -> result_type_t<I>& {
        return tail.template get<I - 1>();
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) constexpr auto get() const&& noexcept UTL_LIFETIMEBOUND
    -> result_type_t<I> const&& {
        return move(tail).template get<I - 1>();
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN) constexpr auto get() const& noexcept UTL_LIFETIMEBOUND
    -> result_type_t<I> const& {
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
    UTL_CONSTEXPR_CXX14 void swap(tuple const& other) const noexcept {}
    friend inline UTL_CONSTEXPR_CXX14 void swap(tuple const&, tuple const&) noexcept {}
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
    constexpr tuple(TupleLike&& other, index_sequence<Is...>) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Is)>,
            TT_SCOPE rebind_references_t<traits::template is_nothrow_constructible, TupleLike,
                sizeof...(Is)>>::value)
        : base_type(UTL_TUPLE_GET(Is, forward<TupleLike>(other))...) {}

    template <typename Alloc, typename TupleLike, size_t... Is>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other,
        index_sequence<
            Is...>) noexcept(conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Is)>,
        TT_SCOPE rebind_references_t<
            variadic_proxy<traits::template is_nothrow_constructible_with_allocator,
                Alloc>::template apply,
            TupleLike, sizeof...(Types)>>::value)
        : base_type(allocator_arg, alloc, UTL_TUPLE_GET(Is, forward<TupleLike>(other))...) {}

    template <typename TupleLike, size_t... Is>
    UTL_CONSTEXPR_CXX14 tuple& assign(TupleLike&& other, index_sequence<Is...>) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Is)>,
            TT_SCOPE rebind_references_t<traits::template is_nothrow_assignable, TupleLike,
                sizeof...(Is)>>::value) {
        return (tuple&)base_type::assign(UTL_TUPLE_GET(Is, forward<TupleLike>(other))...);
    }

    template <typename TupleLike, size_t... Is>
    constexpr tuple const& assign(TupleLike&& other, index_sequence<Is...>) const
        noexcept(conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Is)>,
            TT_SCOPE rebind_references_t<traits::template is_nothrow_const_assignable, TupleLike,
                sizeof...(Is)>>::value) {
        return (tuple const&)base_type::assign(UTL_TUPLE_GET(Is, forward<TupleLike>(other))...);
    }

public:
    constexpr tuple(tuple const&) noexcept(traits::is_nothrow_copy_constructible) = default;
    UTL_CONSTEXPR_CXX14 tuple& operator=(tuple const& other) noexcept(
        traits::is_nothrow_copy_assignable) = default;
    constexpr tuple(tuple&&) noexcept(traits::is_nothrow_move_constructible) = default;
    UTL_CONSTEXPR_CXX14 tuple& operator=(tuple&&) noexcept(traits::is_nothrow_move_assignable) = default;

    UTL_CONSTEXPR_CXX20 ~tuple() = default;

    template <bool NotEmpty = (sizeof...(Types) >= 1),
        typename = enable_if_t<NotEmpty && traits::is_const_copy_assignable>>
    constexpr tuple const& operator=(tuple const& other) const
        noexcept(traits::is_nothrow_const_copy_assignable) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <bool NotEmpty = (sizeof...(Types) >= 1),
        typename = enable_if_t<NotEmpty && traits::is_const_move_assignable>>
    constexpr tuple const& operator=(tuple&& other) const
        noexcept(traits::is_nothrow_const_move_assignable) {
        return assign(move(other), index_sequence_for<Types...>{});
    }

public:
    template <typename... Us>
    UTL_CONSTEXPR_CXX14 enable_if_t<conjunction<typename traits::template matches<Us...>,
        typename traits::template is_swappable_with<Us&...>>::value>
    swap(tuple<Us...>& other) noexcept(traits::template is_nothrow_swappable_with<Us&...>::value) {
        static_assert(is_base_of<details::tuple::storage<Us...>, tuple<Us...>>::value,
            "tuple must inherit from storage");
        base_type::swap((details::tuple::storage<Us...>&)other);
    }

    template <typename... Us>
    UTL_CONSTEXPR_CXX14 enable_if_t<conjunction<typename traits::template matches<Us...>,
        typename traits::template is_swappable_with<Us const&...>>::value>
    swap(tuple<Us...> const& other) noexcept(
        traits::template is_nothrow_swappable_with<Us const&...>::value) {
        static_assert(is_base_of<details::tuple::storage<Us...>, tuple<Us...>>::value,
            "tuple must inherit from storage");
        base_type::swap((details::tuple::storage<Us...> const&)other);
    }

    friend inline UTL_CONSTEXPR_CXX14 void swap(swap_type& l, swap_type& r) noexcept(
        traits::is_nothrow_swappable) {
        l.swap(r);
    }

    friend inline UTL_CONSTEXPR_CXX14 void swap(const_swap_type& l, const_swap_type& r) noexcept(
        traits::is_nothrow_const_swappable) {
        l.swap(r);
    }

public:
    template <bool NotEmpty = (sizeof...(Types) >= 1),
        enable_if_t<NotEmpty && traits::is_implicit_default_constructible, int> = 0>
    constexpr tuple() noexcept(traits::is_nothrow_default_constructible) : base_type() {}

    template <bool NotEmpty = (sizeof...(Types) >= 1),
        enable_if_t<NotEmpty && traits::is_explicit_default_constructible, int> = 1>
    explicit constexpr tuple() noexcept(traits::is_nothrow_default_constructible) : base_type() {}

public:
    template <bool NotEmpty = (sizeof...(Types) >= 1),
        enable_if_t<
            conjunction<bool_constant<NotEmpty>,
                typename traits::template is_implicit_constructible<Types const&...>>::value,
            int> = 0>
    constexpr tuple(Types const&... args) noexcept(traits::is_nothrow_copy_constructible)
        : base_type(args...) {}

    template <bool NotEmpty = (sizeof...(Types) >= 1),
        enable_if_t<
            conjunction<bool_constant<NotEmpty>,
                typename traits::template is_explicit_constructible<Types const&...>>::value,
            int> = 1>
    explicit constexpr tuple(Types const&... args) noexcept(traits::is_nothrow_copy_constructible)
        : base_type(args...) {}

public:
    template <typename UHead, typename... UTail,
        enable_if_t<
            conjunction<bool_constant<sizeof...(Types) == sizeof...(UTail) + 1>,
                typename traits::template is_implicit_constructible<UHead, UTail...>,
                negation<typename traits::template is_dangling<UHead&&, UTail&&...>>>::value,
            int> = 0>
    constexpr tuple(UHead&& head, UTail&&... tail) noexcept(
        conjunction<bool_constant<sizeof...(Types) == sizeof...(UTail) + 1>,
            typename traits::template is_nothrow_constructible<UHead, UTail...>>::value)
        : base_type(forward<UHead>(head), forward<UTail>(tail)...) {}

    template <typename UHead, typename... UTail,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTail) + 1>,
                        typename traits::template is_explicit_constructible<UHead, UTail...>,
                        // negation<typename traits::template is_dangling<UHead&&, UTail&&...>>
                        true_type>::value,
            int> = 1>
    explicit constexpr tuple(UHead&& head, UTail&&... tail) noexcept(
        traits::template is_nothrow_constructible<UHead, UTail...>::value)
        : base_type(forward<UHead>(head), forward<UTail>(tail)...) {}

    template <typename UHead, typename... UTail,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTail) + 1>,
                        typename traits::template is_implicit_constructible<UHead, UTail...>,
                        typename traits::template is_dangling<UHead&&, UTail&&...>>::value,
            int> = 2>
    constexpr tuple(UHead&& head, UTail&&... tail) noexcept(
        traits::template is_nothrow_constructible<UHead, UTail...>::value) = delete;

    template <typename UHead, typename... UTail,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTail) + 1>,
                        typename traits::template is_explicit_constructible<UHead, UTail...>,
                        typename traits::template is_dangling<UHead&&, UTail&&...>>::value,
            int> = 3>
    explicit constexpr tuple(UHead&& head, UTail&&... tail) noexcept(
        traits::template is_nothrow_constructible<UHead, UTail...>::value) = delete;

public:
    template <typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible<UTypes&...>,
                        negation<typename traits::template is_dangling<UTypes&...>>>::value,
            int> = 0>
    constexpr tuple(tuple<UTypes...>& other) noexcept(
        traits::template is_nothrow_constructible<UTypes&...>::value)
        : tuple(other, index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible<UTypes&...>,
                        negation<typename traits::template is_dangling<UTypes&...>>>::value,
            int> = 1>
    explicit constexpr tuple(tuple<UTypes...>& other) noexcept(
        traits::template is_nothrow_constructible<UTypes&...>::value)
        : tuple(other, index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible<UTypes&...>,
                        typename traits::template is_dangling<UTypes&...>>::value,
            int> = 2>
    constexpr tuple(tuple<UTypes...>& other) noexcept(
        traits::template is_nothrow_constructible<UTypes&...>::value) = delete;

    template <typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible<UTypes&...>,
                        typename traits::template is_dangling<UTypes&...>>::value,
            int> = 3>
    explicit constexpr tuple(tuple<UTypes...>& other) noexcept(
        traits::template is_nothrow_constructible<UTypes&...>::value) = delete;

public:
    template <typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible<UTypes const&...>,
                        negation<typename traits::template is_dangling<UTypes const&...>>>::value,
            int> = 0>
    constexpr tuple(tuple<UTypes...> const& other) noexcept(
        traits::template is_nothrow_constructible<UTypes const&...>::value)
        : tuple(other, index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible<UTypes const&...>,
                        negation<typename traits::template is_dangling<UTypes const&...>>>::value,
            int> = 1>
    explicit constexpr tuple(tuple<UTypes...> const& other) noexcept(
        traits::template is_nothrow_constructible<UTypes const&...>::value)
        : tuple(other, index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible<UTypes const&...>,
                        typename traits::template is_dangling<UTypes const&...>>::value,
            int> = 2>
    constexpr tuple(tuple<UTypes...> const&) noexcept(
        traits::template is_nothrow_constructible<UTypes const&...>::value) = delete;

    template <typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible<UTypes const&...>,
                        typename traits::template is_dangling<UTypes const&...>>::value,
            int> = 3>
    explicit constexpr tuple(tuple<UTypes...> const&) noexcept(
        traits::template is_nothrow_constructible<UTypes const&...>::value) = delete;

public:
    template <typename... UTypes,
        enable_if_t<conjunction<typename traits::template is_implicit_constructible<UTypes&&...>,
                        negation<typename traits::template is_dangling<UTypes&&...>>>::value,
            int> = 0>
    constexpr tuple(tuple<UTypes...>&& other) noexcept(
        traits::template is_nothrow_constructible<UTypes&&...>::value)
        : tuple(move(other), index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<conjunction<typename traits::template is_explicit_constructible<UTypes&&...>,
                        negation<typename traits::template is_dangling<UTypes&&...>>>::value,
            int> = 1>
    explicit constexpr tuple(tuple<UTypes...>&& other) noexcept(
        traits::template is_nothrow_constructible<UTypes&&...>::value)
        : tuple(move(other), index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<conjunction<typename traits::template is_implicit_constructible<UTypes&&...>,
                        typename traits::template is_dangling<UTypes&&...>>::value,
            int> = 2>
    constexpr tuple(tuple<UTypes...>&&) noexcept(
        traits::template is_nothrow_constructible<UTypes&&...>::value) = delete;

    template <typename... UTypes,
        enable_if_t<conjunction<typename traits::template is_explicit_constructible<UTypes&&...>,
                        typename traits::template is_dangling<UTypes&&...>>::value,
            int> = 3>
    explicit constexpr tuple(tuple<UTypes...>&&) noexcept(
        traits::template is_nothrow_constructible<UTypes&&...>::value) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes,
        enable_if_t<
            disjunction<negation<typename traits::template is_constructible<UTypes&&...>>>::value,
            int> = 4>
    constexpr tuple(tuple<UTypes...>&&) = delete;

#endif

public:
    template <typename... UTypes,
        enable_if_t<
            conjunction<typename traits::template is_implicit_constructible<UTypes const&&...>,
                negation<typename traits::template is_dangling<UTypes const&&...>>>::value,
            int> = 0>
    constexpr tuple(tuple<UTypes...> const&& other) noexcept(
        traits::template is_nothrow_constructible<UTypes const&&...>::value)
        : tuple(move(other), index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<
            conjunction<typename traits::template is_explicit_constructible<UTypes const&&...>,
                negation<typename traits::template is_dangling<UTypes const&&...>>>::value,
            int> = 1>
    explicit constexpr tuple(tuple<UTypes...> const&& other) noexcept(
        traits::template is_nothrow_constructible<UTypes const&&...>::value)
        : tuple(move(other), index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<
            conjunction<typename traits::template is_implicit_constructible<UTypes const&&...>,
                typename traits::template is_dangling<UTypes const&&...>>::value,
            int> = 2>
    constexpr tuple(tuple<UTypes...> const&&) noexcept(
        traits::template is_nothrow_constructible<UTypes const&&...>::value) = delete;

    template <typename... UTypes,
        enable_if_t<
            conjunction<typename traits::template is_explicit_constructible<UTypes const&&...>,
                typename traits::template is_dangling<UTypes const&&...>>::value,
            int> = 3>
    explicit constexpr tuple(tuple<UTypes...> const&&) noexcept(
        traits::template is_nothrow_constructible<UTypes const&&...>::value) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes,
        enable_if_t<disjunction<negation<
                        typename traits::template is_constructible<UTypes const&&...>>>::value,
            int> = 4>
    constexpr tuple(tuple<UTypes...> const&&) = delete;
#endif

public:
    template <typename TupleLike,
        enable_if_t<
            conjunction<not_this<TupleLike>, TT_SCOPE is_all_gettable<TupleLike, sizeof...(Types)>,
                TT_SCOPE rebind_references_t<traits::template is_implicit_constructible, TupleLike,
                    sizeof...(Types)>,
                negation<TT_SCOPE rebind_references_t<traits::template is_dangling, TupleLike,
                    sizeof...(Types)>>>::value,
            int> = 0>
    constexpr tuple(TupleLike&& p) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            TT_SCOPE rebind_references_t<traits::template is_nothrow_constructible, TupleLike,
                sizeof...(Types)>>::value)
        : tuple(forward<TupleLike>(p), index_sequence_for<Types...>{}) {}

    template <typename TupleLike,
        enable_if_t<
            conjunction<not_this<TupleLike>, TT_SCOPE is_all_gettable<TupleLike, sizeof...(Types)>,
                TT_SCOPE rebind_references_t<traits::template is_explicit_constructible, TupleLike,
                    sizeof...(Types)>,
                negation<TT_SCOPE rebind_references_t<traits::template is_dangling, TupleLike,
                    sizeof...(Types)>>>::value,
            int> = 1>
    explicit constexpr tuple(TupleLike&& p) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            TT_SCOPE rebind_references_t<traits::template is_nothrow_constructible, TupleLike,
                sizeof...(Types)>>::value)
        : tuple(forward<TupleLike>(p), index_sequence_for<Types...>{}) {}

    template <typename TupleLike,
        enable_if_t<
            conjunction<not_this<TupleLike>, TT_SCOPE is_all_gettable<TupleLike, sizeof...(Types)>,
                TT_SCOPE rebind_references_t<traits::template is_implicit_constructible, TupleLike,
                    sizeof...(Types)>,
                TT_SCOPE rebind_references_t<traits::template is_dangling, TupleLike,
                    sizeof...(Types)>>::value,
            int> = 2>
    constexpr tuple(TupleLike&& p) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            TT_SCOPE rebind_references_t<traits::template is_nothrow_constructible, TupleLike,
                sizeof...(Types)>>::value) = delete;

    template <typename TupleLike,
        enable_if_t<
            conjunction<not_this<TupleLike>, TT_SCOPE is_all_gettable<TupleLike, sizeof...(Types)>,
                TT_SCOPE rebind_references_t<traits::template is_explicit_constructible, TupleLike,
                    sizeof...(Types)>,
                TT_SCOPE rebind_references_t<traits::template is_dangling, TupleLike,
                    sizeof...(Types)>>::value,
            int> = 3>
    explicit constexpr tuple(TupleLike&& p) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            TT_SCOPE rebind_references_t<traits::template is_nothrow_constructible, TupleLike,
                sizeof...(Types)>>::value) = delete;

public:
    template <typename Alloc,
        enable_if_t<traits::template is_implicit_constructible_with_allocator<Alloc>::value, int> =
            0>
    constexpr tuple(allocator_arg_t, Alloc const& alloc) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc>::value)
        : base_type(allocator_arg, alloc) {}

    template <typename Alloc,
        enable_if_t<traits::template is_explicit_constructible_with_allocator<Alloc>::value, int> =
            1>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc>::value)
        : base_type(allocator_arg, alloc) {}

public:
    template <typename Alloc,
        enable_if_t<traits::template is_implicit_constructible_with_allocator<Alloc,
                        Types const&...>::value,
            int> = 0>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, Types const&... args) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, Types const&...>::value)
        : base_type(allocator_arg, alloc, args...) {}

    template <typename Alloc,
        enable_if_t<traits::template is_explicit_constructible_with_allocator<Alloc,
                        Types const&...>::value,
            int> = 1>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc, Types const&... args) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, Types const&...>::value)
        : base_type(allocator_arg, alloc, args...) {}

public:
    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible_with_allocator<Alloc,
                            UTypes...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes...>>>::value,
            int> = 0>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, UTypes&&... args) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes...>::value)
        : base_type(allocator_arg, alloc, forward<UTypes>(args)...) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes...>>>::value,
            int> = 1>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc, UTypes&&... args) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes...>::value)
        : base_type(allocator_arg, alloc, forward<UTypes>(args)...) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<
            conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                typename traits::template is_implicit_constructible_with_allocator<Alloc,
                    UTypes...>,
                typename traits::template is_dangling_without_allocator<Alloc, UTypes...>>::value,
            int> = 2>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, UTypes&&... args) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes...>::value) = delete;

    template <typename Alloc, typename... UTypes,
        enable_if_t<
            conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                typename traits::template is_explicit_constructible_with_allocator<Alloc,
                    UTypes...>,
                typename traits::template is_dangling_without_allocator<Alloc, UTypes...>>::value,
            int> = 3>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc, UTypes&&... args) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes...>::value) = delete;

public:
    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible_with_allocator<Alloc,
                            UTypes&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes&...>>>::value,
            int> = 0>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...>& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes&...>::value)
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes&...>>>::value,
            int> = 1>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...>& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes&...>::value)
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<
            conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                typename traits::template is_implicit_constructible_with_allocator<Alloc,
                    UTypes&...>,
                typename traits::template is_dangling_without_allocator<Alloc, UTypes&...>>::value,
            int> = 2>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...>& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes&...>::value) =
        delete;

    template <typename Alloc, typename... UTypes,
        enable_if_t<
            conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                typename traits::template is_explicit_constructible_with_allocator<Alloc,
                    UTypes&...>,
                typename traits::template is_dangling_without_allocator<Alloc, UTypes&...>>::value,
            int> = 3>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...>& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes&...>::value) =
        delete;

public:
    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible_with_allocator<Alloc,
                            UTypes const&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes const&...>>>::value,
            int> = 0>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...> const& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes const&...>::value)
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes const&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes const&...>>>::value,
            int> = 1>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...> const&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes const&...>::value)
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible_with_allocator<Alloc,
                            UTypes const&...>,
                        typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes const&...>>::value,
            int> = 2>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...> const& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes const&...>::value) =
        delete;

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes const&...>,
                        typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes const&...>>::value,
            int> = 3>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...> const&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes const&...>::value) = delete;

public:
    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible_with_allocator<Alloc,
                            UTypes&&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes&&...>>>::value,
            int> = 0>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...>&& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes&&...>::value)
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes&&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes&&...>>>::value,
            int> = 1>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...>&&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes&&...>::value)
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<
            conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                typename traits::template is_implicit_constructible_with_allocator<Alloc,
                    UTypes&&...>,
                typename traits::template is_dangling_without_allocator<Alloc, UTypes&&...>>::value,
            int> = 2>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...>&& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes&&...>::value) =
        delete;

    template <typename Alloc, typename... UTypes,
        enable_if_t<
            conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                typename traits::template is_explicit_constructible_with_allocator<Alloc,
                    UTypes&&...>,
                typename traits::template is_dangling_without_allocator<Alloc, UTypes&&...>>::value,
            int> = 3>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...>&&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes&&...>::value) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename Alloc, typename... UTypes,
        enable_if_t<disjunction<negation<typename traits::template is_constructible_with_allocator<
                        Alloc, UTypes&&...>>>::value,
            int> = 4>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...>&& other) = delete;
#endif

public:
    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible_with_allocator<Alloc,
                            UTypes const&&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes const&&...>>>::value,
            int> = 0>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...> const&& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes const&&...>::value)
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes const&&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes const&&...>>>::value,
            int> = 1>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...> const&&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes const&&...>::value)
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible_with_allocator<Alloc,
                            UTypes const&&...>,
                        typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes const&&...>>::value,
            int> = 2>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...> const&& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, UTypes const&&...>::value) =
        delete;

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes const&&...>,
                        typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes const&&...>>::value,
            int> = 3>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...> const&&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes const&&...>::value) = delete;

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename Alloc, typename... UTypes,
        enable_if_t<disjunction<negation<typename traits::template is_constructible_with_allocator<
                        Alloc, UTypes const&&...>>>::value,
            int> = 4>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple<UTypes...> const&& other) = delete;
#endif

public:
    template <typename Alloc, typename TupleLike,
        enable_if_t<
            conjunction<TT_SCOPE is_tuple_like<TupleLike>, not_this<TupleLike>,
                bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                TT_SCOPE rebind_references_t<
                    variadic_proxy<traits::template is_implicit_constructible_with_allocator,
                        Alloc>::template apply,
                    TupleLike>,
                negation<TT_SCOPE rebind_references_t<
                    variadic_proxy<traits::template is_dangling_without_allocator,
                        Alloc>::template apply,
                    TupleLike>>>::value,
            int> = 0>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            TT_SCOPE rebind_references_t<
                variadic_proxy<traits::template is_nothrow_constructible_with_allocator,
                    Alloc>::template apply,
                TupleLike, sizeof...(Types)>>::value)
        : tuple(allocator_arg, alloc, forward<TupleLike>(other), index_sequence_for<Types...>{}) {}

    template <typename Alloc, typename TupleLike,
        enable_if_t<
            conjunction<TT_SCOPE is_tuple_like<TupleLike>, not_this<TupleLike>,
                bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                TT_SCOPE rebind_references_t<
                    variadic_proxy<traits::template is_explicit_constructible_with_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>,
                negation<TT_SCOPE rebind_references_t<
                    variadic_proxy<traits::template is_dangling_without_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>>>::value,
            int> = 1>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            TT_SCOPE rebind_references_t<
                variadic_proxy<traits::template is_nothrow_constructible_with_allocator,
                    Alloc>::template apply,
                TupleLike, sizeof...(Types)>>::value)
        : tuple(allocator_arg, alloc, forward<TupleLike>(other), index_sequence_for<Types...>{}) {}

    template <typename Alloc, typename TupleLike,
        enable_if_t<
            conjunction<TT_SCOPE is_tuple_like<TupleLike>, not_this<TupleLike>,
                bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                TT_SCOPE rebind_references_t<
                    variadic_proxy<traits::template is_implicit_constructible_with_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>,
                TT_SCOPE rebind_references_t<
                    variadic_proxy<traits::template is_dangling_without_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>>::value,
            int> = 2>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            TT_SCOPE rebind_references_t<
                variadic_proxy<traits::template is_nothrow_constructible_with_allocator,
                    Alloc>::template apply,
                TupleLike, sizeof...(Types)>>::value) = delete;

    template <typename Alloc, typename TupleLike,
        enable_if_t<
            conjunction<TT_SCOPE is_tuple_like<TupleLike>, not_this<TupleLike>,
                bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                TT_SCOPE rebind_references_t<
                    variadic_proxy<traits::template is_explicit_constructible_with_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>,
                TT_SCOPE rebind_references_t<
                    variadic_proxy<traits::template is_dangling_without_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>>::value,
            int> = 3>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            TT_SCOPE rebind_references_t<
                variadic_proxy<traits::template is_nothrow_constructible_with_allocator,
                    Alloc>::template apply,
                TupleLike, sizeof...(Types)>>::value) = delete;

public:
    template <typename Alloc,
        typename = enable_if_t<
            traits::template is_constructible_with_allocator<Alloc, Types const&...>::value>>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple const& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, Types const&...>::value)
        : tuple(allocator_arg, alloc, other, index_sequence_for<Types...>{}) {}

    template <typename Alloc,
        typename =
            enable_if_t<traits::template is_constructible_with_allocator<Alloc, Types&&...>::value>>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple&& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, Types&&...>::value)
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<Types...>{}) {}

    template <typename Alloc,
        typename = enable_if_t<
            traits::template is_constructible_with_allocator<Alloc, Types const&&...>::value>>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, tuple const&& other) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc, Types const&&...>::value)
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<Types...>{}) {}

public:
    template <typename... UTypes,
        typename = enable_if_t<
            conjunction<typename traits::template is_assignable<UTypes const&...>>::value>>
    UTL_CONSTEXPR_CXX14 tuple& operator=(tuple<UTypes...> const& other) noexcept(
        traits::template is_nothrow_assignable<UTypes const&...>::value) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <typename... UTypes,
        typename = enable_if_t<
            conjunction<typename traits::template is_const_assignable<UTypes const&...>>::value>>
    constexpr tuple const& operator=(tuple<UTypes...> const& other) const
        noexcept(traits::template is_nothrow_const_assignable<UTypes const&...>::value) {
        return assign(other, index_sequence_for<Types...>{});
    }

public:
    template <typename... UTypes,
        enable_if_t<conjunction<typename traits::template is_assignable<UTypes&&...>>::value, int> =
            0>
    UTL_CONSTEXPR_CXX14 tuple& operator=(tuple<UTypes...>&& other) noexcept(
        traits::template is_nothrow_assignable<UTypes&&...>::value) {
        return assign(move(other), index_sequence_for<Types...>{});
    }

    template <typename... UTypes,
        enable_if_t<
            disjunction<negation<typename traits::template is_assignable<UTypes&&...>>>::value,
            int> = 1>
    UTL_CONSTEXPR_CXX14 tuple& operator=(tuple<UTypes...>&& other) = delete;

    template <typename... UTypes,
        enable_if_t<conjunction<typename traits::template is_const_assignable<UTypes&&...>>::value,
            int> = 0>
    constexpr tuple const& operator=(tuple<UTypes...>&& other) const
        noexcept(traits::template is_nothrow_const_assignable<UTypes&&...>::value) {
        return assign(move(other), index_sequence_for<Types...>{});
    }

#if UTL_ENFORCE_NONMOVABILIITY
    /**
     * Non-standard overload
     */
    template <typename... UTypes,
        enable_if_t<disjunction<negation<
                        typename traits::template is_const_assignable<UTypes&&...>>>::value,
            int> = 1>
    constexpr tuple const& operator=(tuple<UTypes...>&& other) const = delete;
#endif

public:
    template <typename TupleLike,
        enable_if_t<conjunction<TT_SCOPE is_tuple_like<TupleLike>, not_this<TupleLike>,
                        bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                        TT_SCOPE rebind_references_t<traits::template is_assignable, TupleLike,
                            sizeof...(Types)>>::value,
            int> = 0>
    UTL_CONSTEXPR_CXX14 tuple& operator=(TupleLike&& other) noexcept(
        conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            TT_SCOPE rebind_references_t<traits::template is_nothrow_assignable, TupleLike,
                sizeof...(Types)>>::value) {
        return assign(forward<TupleLike>(other), index_sequence_for<Types...>{});
    }

    template <typename TupleLike,
        enable_if_t<conjunction<TT_SCOPE is_tuple_like<TupleLike>, not_this<TupleLike>,
                        bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                        TT_SCOPE rebind_references_t<traits::template is_const_assignable,
                            TupleLike, sizeof...(Types)>>::value,
            int> = 1>
    constexpr tuple const& operator=(TupleLike&& other) const
        noexcept(conjunction<TT_SCOPE is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            TT_SCOPE rebind_references_t<traits::template is_nothrow_const_assignable, TupleLike,
                sizeof...(Types)>>::value) {
        return assign(forward<TupleLike>(other), index_sequence_for<Types...>{});
    }
};

template <size_t I, typename T>
struct tuple_element_offset;

template <size_t I, typename... Ts>
struct tuple_element_offset<I, tuple<Ts...>> :
    enable_if_t<is_base_of<details::tuple::storage<Ts...>, tuple<Ts...>>::value,
        details::tuple::offset_impl<I, details::tuple::storage<Ts...>>> {};

namespace details {
namespace tuple {
template <size_t I, typename T, typename U>
UTL_ATTRIBUTE(CONST) constexpr enable_if_t<(I == tuple_size<T>::value), bool> less(
    T const& l, U const& r) noexcept {
    return false;
}

template <size_t I, typename T, typename U>
constexpr enable_if_t<(I < tuple_size<T>::value), bool> less(T const& l, U const& r) noexcept(
    conjunction<compare_ops::all_have_nothrow_eq<T, U>,
        compare_ops::all_have_nothrow_lt<T, U>>::value) {
    return (UTL_TUPLE_GET(I - 1, l) == UTL_TUPLE_GET(I - 1, r)) &&
        ((UTL_TUPLE_GET(I, l) < UTL_TUPLE_GET(I, r)) || less<I + 1>(l, r));
}

template <typename T, typename U>
UTL_ATTRIBUTE(FLATTEN) constexpr bool less(T const& l, U const& r) noexcept(
    conjunction<compare_ops::all_have_nothrow_eq<T, U>,
        compare_ops::all_have_nothrow_lt<T, U>>::value) {
    static_assert(compare_ops::all_have_eq<T, U>::value, "All elements must be comparable");
    static_assert(compare_ops::all_have_lt<T, U>::value, "All elements must be comparable");
    return (UTL_TUPLE_GET(0, l) < UTL_TUPLE_GET(0, r)) || less<1>(l, r);
}

template <size_t I, typename T, typename U>
UTL_ATTRIBUTE(CONST) constexpr enable_if_t<(I == tuple_size<T>::value), bool> equals(
    T const& l, U const& r) noexcept {
    return true;
}

template <size_t I, typename T, typename U>
constexpr enable_if_t<(I < tuple_size<T>::value), bool> equals(T const& l, U const& r) noexcept(
    compare_ops::all_have_nothrow_eq<T, U>::value) {
    return (UTL_TUPLE_GET(I, l) == UTL_TUPLE_GET(I, r)) && equals<I + 1>(l, r);
}

template <typename T, typename U>
UTL_ATTRIBUTES(NODISCARD, FLATTEN) constexpr bool equals(T const& l, U const& r) noexcept(
    compare_ops::all_have_nothrow_eq<T, U>::value) {
    static_assert(compare_ops::all_have_eq<T, U>::value, "All elements must be comparable");
    return equals<0>(l, r);
}

} // namespace tuple
} // namespace details

template <typename... Ts, typename... Us>
UTL_ATTRIBUTE(NODISCARD) constexpr enable_if_t<
    conjunction<compare_ops::all_have_eq<tuple<Ts...>, tuple<Us...>>>::value, bool>
operator==(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(details::tuple::equals(l, r)) {
    return details::tuple::equals(l, r);
}

template <typename... Ts, typename... Us>
UTL_ATTRIBUTE(NODISCARD) constexpr enable_if_t<
    conjunction<compare_ops::all_have_eq<tuple<Ts...>, tuple<Us...>>,
        compare_ops::all_have_lt<tuple<Ts...>, tuple<Us...>>>::value,
    bool>
operator<(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(details::tuple::less(l, r)) {
    return details::tuple::less(l, r);
}

UTL_ATTRIBUTES(NODISCARD, CONST) constexpr bool operator==(tuple<> const& l, tuple<> const& r) noexcept {
    return true;
}

UTL_ATTRIBUTES(NODISCARD, CONST) constexpr bool operator<(tuple<> const& l, tuple<> const& r) noexcept {
    return false;
}

template <typename... Ts, typename... Us>
UTL_ATTRIBUTE(NODISCARD) constexpr enable_if_t<
    UTL_TRAIT_is_equality_comparable_with(tuple<Ts...>, tuple<Us...>), bool>
operator!=(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(!(l == r))) {
    return !(l == r);
}

template <typename... Ts, typename... Us>
UTL_ATTRIBUTE(NODISCARD) constexpr enable_if_t<
    UTL_TRAIT_is_strict_subordinate_comparable_with(tuple<Us...>, tuple<Ts...>), bool>
operator<=(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(!(r < l))) {
    return !(r < l);
}

template <typename... Ts, typename... Us>
UTL_ATTRIBUTE(NODISCARD) constexpr enable_if_t<
    UTL_TRAIT_is_strict_subordinate_comparable_with(tuple<Us...>, tuple<Ts...>), bool>
operator>(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(
    noexcept(static_cast<bool>(r < l))) {
    return static_cast<bool>(r < l);
}

template <typename... Ts, typename... Us>
UTL_ATTRIBUTE(NODISCARD) constexpr enable_if_t<
    UTL_TRAIT_is_strict_subordinate_comparable_with(tuple<Ts...>, tuple<Us...>), bool>
operator>=(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(!(l < r))) {
    return !(l < r);
}

template <typename... Ts>
constexpr tuple<unwrap_reference_t<decay_t<Ts>>...> make_tuple(Ts&&... ts) noexcept(
    is_nothrow_constructible<tuple<unwrap_reference_t<decay_t<Ts>>...>, Ts...>::value) {
    return tuple<unwrap_reference_t<decay_t<Ts>>...>{forward<Ts>(ts)...};
}

template <typename... Args>
UTL_ATTRIBUTE(NODISCARD) constexpr tuple<Args&...> tie(Args&... args UTL_LIFETIMEBOUND) noexcept {
    return {args...};
}

template <typename... Args>
UTL_ATTRIBUTE(NODISCARD) constexpr tuple<Args&&...> forward_as_tuple(
    Args&&... args UTL_LIFETIMEBOUND) noexcept {
    return {forward<Args>(args)...};
}

UTL_NAMESPACE_END

#undef TT_SCOPE
