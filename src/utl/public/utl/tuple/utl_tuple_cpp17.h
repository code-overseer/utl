// Copyright 2023-2024 Bryan Wong

#pragma once

/**
 * Standard Tuple implementation up to and including C++17
 */

#if !defined(UTL_TUPLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#include "utl/tuple/utl_tuple_fwd.h"

#include "utl/memory/utl_uses_allocator.h"
#include "utl/ranges/utl_swap.h"
#include "utl/tuple/utl_tuple_compare_traits.h"
#include "utl/tuple/utl_tuple_details.h"
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
struct common_type_impl<__UTL tuple<Ts...>, __UTL tuple<Us...>, void_t<common_type_t<Ts, Us>...>> {
    using type UTL_NODEBUG = __UTL tuple<common_type_t<Ts, Us>...>;
};

template <typename, typename, template <typename> class TQual, template <typename> class UQual,
    typename = void>
struct basic_ref_impl {};

template <typename... Ts, typename... Us, template <typename> class TQual,
    template <typename> class UQual>
struct basic_ref_impl<__UTL tuple<Ts...>, __UTL tuple<Us...>, TQual, UQual,
    void_t<common_reference_t<TQual<Ts>, UQual<Us>>...>> {
    using type UTL_NODEBUG = __UTL tuple<common_reference_t<TQual<Ts>, UQual<Us>>...>;
};
} // namespace tuple
} // namespace details

UTL_NAMESPACE_END

