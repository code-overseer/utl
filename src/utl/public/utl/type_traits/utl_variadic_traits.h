// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/memory/utl_uses_allocator.h"
#include "utl/type_traits/utl_add_lvalue_reference.h"
#include "utl/type_traits/utl_add_rvalue_reference.h"
#include "utl/type_traits/utl_is_copy_assignable.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_move_assignable.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_reference_constructs_from_temporary.h"
#include "utl/utility/utl_sequence.h"
#include "utl/utility/utl_swap.h"

UTL_NAMESPACE_BEGIN

using size_t = decltype(sizeof(0));

template <typename... Types>
struct __UTL_PUBLIC_TEMPLATE variadic_traits {
    static constexpr size_t size = sizeof...(Types);

    static constexpr bool is_nothrow_swappable =
        conjunction<__UTL is_nothrow_swappable<Types>...>::value;
    static constexpr bool is_nothrow_const_swappable =
        conjunction<__UTL is_nothrow_swappable<add_const_t<Types>>...>::value;
    static constexpr bool is_nothrow_default_constructible =
        conjunction<__UTL is_nothrow_default_constructible<Types>...>::value;
    static constexpr bool is_nothrow_copy_constructible =
        conjunction<__UTL is_nothrow_copy_constructible<Types>...>::value;
    static constexpr bool is_nothrow_copy_assignable =
        conjunction<__UTL is_nothrow_copy_assignable<Types>...>::value;
    static constexpr bool is_nothrow_const_copy_assignable =
        conjunction<__UTL is_nothrow_copy_assignable<add_const_t<Types>>...>::value;
    static constexpr bool is_nothrow_const_move_assignable =
        conjunction<__UTL is_nothrow_assignable<add_lvalue_reference_t<add_const_t<Types>>,
            add_rvalue_reference_t<Types>>...>::value;
    static constexpr bool is_nothrow_move_constructible =
        conjunction<__UTL is_nothrow_move_constructible<Types>...>::value;
    static constexpr bool is_nothrow_move_assignable =
        conjunction<__UTL is_nothrow_move_assignable<Types>...>::value;
    static constexpr bool is_implicit_default_constructible =
        conjunction<__UTL is_implicit_constructible<Types>...>::value;
    static constexpr bool is_explicit_default_constructible =
        disjunction<__UTL is_explicit_constructible<Types>...>::value;
    static constexpr bool is_copy_constructible =
        conjunction<__UTL is_copy_constructible<Types>...>::value;
    static constexpr bool is_move_constructible =
        conjunction<__UTL is_move_constructible<Types>...>::value;
    static constexpr bool is_copy_assignable =
        conjunction<__UTL is_copy_assignable<Types>...>::value;
    static constexpr bool is_const_copy_assignable =
        conjunction<__UTL is_copy_assignable<add_const_t<Types>>...>::value;
    static constexpr bool is_move_assignable =
        conjunction<__UTL is_move_assignable<Types>...>::value;
    static constexpr bool is_const_move_assignable =
        conjunction<__UTL is_move_assignable<add_const_t<Types>>...>::value;
    static constexpr bool is_empty = conjunction<__UTL is_empty<Types>...>::value;
    static constexpr bool is_swappable = conjunction<__UTL is_swappable<Types>...>::value;
    static constexpr bool is_const_swappable =
        conjunction<__UTL is_swappable<add_const_t<Types>>...>::value;

    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE matches : conjunction<__UTL is_same<Types, UTypes>...> {};

    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_nothrow_assignable :
        conjunction<__UTL is_nothrow_assignable<add_lvalue_reference_t<Types>, UTypes>...> {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_nothrow_const_assignable :
        conjunction<
            __UTL is_nothrow_assignable<add_lvalue_reference_t<add_const_t<Types>>, UTypes>...> {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_nothrow_constructible :
        conjunction<__UTL is_nothrow_constructible<Types, UTypes>...> {};
    template <typename Alloc, typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_nothrow_constructible_with_allocator :
        conjunction<disjunction<__UTL is_nothrow_constructible_with_allocator<Types, Alloc, UTypes>,
            __UTL is_nothrow_constructible<Types, UTypes>>... // disjunction
            > {};
    template <typename Alloc>
    struct __UTL_PUBLIC_TEMPLATE is_nothrow_constructible_with_allocator<Alloc> :
        conjunction<disjunction<__UTL is_nothrow_constructible_with_allocator<Types, Alloc>,
            __UTL is_nothrow_constructible<Types>>... // disjunction
            > {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_convertible :
        conjunction<__UTL is_convertible<Types, UTypes>...> {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_implicit_constructible :
        conjunction<__UTL is_implicit_constructible<Types, UTypes>...> {};
    template <typename Alloc, typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_implicit_constructible_with_allocator :
        conjunction<conditional_t<__UTL uses_allocator<Types, Alloc>::value,
            __UTL is_implicit_constructible_with_allocator<Types, Alloc, UTypes>,
            __UTL is_implicit_constructible<Types, UTypes>>... // disjunction
            > {};

    template <typename Alloc>
    struct __UTL_PUBLIC_TEMPLATE is_implicit_constructible_with_allocator<Alloc> :
        conjunction<conditional_t<__UTL uses_allocator<Types, Alloc>::value,
            __UTL is_implicit_constructible_with_allocator<Types, Alloc>,
            __UTL is_implicit_constructible<Types>>... // disjunction
            > {};

    template <typename... UTypes UTL_REQUIRES_CXX11(sizeof...(Types) != sizeof...(UTypes))>
    UTL_REQUIRES_CXX20(sizeof...(Types) != sizeof...(UTypes))
    static auto is_explicit_constructible_impl() noexcept -> false_type;
    template <typename... UTypes UTL_REQUIRES_CXX11(sizeof...(Types) == sizeof...(UTypes))>
    UTL_REQUIRES_CXX20(sizeof...(Types) == sizeof...(UTypes))
    static auto is_explicit_constructible_impl() noexcept
        -> disjunction<__UTL is_explicit_constructible<Types, UTypes>...>;
    template <typename... UTypes>
    using is_explicit_constructible = decltype(is_explicit_constructible_impl<UTypes...>());
    template <typename Alloc, typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_explicit_constructible_with_allocator :
        disjunction<conditional_t<__UTL uses_allocator<Types, Alloc>::value,
            __UTL is_explicit_constructible_with_allocator<Types, Alloc, UTypes>,
            __UTL is_explicit_constructible<Types, UTypes>>...> {};
    template <typename Alloc>
    struct __UTL_PUBLIC_TEMPLATE is_explicit_constructible_with_allocator<Alloc> :
        disjunction<conditional_t<__UTL uses_allocator<Types, Alloc>::value,
            __UTL is_explicit_constructible_with_allocator<Types, Alloc>,
            __UTL is_explicit_constructible<Types>>...> {};

    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_constructible :
        conjunction<__UTL is_constructible<Types, UTypes>...> {};
    template <typename Alloc, typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_constructible_with_allocator :
        conjunction<disjunction<__UTL is_constructible_with_allocator<Types, Alloc, UTypes>,
            __UTL is_constructible<Types, UTypes>>... // disjunction
            > {};
    template <typename Alloc>
    struct __UTL_PUBLIC_TEMPLATE is_constructible_with_allocator<Alloc> :
        conjunction<disjunction<__UTL is_constructible_with_allocator<Types, Alloc>,
            __UTL is_constructible<Types>>... // disjunction
            > {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_assignable :
        conjunction<__UTL is_assignable<add_lvalue_reference_t<Types>, UTypes>...> {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_const_assignable :
        conjunction<__UTL is_assignable<add_lvalue_reference_t<add_const_t<Types>>, UTypes>...> {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_dangling :
        disjunction<__UTL reference_constructs_from_temporary<Types, UTypes>...> {};

    template <typename Alloc, typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_dangling_without_allocator :
        disjunction<conjunction<negation<__UTL uses_allocator<Types, Alloc>>,
            __UTL reference_constructs_from_temporary<Types, UTypes>>... // conjunction
            > {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_convertible_from :
        conjunction<__UTL is_convertible<UTypes, Types>...> {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_swappable_with :
        conjunction<__UTL is_swappable_with<Types&, UTypes>...> {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_const_swappable_with :
        conjunction<__UTL is_swappable_with<Types const&, UTypes>...> {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_nothrow_swappable_with :
        conjunction<__UTL is_nothrow_swappable_with<Types&, UTypes>...> {};
    template <typename... UTypes>
    struct __UTL_PUBLIC_TEMPLATE is_nothrow_const_swappable_with :
        conjunction<__UTL is_nothrow_swappable_with<Types const&, UTypes>...> {};
};

UTL_NAMESPACE_END
