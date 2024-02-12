// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"
#include "utl/memory/utl_uses_allocator.h"
#include "utl/type_traits/utl_constructor_traits.h"
#include "utl/type_traits/utl_std_traits.h"
#include "utl/utility/utl_sequence.h"
#include "utl/utility/utl_swap.h"

UTL_NAMESPACE_BEGIN

using size_t = decltype(sizeof(0));

template<typename... Types>
struct variadic_traits {
    static constexpr size_t size = sizeof...(Types);

    static constexpr bool is_nothrow_swappable = conjunction<UTL_SCOPE is_nothrow_swappable<Types>...>::value;
    static constexpr bool is_nothrow_const_swappable = conjunction<UTL_SCOPE is_nothrow_swappable<add_const_t<Types>>...>::value;
    static constexpr bool is_nothrow_default_constructible = conjunction<UTL_SCOPE is_nothrow_default_constructible<Types>...>::value;
    static constexpr bool is_nothrow_copy_constructible = conjunction<UTL_SCOPE is_nothrow_copy_constructible<Types>...>::value;
    static constexpr bool is_nothrow_copy_assignable = conjunction<UTL_SCOPE is_nothrow_copy_assignable<Types>...>::value;
    static constexpr bool is_nothrow_const_copy_assignable = conjunction<UTL_SCOPE is_nothrow_copy_assignable<add_const_t<Types>>...>::value;
    static constexpr bool is_nothrow_const_move_assignable = conjunction<UTL_SCOPE is_nothrow_assignable<add_lvalue_reference_t<add_const_t<Types>>, add_rvalue_reference_t<Types>>...>::value;
    static constexpr bool is_nothrow_move_constructible = conjunction<UTL_SCOPE is_nothrow_move_constructible<Types>...>::value;
    static constexpr bool is_nothrow_move_assignable = conjunction<UTL_SCOPE is_nothrow_move_assignable<Types>...>::value;
    static constexpr bool is_implicit_default_constructible = conjunction<UTL_SCOPE is_implicit_constructible<Types>...>::value;
    static constexpr bool is_explicit_default_constructible = disjunction<UTL_SCOPE is_explicit_constructible<Types>...>::value;
    static constexpr bool is_copy_constructible = conjunction<UTL_SCOPE is_copy_constructible<Types>...>::value;
    static constexpr bool is_move_constructible = conjunction<UTL_SCOPE is_move_constructible<Types>...>::value;
    static constexpr bool is_copy_assignable = conjunction<UTL_SCOPE is_copy_assignable<Types>...>::value;
    static constexpr bool is_const_copy_assignable = conjunction<UTL_SCOPE is_copy_assignable<add_const_t<Types>>...>::value;
    static constexpr bool is_move_assignable = conjunction<UTL_SCOPE is_move_assignable<Types>...>::value;
    static constexpr bool is_const_move_assignable = conjunction<UTL_SCOPE is_move_assignable<add_const_t<Types>>...>::value;
    static constexpr bool is_empty = conjunction<UTL_SCOPE is_empty<Types>...>::value;
    static constexpr bool is_swappable = conjunction<UTL_SCOPE is_swappable<Types>...>::value;
    static constexpr bool is_const_swappable = conjunction<UTL_SCOPE is_swappable<add_const_t<Types>>...>::value;

    template<typename... UTypes>
    struct matches : conjunction<
        UTL_SCOPE is_same<Types, UTypes>...
    > {};

    template<typename... UTypes>
    struct is_nothrow_assignable : conjunction<
        UTL_SCOPE is_nothrow_assignable<add_lvalue_reference_t<Types>, UTypes>...
    > {};
    template<typename... UTypes>
    struct is_nothrow_const_assignable : conjunction<
        UTL_SCOPE is_nothrow_assignable<add_lvalue_reference_t<add_const_t<Types>>, UTypes>...
    > {};
    template<typename... UTypes>
    struct is_nothrow_constructible : conjunction<
        UTL_SCOPE is_nothrow_constructible<Types, UTypes>...
    > {};
    template<typename Alloc, typename... UTypes>
    struct is_nothrow_constructible_with_allocator : conjunction<
        disjunction<
            UTL_SCOPE is_nothrow_constructible_with_allocator<Types, Alloc, UTypes>,
            UTL_SCOPE is_nothrow_constructible<Types, UTypes>
        >... // disjunction
    > {};
    template<typename Alloc>
    struct is_nothrow_constructible_with_allocator<Alloc> : conjunction<
        disjunction<
            UTL_SCOPE is_nothrow_constructible_with_allocator<Types, Alloc>,
            UTL_SCOPE is_nothrow_constructible<Types>
        >... // disjunction
    > {};
    template<typename... UTypes>
    struct is_convertible : conjunction<
        UTL_SCOPE is_convertible<Types, UTypes>...>{};
    template<typename... UTypes>
    struct is_implicit_constructible : conjunction<
        UTL_SCOPE is_implicit_constructible<Types, UTypes>...>{};
    template<typename Alloc, typename... UTypes>
    struct is_implicit_constructible_with_allocator : conjunction<
        conditional_t<
            UTL_SCOPE uses_allocator<Types, Alloc>::value,
            UTL_SCOPE is_implicit_constructible_with_allocator<Types, Alloc, UTypes>,
            UTL_SCOPE is_implicit_constructible<Types, UTypes>
        >... // disjunction
    > {};