// TODO: if std is included or forward declared use std, else use UTL
namespace std {
template <typename... Ts, typename... Us>
struct common_type<__UTL tuple<Ts...>, __UTL tuple<Us...>> :
    __UTL details::tuple::common_type_impl<__UTL tuple<Ts...>, __UTL tuple<Us...>> {};
template <typename... Ts, typename... Us, template <typename> class TQual,
    template <typename> class UQual>
struct basic_common_reference<__UTL tuple<Ts...>, __UTL tuple<Us...>, TQual, UQual> :
    __UTL details::tuple::basic_ref_impl<__UTL tuple<Ts...>, __UTL tuple<Us...>, TQual, UQual> {};
} // namespace std

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {

template <typename T, typename... Tail>
struct storage<T, Tail...> {
    using traits UTL_NODEBUG = variadic_traits<T, Tail...>;
    using head_type UTL_NODEBUG = T;
    using tail_type UTL_NODEBUG = storage<Tail...>;
    using move_assign_t UTL_NODEBUG = conditional_t<traits::is_move_assignable, invalid_t, storage>;
    using move_construct_t UTL_NODEBUG =
        conditional_t<traits::is_move_constructible, invalid_t, storage>;

    __UTL_HIDE_FROM_ABI constexpr storage() noexcept(
        traits::is_nothrow_default_constructible) = default;
    __UTL_HIDE_FROM_ABI constexpr storage(storage const&) noexcept(
        traits::is_nothrow_copy_constructible) = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 storage& operator=(storage const&) noexcept(
        traits::is_nothrow_copy_assignable) = default;

#if UTL_ENFORCE_NONMOVABILIITY
    __UTL_HIDE_FROM_ABI constexpr storage(move_construct_t&&) noexcept(
        traits::is_nothrow_move_constructible) = delete;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 storage& operator=(move_assign_t&&) noexcept(
        traits::is_nothrow_move_assignable) = delete;
#else
    __UTL_HIDE_FROM_ABI constexpr storage(storage&&) noexcept(
        traits::is_nothrow_move_constructible) = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 storage& operator=(storage&&) noexcept(
        traits::is_nothrow_move_assignable) = default;
#endif

    template <typename UHead, typename... UTail,
        enable_if_t<sizeof...(UTail) == sizeof...(Tail), int> = 1>
    __UTL_HIDE_FROM_ABI constexpr storage(UHead&& other_head, UTail&&... other_tail) noexcept(
        traits::template is_nothrow_constructible<UHead, UTail...>::value)
        : head(__UTL forward<UHead>(other_head))
        , tail(__UTL forward<UTail>(other_tail)...) {}

    template <typename Alloc,
        enable_if_t<conjunction<__UTL uses_allocator<T, Alloc>,
                        __UTL is_constructible<T, allocator_arg_t, Alloc const&>>::value,
            int> = 0>
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        conjunction<__UTL is_nothrow_constructible<T, allocator_arg_t, Alloc const&>,
            __UTL is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&>>::value)
        : head(allocator_arg, alloc)
        , tail(allocator_arg, alloc) {}

    template <typename Alloc,
        enable_if_t<conjunction<__UTL uses_allocator<T, Alloc>,
                        negation<__UTL is_constructible<T, allocator_arg_t, Alloc const&>>,
                        __UTL is_constructible<T, Alloc const&>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        conjunction<__UTL is_nothrow_constructible<T, Alloc const&>,
            __UTL is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&>>::value)
        : head(alloc)
        , tail(allocator_arg, alloc) {}

    template <typename Alloc, enable_if_t<!__UTL uses_allocator<T, Alloc>::value, int> = 2>
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        conjunction<__UTL is_nothrow_constructible<T>,
            __UTL is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&>>::value)
        : head()
        , tail(allocator_arg, alloc) {}

    template <typename Alloc, typename UHead, typename... UTail,
        enable_if_t<conjunction<__UTL uses_allocator<T, Alloc>,
                        __UTL is_constructible<T, allocator_arg_t, Alloc const&, UHead>>::value,
            int> = 0>
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(conjunction<__UTL is_nothrow_constructible<T,
                                                        allocator_arg_t, Alloc const&, UHead>,
        __UTL is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&, UTail...>>::value)
        : head(allocator_arg, alloc, __UTL forward<UHead>(other_head))
        , tail(allocator_arg, alloc, __UTL forward<UTail>(other_tail)...) {}

    template <typename Alloc, typename UHead, typename... UTail,
        enable_if_t<conjunction<__UTL uses_allocator<T, Alloc>,
                        negation<__UTL is_constructible<T, allocator_arg_t, Alloc const&>>,
                        __UTL is_constructible<T, UHead, Alloc const&>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(conjunction<__UTL is_nothrow_constructible<T, UHead,
                                                        Alloc const&>,
        __UTL is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&, UTail...>>::value)
        : head(__UTL forward<UHead>(other_head), alloc)
        , tail(allocator_arg, alloc, __UTL forward<UTail>(other_tail)...) {}

    template <typename Alloc, typename UHead, typename... UTail,
        enable_if_t<!__UTL uses_allocator<T, Alloc>::value, int> = 2>
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(conjunction<__UTL is_nothrow_constructible<T>,
        __UTL is_nothrow_constructible<tail_type, allocator_arg_t, Alloc const&, UTail...>>::value)
        : head(__UTL forward<UHead>(other_head))
        , tail(allocator_arg, alloc, __UTL forward<UTail>(other_tail)...) {}

    template <typename... Us>
    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 void swap(storage<Us&...>& other) noexcept(
        traits::template is_nothrow_swappable_with<Us&...>::value) {
        __UTL ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename... Us>
    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 void swap(storage<Us...> const& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<Us const&...>::value) {
        __UTL ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename... Us>
    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 void swap(storage<Us...>& other) const
        noexcept(traits::template is_nothrow_const_swappable_with<Us&...>::value) {
        __UTL ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename UHead, typename... UTail>
    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 storage&
    assign(UHead&& other_head, UTail&&... other_tail) noexcept(
        traits::template is_nothrow_assignable<UHead, UTail...>::value) {
        head = __UTL forward<UHead>(other_head);
        tail.assign(__UTL forward<UTail>(other_tail)...);
        return *this;
    }

    template <typename UHead, typename... UTail>
    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) inline constexpr storage const& assign(
        UHead&& other_head, UTail&&... other_tail) const
        noexcept(traits::template is_nothrow_const_assignable<UHead, UTail...>::value) {
        head = __UTL forward<UHead>(other_head);
        tail.assign(__UTL forward<UTail>(other_tail)...);
        return *this;
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI)
    UTL_CONSTEXPR_CXX14 auto get() && noexcept UTL_LIFETIMEBOUND
    -> enable_if_t<!I, T&&> {
        return __UTL move(head);
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI)
    UTL_CONSTEXPR_CXX14 auto get() & noexcept UTL_LIFETIMEBOUND
    -> enable_if_t<!I, T&> {
        return head;
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get() const&& noexcept UTL_LIFETIMEBOUND -> enable_if_t<!I, T const&&> {
        return __UTL move(head);
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get() const& noexcept UTL_LIFETIMEBOUND -> enable_if_t<!I, T const&> {
        return head;
    }

    template <size_t I>
    using result_type_t UTL_NODEBUG =
        enable_if_t<(I > 0 && I < traits::size), template_element_t<I, storage>>;

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 auto get() && noexcept UTL_LIFETIMEBOUND -> result_type_t<I>&& {
        return __UTL move(tail).template get<I - 1>();
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 auto get() & noexcept UTL_LIFETIMEBOUND -> result_type_t<I>& {
        return tail.template get<I - 1>();
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) inline constexpr auto get() const&& noexcept UTL_LIFETIMEBOUND -> result_type_t<I> const&& {
        return __UTL move(tail).template get<I - 1>();
    }

    template <size_t I>
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) inline constexpr auto get() const& noexcept UTL_LIFETIMEBOUND -> result_type_t<I> const& {
        return tail.template get<I - 1>();
    }

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) head_type head;
    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) tail_type tail;
};

} // namespace tuple
} // namespace details

template <typename... Types>
class UTL_ATTRIBUTES(
    _PUBLIC_TEMPLATE, EMPTY_BASES) tuple : private details::tuple::storage<Types...> {
private:
    template <size_t I, typename T>
    friend struct tuple_element_offset;

    using base_type UTL_NODEBUG = details::tuple::storage<Types...>;
    using typename base_type::traits;
    using swap_type UTL_NODEBUG =
        conditional_t<traits::is_swappable, tuple, details::tuple::invalid_swap_t<Types...>>;
    using const_swap_type UTL_NODEBUG = conditional_t<traits::is_const_swappable, tuple,
        details::tuple::invalid_swap_t<Types...>> const;
    using move_assign_t UTL_NODEBUG =
        conditional_t<traits::is_move_assignable, details::tuple::invalid_t, tuple>;
    using move_construct_t UTL_NODEBUG =
        conditional_t<traits::is_move_constructible, details::tuple::invalid_t, tuple>;
    template <typename T>
    using not_this UTL_NODEBUG = negation<is_same<T, tuple>>;
    using base_type::get;

    template <typename TupleLike, size_t... Is>
    __UTL_HIDE_FROM_ABI constexpr tuple(TupleLike&& other, index_sequence<Is...>) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Is)>,
            typename traits::template is_nothrow_constructible<
                details::tuple::get_type_t<Is, TupleLike>...>>::value)
        : base_type(__UTL get_element<Is>(__UTL forward<TupleLike>(other))...) {}

    template <typename Alloc, typename TupleLike, size_t... Is>
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other,
        index_sequence<
            Is...>) noexcept(conjunction<details::tuple::is_all_nothrow_gettable<TupleLike,
                                             sizeof...(Is)>,
        typename traits::template is_nothrow_constructible_with_allocator<Alloc,
            details::tuple::get_type_t<Is, TupleLike>...>>::value)
        : base_type(
              allocator_arg, alloc, __UTL get_element<Is>(__UTL forward<TupleLike>(other))...) {}

    template <typename TupleLike, size_t... Is>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 tuple& assign(TupleLike&& other, index_sequence<Is...>) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Is)>,
            typename traits::template is_nothrow_assignable<
                details::tuple::get_type_t<Is, TupleLike>...>>::value) {
        return (tuple&)base_type::assign(__UTL get_element<Is>(__UTL forward<TupleLike>(other))...);
    }

    template <typename TupleLike, size_t... Is>
    __UTL_HIDE_FROM_ABI constexpr tuple const& assign(
        TupleLike&& other, index_sequence<Is...>) const
        noexcept(conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Is)>,
            typename traits::template is_nothrow_const_assignable<
                details::tuple::get_type_t<Is, TupleLike>...>>::value) {
        return (tuple const&)base_type::assign(
            __UTL get_element<Is>(__UTL forward<TupleLike>(other))...);
    }

public:
    __UTL_HIDE_FROM_ABI constexpr tuple(tuple const&) noexcept(
        traits::is_nothrow_copy_constructible) = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 tuple& operator=(tuple const& other) noexcept(
        traits::is_nothrow_copy_assignable) = default;
    __UTL_HIDE_FROM_ABI constexpr tuple(tuple&&) noexcept(
        traits::is_nothrow_move_constructible) = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 tuple& operator=(tuple&&) noexcept(
        traits::is_nothrow_move_assignable) = default;

    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 ~tuple() = default;

    template <bool NotEmpty = (sizeof...(Types) >= 1),
        typename = enable_if_t<NotEmpty && traits::is_const_copy_assignable>>
    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple const& other) const
        noexcept(traits::is_nothrow_const_copy_assignable) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <bool NotEmpty = (sizeof...(Types) >= 1),
        typename = enable_if_t<NotEmpty && traits::is_const_move_assignable>>
    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple&& other) const
        noexcept(traits::is_nothrow_const_move_assignable) {
        return assign(move(other), index_sequence_for<Types...>{});
    }