    template<typename Alloc>
    struct is_implicit_constructible_with_allocator<Alloc> : conjunction<
        conditional_t<
            UTL_SCOPE uses_allocator<Types, Alloc>::value,
            UTL_SCOPE is_implicit_constructible_with_allocator<Types, Alloc>,
            UTL_SCOPE is_implicit_constructible<Types>
        >... // disjunction
    > {};

    template<typename... UTypes UTL_REQUIRES_CXX11(sizeof...(Types) != sizeof...(UTypes))>
    UTL_REQUIRES_CXX20(sizeof...(Types) != sizeof...(UTypes))
    static auto is_explicit_constructible_impl() noexcept -> false_type;
    template<typename... UTypes UTL_REQUIRES_CXX11(sizeof...(Types) == sizeof...(UTypes))>
    UTL_REQUIRES_CXX20(sizeof...(Types) == sizeof...(UTypes))
    static auto is_explicit_constructible_impl() noexcept -> disjunction<
        UTL_SCOPE is_explicit_constructible<Types, UTypes>...
    >;
    template<typename... UTypes>
    using is_explicit_constructible = decltype(is_explicit_constructible_impl<UTypes...>());
    template<typename Alloc, typename... UTypes>
    struct is_explicit_constructible_with_allocator : disjunction<
        conditional_t<
            UTL_SCOPE uses_allocator<Types, Alloc>::value,
            UTL_SCOPE is_explicit_constructible_with_allocator<Types, Alloc, UTypes>,
            UTL_SCOPE is_explicit_constructible<Types, UTypes>
        >...
    > {};
    template<typename Alloc>
    struct is_explicit_constructible_with_allocator<Alloc> : disjunction<
        conditional_t<
            UTL_SCOPE uses_allocator<Types, Alloc>::value,
            UTL_SCOPE is_explicit_constructible_with_allocator<Types, Alloc>,
            UTL_SCOPE is_explicit_constructible<Types>
        >...
    > {};
    
    template<typename... UTypes>
    struct is_constructible : conjunction<
        UTL_SCOPE is_constructible<Types, UTypes>...
    > {};
    template<typename Alloc, typename... UTypes>
    struct is_constructible_with_allocator : conjunction<
        disjunction<
            UTL_SCOPE is_constructible_with_allocator<Types, Alloc, UTypes>,
            UTL_SCOPE is_constructible<Types, UTypes>
        >... // disjunction
    > {};
    template<typename Alloc>
    struct is_constructible_with_allocator<Alloc> : conjunction<
        disjunction<
            UTL_SCOPE is_constructible_with_allocator<Types, Alloc>,
            UTL_SCOPE is_constructible<Types>
        >... // disjunction
    > {};
    template<typename... UTypes>
    struct is_assignable : conjunction<
        UTL_SCOPE is_assignable<add_lvalue_reference_t<Types>, UTypes>...
    > {};
    template<typename... UTypes>
    struct is_const_assignable : conjunction<
        UTL_SCOPE is_assignable<add_lvalue_reference_t<add_const_t<Types>>, UTypes>...
    > {};
    template<typename... UTypes>
    struct is_dangling : disjunction<UTL_SCOPE reference_constructs_from_temporary<Types, UTypes>...> {};
        
    template<typename Alloc, typename... UTypes>
    struct is_dangling_without_allocator : disjunction<
        conjunction<
            negation<UTL_SCOPE uses_allocator<Types, Alloc>>,
            UTL_SCOPE reference_constructs_from_temporary<Types, UTypes>
        >... // conjunction
    > {};
    template<typename... UTypes>
    struct is_convertible_from : conjunction<
        UTL_SCOPE is_convertible<UTypes, Types>...
    > {};
    template<typename... UTypes>
    struct is_swappable_with : conjunction<
        UTL_SCOPE is_swappable_with<Types&, UTypes>...
    > {};
    template<typename... UTypes>
    struct is_const_swappable_with : conjunction<
        UTL_SCOPE is_swappable_with<Types const&, UTypes>...
    > {};
    template<typename... UTypes>
    struct is_nothrow_swappable_with : conjunction<
        UTL_SCOPE is_nothrow_swappable_with<Types&, UTypes>...
    > {};
    template<typename... UTypes>
    struct is_nothrow_const_swappable_with : conjunction<
        UTL_SCOPE is_nothrow_swappable_with<Types const&, UTypes>...
    > {};
};

UTL_NAMESPACE_END