public:
    template <typename... Us>
    __UTL_HIDE_FROM_ABI
        UTL_CONSTEXPR_CXX14 enable_if_t<conjunction<typename traits::template matches<Us...>,
        typename traits::template is_swappable_with<Us&...>>::value>
    swap(tuple<Us...>& other) noexcept(traits::template is_nothrow_swappable_with<Us&...>::value) {
        static_assert(UTL_TRAIT_is_base_of(details::tuple::storage<Us...>, tuple<Us...>),
            "tuple must inherit from storage");
        base_type::swap((details::tuple::storage<Us...>&)other);
    }

    template <typename... Us>
    __UTL_HIDE_FROM_ABI
        UTL_CONSTEXPR_CXX14 enable_if_t<conjunction<typename traits::template matches<Us...>,
        typename traits::template is_swappable_with<Us const&...>>::value>
    swap(tuple<Us...> const& other) noexcept(
        traits::template is_nothrow_swappable_with<Us const&...>::value) {
        static_assert(UTL_TRAIT_is_base_of(details::tuple::storage<Us...>, tuple<Us...>),
            "tuple must inherit from storage");
        base_type::swap((details::tuple::storage<Us...> const&)other);
    }

    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) friend inline UTL_CONSTEXPR_CXX14 void swap(
        swap_type& l, swap_type& r) noexcept(traits::is_nothrow_swappable) {
        l.swap(r);
    }

    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) friend inline UTL_CONSTEXPR_CXX14 void swap(
        const_swap_type& l, const_swap_type& r) noexcept(traits::is_nothrow_const_swappable) {
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
    explicit(UTL_TRAIT_disjunction(is_explicit_constructible<Types>...)) constexpr tuple() noexcept(
        traits::is_nothrow_default_constructible) = default;
    UTL_DISABLE_WARNING_POP()
#else
    template <bool NotEmpty = (sizeof...(Types) >= 1),
        enable_if_t<NotEmpty && traits::is_implicit_default_constructible, int> = 0>
    __UTL_HIDE_FROM_ABI constexpr tuple() noexcept(traits::is_nothrow_default_constructible)
        : base_type() {}

    template <bool NotEmpty = (sizeof...(Types) >= 1),
        enable_if_t<NotEmpty && traits::is_explicit_default_constructible, int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple() noexcept(
        traits::is_nothrow_default_constructible)
        : base_type() {}
#endif

public:
    template <bool NotEmpty = (sizeof...(Types) >= 1),
        enable_if_t<
            conjunction<bool_constant<NotEmpty>,
                typename traits::template is_implicit_constructible<Types const&...>>::value,
            int> = 0>
    __UTL_HIDE_FROM_ABI constexpr tuple(Types const&... args) noexcept(
        traits::is_nothrow_copy_constructible)
        : base_type(args...) {}

    template <bool NotEmpty = (sizeof...(Types) >= 1),
        enable_if_t<
            conjunction<bool_constant<NotEmpty>,
                typename traits::template is_explicit_constructible<Types const&...>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(Types const&... args) noexcept(
        traits::is_nothrow_copy_constructible)
        : base_type(args...) {}

public:
    template <typename UHead, typename... UTail,
        enable_if_t<
            conjunction<bool_constant<sizeof...(Types) == sizeof...(UTail) + 1>,
                typename traits::template is_implicit_constructible<UHead, UTail...>,
                negation<typename traits::template is_dangling<UHead&&, UTail&&...>>>::value,
            int> = 0>
    __UTL_HIDE_FROM_ABI constexpr tuple(UHead&& head, UTail&&... tail) noexcept(
        conjunction<bool_constant<sizeof...(Types) == sizeof...(UTail) + 1>,
            typename traits::template is_nothrow_constructible<UHead, UTail...>>::value)
        : base_type(__UTL forward<UHead>(head), __UTL forward<UTail>(tail)...) {}

    template <typename UHead, typename... UTail,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTail) + 1>,
                        typename traits::template is_explicit_constructible<UHead, UTail...>,
                        // negation<typename traits::template is_dangling<UHead&&, UTail&&...>>
                        true_type>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(UHead&& head, UTail&&... tail) noexcept(
        traits::template is_nothrow_constructible<UHead, UTail...>::value)
        : base_type(__UTL forward<UHead>(head), __UTL forward<UTail>(tail)...) {}

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
    __UTL_HIDE_FROM_ABI constexpr tuple(tuple<UTypes...>& other) noexcept(
        traits::template is_nothrow_constructible<UTypes&...>::value)
        : tuple(other, index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible<UTypes&...>,
                        negation<typename traits::template is_dangling<UTypes&...>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(tuple<UTypes...>& other) noexcept(
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
    __UTL_HIDE_FROM_ABI constexpr tuple(tuple<UTypes...> const& other) noexcept(
        traits::template is_nothrow_constructible<UTypes const&...>::value)
        : tuple(other, index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible<UTypes const&...>,
                        negation<typename traits::template is_dangling<UTypes const&...>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(tuple<UTypes...> const& other) noexcept(
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
    __UTL_HIDE_FROM_ABI constexpr tuple(tuple<UTypes...>&& other) noexcept(
        traits::template is_nothrow_constructible<UTypes&&...>::value)
        : tuple(move(other), index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<conjunction<typename traits::template is_explicit_constructible<UTypes&&...>,
                        negation<typename traits::template is_dangling<UTypes&&...>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(tuple<UTypes...>&& other) noexcept(
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
    __UTL_HIDE_FROM_ABI constexpr tuple(tuple<UTypes...> const&& other) noexcept(
        traits::template is_nothrow_constructible<UTypes const&&...>::value)
        : tuple(move(other), index_sequence_for<UTypes...>{}) {}

    template <typename... UTypes,
        enable_if_t<
            conjunction<typename traits::template is_explicit_constructible<UTypes const&&...>,
                negation<typename traits::template is_dangling<UTypes const&&...>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(tuple<UTypes...> const&& other) noexcept(
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
            conjunction<not_this<TupleLike>, is_tuple_like<remove_cvref_t<TupleLike>>,
                details::tuple::rebind_references_t<traits::template is_implicit_constructible,
                    TupleLike, sizeof...(Types)>,
                negation<details::tuple::rebind_references_t<traits::template is_dangling,
                    TupleLike, sizeof...(Types)>>>::value,
            int> = 0>
    __UTL_HIDE_FROM_ABI constexpr tuple(TupleLike&& p) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            details::tuple::rebind_references_t<traits::template is_nothrow_constructible,
                TupleLike, sizeof...(Types)>>::value)
        : tuple(__UTL forward<TupleLike>(p), index_sequence_for<Types...>{}) {}

    template <typename TupleLike,
        enable_if_t<
            conjunction<not_this<TupleLike>, is_tuple_like<remove_cvref_t<TupleLike>>,
                details::tuple::rebind_references_t<traits::template is_explicit_constructible,
                    TupleLike, sizeof...(Types)>,
                negation<details::tuple::rebind_references_t<traits::template is_dangling,
                    TupleLike, sizeof...(Types)>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(TupleLike&& p) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            details::tuple::rebind_references_t<traits::template is_nothrow_constructible,
                TupleLike, sizeof...(Types)>>::value)
        : tuple(__UTL forward<TupleLike>(p), index_sequence_for<Types...>{}) {}

    template <typename TupleLike,
        enable_if_t<
            conjunction<not_this<TupleLike>, is_tuple_like<remove_cvref_t<TupleLike>>,
                details::tuple::rebind_references_t<traits::template is_implicit_constructible,
                    TupleLike, sizeof...(Types)>,
                details::tuple::rebind_references_t<traits::template is_dangling, TupleLike,
                    sizeof...(Types)>>::value,
            int> = 2>
    constexpr tuple(TupleLike&& p) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            details::tuple::rebind_references_t<traits::template is_nothrow_constructible,
                TupleLike, sizeof...(Types)>>::value) = delete;

    template <typename TupleLike,
        enable_if_t<
            conjunction<not_this<TupleLike>, is_tuple_like<remove_cvref_t<TupleLike>>,
                details::tuple::rebind_references_t<traits::template is_explicit_constructible,
                    TupleLike, sizeof...(Types)>,
                details::tuple::rebind_references_t<traits::template is_dangling, TupleLike,
                    sizeof...(Types)>>::value,
            int> = 3>
    explicit constexpr tuple(TupleLike&& p) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            details::tuple::rebind_references_t<traits::template is_nothrow_constructible,
                TupleLike, sizeof...(Types)>>::value) = delete;

public:
    template <typename Alloc,
        enable_if_t<traits::template is_implicit_constructible_with_allocator<Alloc>::value, int> =
            0>
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc>::value)
        : base_type(allocator_arg, alloc) {}

    template <typename Alloc,
        enable_if_t<traits::template is_explicit_constructible_with_allocator<Alloc>::value, int> =
            1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(allocator_arg_t, Alloc const& alloc) noexcept(
        traits::template is_nothrow_constructible_with_allocator<Alloc>::value)
        : base_type(allocator_arg, alloc) {}

public:
    template <typename Alloc,
        enable_if_t<traits::template is_implicit_constructible_with_allocator<Alloc,
                        Types const&...>::value,
            int> = 0>
    __UTL_HIDE_FROM_ABI constexpr tuple(
        allocator_arg_t, Alloc const& alloc, Types const&... args) noexcept(traits::
            template is_nothrow_constructible_with_allocator<Alloc, Types const&...>::value)
        : base_type(allocator_arg, alloc, args...) {}

    template <typename Alloc,
        enable_if_t<traits::template is_explicit_constructible_with_allocator<Alloc,
                        Types const&...>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(
        allocator_arg_t, Alloc const& alloc, Types const&... args) noexcept(traits::
            template is_nothrow_constructible_with_allocator<Alloc, Types const&...>::value)
        : base_type(allocator_arg, alloc, args...) {}

public:
    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_implicit_constructible_with_allocator<Alloc,
                            UTypes...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes...>>>::value,
            int> = 0>
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc,
        UTypes&&... args) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes...>::value)
        : base_type(allocator_arg, alloc, __UTL forward<UTypes>(args)...) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes...>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
        UTypes&&... args) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes...>::value)
        : base_type(allocator_arg, alloc, __UTL forward<UTypes>(args)...) {}

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
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...>&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes&...>::value)
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes&...>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...>&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes&...>::value)
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
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...> const&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes const&...>::value)
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes const&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes const&...>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
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
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...>&&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes&&...>::value)
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes&&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes&&...>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
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
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...> const&&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        UTypes const&&...>::value)
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<UTypes...>{}) {}

    template <typename Alloc, typename... UTypes,
        enable_if_t<conjunction<bool_constant<sizeof...(Types) == sizeof...(UTypes)>,
                        typename traits::template is_explicit_constructible_with_allocator<Alloc,
                            UTypes const&&...>,
                        negation<typename traits::template is_dangling_without_allocator<Alloc,
                            UTypes const&&...>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
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
            conjunction<is_tuple_like<TupleLike>, not_this<TupleLike>,
                bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                details::tuple::rebind_references_t<
                    variadic_proxy<traits::template is_implicit_constructible_with_allocator,
                        Alloc>::template apply,
                    TupleLike>,
                negation<details::tuple::rebind_references_t<
                    variadic_proxy<traits::template is_dangling_without_allocator,
                        Alloc>::template apply,
                    TupleLike>>>::value,
            int> = 0>
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc,
        TupleLike&& other) noexcept(conjunction<details::tuple::is_all_nothrow_gettable<TupleLike,
                                                    sizeof...(Types)>,
        details::tuple::rebind_references_t<
            variadic_proxy<traits::template is_nothrow_constructible_with_allocator,
                Alloc>::template apply,
            TupleLike, sizeof...(Types)>>::value)
        : tuple(allocator_arg, alloc, forward<TupleLike>(other), index_sequence_for<Types...>{}) {}

    template <typename Alloc, typename TupleLike,
        enable_if_t<
            conjunction<is_tuple_like<TupleLike>, not_this<TupleLike>,
                bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                details::tuple::rebind_references_t<
                    variadic_proxy<traits::template is_explicit_constructible_with_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>,
                negation<details::tuple::rebind_references_t<
                    variadic_proxy<traits::template is_dangling_without_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit constexpr tuple(allocator_arg_t, Alloc const& alloc,
        TupleLike&& other) noexcept(conjunction<details::tuple::is_all_nothrow_gettable<TupleLike,
                                                    sizeof...(Types)>,
        details::tuple::rebind_references_t<
            variadic_proxy<traits::template is_nothrow_constructible_with_allocator,
                Alloc>::template apply,
            TupleLike, sizeof...(Types)>>::value)
        : tuple(allocator_arg, alloc, forward<TupleLike>(other), index_sequence_for<Types...>{}) {}

    template <typename Alloc, typename TupleLike,
        enable_if_t<
            conjunction<is_tuple_like<TupleLike>, not_this<TupleLike>,
                bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                details::tuple::rebind_references_t<
                    variadic_proxy<traits::template is_implicit_constructible_with_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>,
                details::tuple::rebind_references_t<
                    variadic_proxy<traits::template is_dangling_without_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>>::value,
            int> = 2>
    constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            details::tuple::rebind_references_t<
                variadic_proxy<traits::template is_nothrow_constructible_with_allocator,
                    Alloc>::template apply,
                TupleLike, sizeof...(Types)>>::value) = delete;

    template <typename Alloc, typename TupleLike,
        enable_if_t<
            conjunction<is_tuple_like<TupleLike>, not_this<TupleLike>,
                bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                details::tuple::rebind_references_t<
                    variadic_proxy<traits::template is_explicit_constructible_with_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>,
                details::tuple::rebind_references_t<
                    variadic_proxy<traits::template is_dangling_without_allocator,
                        Alloc>::template apply,
                    TupleLike, sizeof...(Types)>>::value,
            int> = 3>
    explicit constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            details::tuple::rebind_references_t<
                variadic_proxy<traits::template is_nothrow_constructible_with_allocator,
                    Alloc>::template apply,
                TupleLike, sizeof...(Types)>>::value) = delete;

public:
    template <typename Alloc,
        typename = enable_if_t<
            traits::template is_constructible_with_allocator<Alloc, Types const&...>::value>>
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple const& other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        Types const&...>::value)
        : tuple(allocator_arg, alloc, other, index_sequence_for<Types...>{}) {}

    template <typename Alloc,
        typename =
            enable_if_t<traits::template is_constructible_with_allocator<Alloc, Types&&...>::value>>
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple&& other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        Types&&...>::value)
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<Types...>{}) {}

    template <typename Alloc,
        typename = enable_if_t<
            traits::template is_constructible_with_allocator<Alloc, Types const&&...>::value>>
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc,
        tuple const&&
            other) noexcept(traits::template is_nothrow_constructible_with_allocator<Alloc,
        Types const&&...>::value)
        : tuple(allocator_arg, alloc, move(other), index_sequence_for<Types...>{}) {}

public:
    template <typename... UTypes,
        typename = enable_if_t<
            conjunction<typename traits::template is_assignable<UTypes const&...>>::value>>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 tuple& operator=(tuple<UTypes...> const& other) noexcept(
        traits::template is_nothrow_assignable<UTypes const&...>::value) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <typename... UTypes,
        typename = enable_if_t<
            conjunction<typename traits::template is_const_assignable<UTypes const&...>>::value>>
    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple<UTypes...> const& other) const
        noexcept(traits::template is_nothrow_const_assignable<UTypes const&...>::value) {
        return assign(other, index_sequence_for<Types...>{});
    }

public:
    template <typename... UTypes,
        enable_if_t<conjunction<typename traits::template is_assignable<UTypes&&...>>::value, int> =
            0>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 tuple& operator=(tuple<UTypes...>&& other) noexcept(
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
    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple<UTypes...>&& other) const
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
        enable_if_t<conjunction<is_tuple_like<TupleLike>, not_this<TupleLike>,
                        bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                        details::tuple::rebind_references_t<traits::template is_assignable,
                            TupleLike, sizeof...(Types)>>::value,
            int> = 0>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 tuple& operator=(TupleLike&& other) noexcept(
        conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            details::tuple::rebind_references_t<traits::template is_nothrow_assignable, TupleLike,
                sizeof...(Types)>>::value) {
        return assign(forward<TupleLike>(other), index_sequence_for<Types...>{});
    }

    template <typename TupleLike,
        enable_if_t<conjunction<is_tuple_like<TupleLike>, not_this<TupleLike>,
                        bool_constant<tuple_size<TupleLike>::value == sizeof...(Types)>,
                        details::tuple::rebind_references_t<traits::template is_const_assignable,
                            TupleLike, sizeof...(Types)>>::value,
            int> = 1>
    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(TupleLike&& other) const
        noexcept(conjunction<details::tuple::is_all_nothrow_gettable<TupleLike, sizeof...(Types)>,
            details::tuple::rebind_references_t<traits::template is_nothrow_const_assignable,
                TupleLike, sizeof...(Types)>>::value) {
        return assign(forward<TupleLike>(other), index_sequence_for<Types...>{});
    }
};

namespace details {
namespace tuple {
template <size_t I, typename T, typename U>
UTL_ATTRIBUTES(CONST, _HIDE_FROM_ABI) constexpr enable_if_t<(I == tuple_size<T>::value), bool> less(
    T const& l, U const& r) noexcept {
    return false;
}

template <size_t I, typename T, typename U>
__UTL_HIDE_FROM_ABI constexpr enable_if_t<(I < tuple_size<T>::value), bool> less(
    T const& l, U const& r) noexcept(conjunction<compare_ops::all_have_nothrow_eq<T, U>,
    compare_ops::all_have_nothrow_lt<T, U>>::value) {
    return (__UTL get_element<I - 1>(l) == __UTL get_element<I - 1>(r)) &&
        ((__UTL get_element<I>(l) < __UTL get_element<I>(r)) || less<I + 1>(l, r));
}

template <typename T, typename U>
__UTL_HIDE_FROM_ABI constexpr bool less(T const& l, U const& r) noexcept(
    conjunction<compare_ops::all_have_nothrow_eq<T, U>,
        compare_ops::all_have_nothrow_lt<T, U>>::value) {
    static_assert(compare_ops::all_have_eq<T, U>::value, "All elements must be comparable");
    static_assert(compare_ops::all_have_lt<T, U>::value, "All elements must be comparable");
    return (__UTL get_element<0>(l) < __UTL get_element<0>(r)) || less<1>(l, r);
}

template <size_t I, typename T, typename U>
UTL_ATTRIBUTES(CONST, _HIDE_FROM_ABI) constexpr enable_if_t<(I == tuple_size<T>::value), bool> equals(
    T const& l, U const& r) noexcept {
    return true;
}

template <size_t I, typename T, typename U>
__UTL_HIDE_FROM_ABI constexpr enable_if_t<(I < tuple_size<T>::value), bool> equals(
    T const& l, U const& r) noexcept(compare_ops::all_have_nothrow_eq<T, U>::value) {
    return (__UTL get_element<I>(l) == __UTL get_element<I>(r)) && equals<I + 1>(l, r);
}

template <typename T, typename U>
__UTL_HIDE_FROM_ABI constexpr bool equals(T const& l, U const& r) noexcept(
    compare_ops::all_have_nothrow_eq<T, U>::value) {
    static_assert(compare_ops::all_have_eq<T, U>::value, "All elements must be comparable");
    return equals<0>(l, r);
}

} // namespace tuple
} // namespace details

template <typename... Ts, typename... Us>
UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) constexpr enable_if_t<
    conjunction<compare_ops::all_have_eq<tuple<Ts...>, tuple<Us...>>>::value, bool>
operator==(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(
    noexcept(details::tuple::equals(l, r))) {
    return details::tuple::equals(l, r);
}

template <typename... Ts, typename... Us>
UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) constexpr enable_if_t<
    conjunction<compare_ops::all_have_eq<tuple<Ts...>, tuple<Us...>>,
        compare_ops::all_have_lt<tuple<Ts...>, tuple<Us...>>>::value,
    bool>
operator<(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(
    noexcept(details::tuple::less(l, r))) {
    return details::tuple::less(l, r);
}

UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr bool operator==(
    tuple<> const& l, tuple<> const& r) noexcept {
    return true;
}

UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr bool operator<(
    tuple<> const& l, tuple<> const& r) noexcept {
    return false;
}

template <typename... Ts, typename... Us>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr enable_if_t<
    UTL_TRAIT_is_equality_comparable_with(tuple<Ts...>, tuple<Us...>), bool>
operator!=(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(!(l == r))) {
    return !(l == r);
}

template <typename... Ts, typename... Us>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr enable_if_t<
    UTL_TRAIT_is_strict_subordinate_comparable_with(tuple<Us...>, tuple<Ts...>), bool>
operator<=(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(!(r < l))) {
    return !(r < l);
}

template <typename... Ts, typename... Us>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr enable_if_t<
    UTL_TRAIT_is_strict_subordinate_comparable_with(tuple<Us...>, tuple<Ts...>), bool>
operator>(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(
    noexcept(static_cast<bool>(r < l))) {
    return static_cast<bool>(r < l);
}

template <typename... Ts, typename... Us>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr enable_if_t<
    UTL_TRAIT_is_strict_subordinate_comparable_with(tuple<Ts...>, tuple<Us...>), bool>
operator>=(tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(!(l < r))) {
    return !(l < r);
}

UTL_NAMESPACE_END
