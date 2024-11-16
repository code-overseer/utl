// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if !defined(UTL_EXPECTED_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/initializer_list/utl_initializer_list_fwd.h"

#include "utl/expected/utl_expected_common.h"
#include "utl/expected/utl_expected_details_cpp17.h"
#include "utl/expected/utl_unexpected.h"
#include "utl/memory/utl_addressof.h"
#include "utl/ranges/swap.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_destructible.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_swappable.h"
#include "utl/type_traits/utl_is_trivially_constructible.h"
#include "utl/type_traits/utl_is_trivially_copy_assignable.h"
#include "utl/type_traits/utl_is_trivially_copy_constructible.h"
#include "utl/type_traits/utl_is_trivially_destructible.h"
#include "utl/type_traits/utl_is_trivially_move_assignable.h"
#include "utl/type_traits/utl_is_trivially_move_constructible.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_in_place.h"
#include "utl/utility/utl_move.h"

#include <new>

#define __UTL_ATTRIBUTE_EXPECTED_INLINE_PURE \
    (ALWAYS_INLINE)(PURE)(NODISCARD)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_PURE
#define __UTL_ATTRIBUTE_EXPECTED_INLINE_CONST \
    (ALWAYS_INLINE)(CONST)(NODISCARD)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_CONST

UTL_NAMESPACE_BEGIN

template <typename T, typename E>
class __UTL_PUBLIC_TEMPLATE expected : private details::expected::storage<T, E> {
    static_assert(!UTL_TRAIT_is_same(remove_cvref_t<T>, unexpect_t), "Invalid type");
    static_assert(!UTL_TRAIT_is_same(remove_cvref_t<T>, in_place_t), "Invalid type");
    static_assert(!__UTL_TRAIT_is_unexpected(remove_cvref_t<T>), "Invalid type");
    static_assert(UTL_TRAIT_is_destructible(T), "Invalid type");
    static_assert(UTL_TRAIT_is_complete(unexpected<E>), "Invalid type");
    using base_type = details::expected::storage<T, E>;

    template <class T1, class E1, class T1Qual, class E1Qual>
    using can_convert = conjunction<is_constructible<T, T1Qual>, is_constructible<E, E1Qual>,
        conditional_t<negation<is_same<remove_cv_t<T>, bool>>::value,
            conjunction<negation<conjunction<is_same<T, T1>, is_same<E, E1>>>,
                negation<is_constructible<T, expected<T1, E1>&>>,
                negation<is_constructible<T, expected<T1, E1>>>,
                negation<is_constructible<T, expected<T1, E1> const&>>,
                negation<is_constructible<T, expected<T1, E1> const>>,
                negation<is_convertible<expected<T1, E1>&, T>>,
                negation<is_convertible<expected<T1, E1>&&, T>>,
                negation<is_convertible<expected<T1, E1> const&, T>>,
                negation<is_convertible<expected<T1, E1> const&&, T>>>,
            true_type>,
        negation<is_constructible<unexpected<E>, expected<T1, E1>&>>,
        negation<is_constructible<unexpected<E>, expected<T1, E1>>>,
        negation<is_constructible<unexpected<E>, expected<T1, E1> const&>>,
        negation<is_constructible<unexpected<E>, expected<T1, E1> const>>>;

    template <typename T1, typename E1>
    friend class expected;
    using base_type::protected_tag;

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;
    template <typename U>
    using rebind = expected<U, error_type>;

    template <typename T = is_implicit_constructible<T>, enable_if_t<T::value, int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr expected() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T)) = default;
    template <typename T = is_explicit_constructible<T>, enable_if_t<T::value, int> = 1>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected const& other) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;
    template <typename T1, typename E1,
        enable_if_t<conjunction_v<negation<conjunction_v<is_convertible_v<T1 const&, T>,
                                      is_convertible_v<E1 const&, E>>>,
                        can_convert<T1, E1, T1 const&, E1 const&>>,
            int> = 0>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(expected<T1, E1> const& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, T1 const&) &&
        UTL_TRAIT_is_nothrow_constructible(E, E1 const&))
        : base_type(protected_tag, other.has_value(), other.data_ref()) {}

    template <typename T1, typename E1,
        enable_if_t<conjunction_v<is_convertible<T1 const&, T>, is_convertible<E1 const&, E>,
                        can_convert<T1, E1, T1 const&, E1 const&>>,
            int> = 1>
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected<T1, E1> const& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, T1 const&) &&
        UTL_TRAIT_is_nothrow_constructible(E, E1 const&))
        : base_type(protected_tag, other.has_value(), other.data_ref()) {}

    template <typename T1, typename E1,
        enable_if_t<
            conjunction_v<negation<conjunction_v<is_convertible_v<T1, T>, is_convertible_v<E1, E>>>,
                can_convert<T1, E1, T1, E1>>,
            int> = 0>
    template <typename T1, typename E1, enable_if_t<can_convert<T1, E1, T1, E1>, int> = 0>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(expected<T1, E1>&& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, T1) && UTL_TRAIT_is_nothrow_constructible(E, E1))
        : base_type(protected_tag, other.has_value(), __UTL move(other.data_ref())) {}

    template <typename T1, typename E1,
        enable_if_t<conjunction_v<is_convertible<T1, T>, is_convertible<E1, E>,
                        can_convert<T1, E1, T1, E1>>,
            int> = 1>
    template <typename T1, typename E1, enable_if_t<can_convert<T1, E1, T1, E1>, int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected<T1, E1>&& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, T1) && UTL_TRAIT_is_nothrow_constructible(E, E1))
        : base_type(protected_tag, other.has_value(), __UTL move(other.data_ref())) {}

    template <typename U = T,
        enable_if_t<!is_same_v<in_place_t, remove_cvref_t<U>> &&
                !is_same_v<expected, remove_cvref_t<U>> && is_constructible_v<T, U> &&
                !__UTL_TRAIT_is_unexpected(U) && is_convertible_v<U, T>,
            int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr expected(U&& value) noexcept(
        is_nothrow_constructible_v<T, U>)
        : base_type(__UTL in_place, __UTL forward<U>(value)) {}

    template <typename U = T,
        enable_if_t<!is_same_v<in_place_t, remove_cvref_t<U>> &&
                !is_same_v<expected, remove_cvref_t<U>> && is_constructible_v<T, U> &&
                !__UTL_TRAIT_is_unexpected(U) && !is_convertible_v<U, T>,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(U&& value) noexcept(
        is_nothrow_constructible_v<T, U>)
        : base_type(__UTL in_place, __UTL forward<U>(value)) {}

    template <typename U,
        enable_if_t<is_constructible_v<E, U> && __UTL_TRAIT_is_unexpected(U) &&
                is_convertible_v<U, E>,
            int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr expected(U&& e) noexcept(
        noexcept(base_type(__UTL unexpect, __UTL forward_like<U>(e.error()))))
        : base_type(__UTL unexpect, __UTL forward_like<U>(e.error())) {}

    template <typename U,
        enable_if_t<is_constructible_v<E, U> && __UTL_TRAIT_is_unexpected(U) &&
                !is_convertible_v<U, E>,
            int> = 1>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(U&& e) noexcept(
        noexcept(base_type(__UTL unexpect, __UTL forward_like<U>(e.error()))))
        : base_type(__UTL unexpect, __UTL forward_like<U>(e.error())) {}

    template <typename... Args, enable_if_t<is_constructible_v<T, Args...>, int> = 0>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(in_place_t, Args&&... args) noexcept(
        is_nothrow_constructible_v<T, Args...>)
        : base_type(__UTL in_place, __UTL forward<Args>(args)...) {}

    template <typename U, typename... Args,
        enable_if_t<is_constructible_v<T, ::std::initializer_list<U>&, Args...>, int> = 0>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(in_place_t,
        ::std::initializer_list<U> il, Args&&... args) noexcept(is_nothrow_constructible_v<T,
        ::std::initializer_list<U>&, Args...>)
        : base_type(__UTL in_place, il, __UTL forward<Args>(args)...) {}

    template <typename... Args, enable_if_t<is_constructible_v<E, Args...>, int> = 0>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(unexpect_t, Args&&... args) noexcept(
        is_nothrow_constructible_v<E, Args...>)
        : base_type(__UTL unexpect, __UTL forward<Args>(args)...) {}

    template <typename U, typename... Args,
        enable_if_t<is_constructible_v<E, ::std::initializer_list<U>&, Args...>, int> = 0>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(unexpect_t,
        ::std::initializer_list<U> il, Args&&... args) noexcept(is_nothrow_constructible_v<E,
        ::std::initializer_list<U>&, Args...>)
        : base_type(__UTL unexpect, il, __UTL forward<Args>(args)...) {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected& operator=(expected const&) noexcept(
        is_nothrow_copy_assignable_v<T> && is_nothrow_copy_assignable_v<E>) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected& operator=(expected&&) noexcept(
        is_nothrow_move_assignable_v<T> && is_nothrow_move_assignable_v<E>) = default;

    template <typename U = T,
        enable_if_t<!is_same_v<expected, remove_cvref_t<U>> && !__UTL_TRAIT_is_unexpected(U) &&
                is_constructible_v<T, U> && is_assignable_v<T&, U>,
            (is_nothrow_constructible_v<T, U> || is_nothrow_move_constructible_v<T> ||
                is_nothrow_move_constructible_v<E>),
            int> = 0>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected& operator=(U&& u) noexcept(
        is_nothrow_constructible_v<T, U> && is_nothrow_move_constructible_v<T> &&
        is_nothrow_move_constructible_v<E>) {

        if (this->has_value()) {
            value() = __UTL forward<U>(u);
            return *this;
        }
    }

    using base_type::error;
    using base_type::has_value;
    using base_type::value;

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline UTL_CONSTEXPR_CXX14 T* operator->() noexcept {
        return __UTL addressof(this->value_ptr());
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T const* operator->() const noexcept {
        return __UTL addressof(this->value_ptr());
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline UTL_CONSTEXPR_CXX14 T& operator*() & noexcept { return this->value(); }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline UTL_CONSTEXPR_CXX14 T&& operator*() && noexcept {
        return __UTL move(this->value());
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T const& operator*() const& noexcept {
        return this->value();
    }
    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T const&& operator*() const&& noexcept {
        return __UTL move(this->value());
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_PURE) inline constexpr explicit operator bool() const noexcept {
        return this->has_value();
    }

    template <UTL_CONCEPT_CXX20(convertible_to<T>) U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_copy_constructible(T) && UTL_TRAIT_is_convertible(U, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr T value_or(U&& default_val) const& noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, T)) {
        return has_value() ? **this : static_cast<T>(__UTL forward<U>(default_val));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<T>) U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_move_constructible(T) && UTL_TRAIT_is_convertible(U, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr T value_or(U&& default_val) && noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, T)) {
        return has_value() ? __UTL move(**this) : static_cast<T>(__UTL forward<U>(default_val));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<E>) U = E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_copy_constructible(E) && UTL_TRAIT_is_convertible(U, E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr E error_or(U&& default_val) const& noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, E)) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val)) : error();
    }

    template <UTL_CONCEPT_CXX20(convertible_to<E>) U = E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_move_constructible(E) && UTL_TRAIT_is_convertible(U, E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr E error_or(U&& default_val) && noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, E)) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val)) : __UTL move(error());
    }

    template <UTL_CONCEPT_CXX20(invocable<T&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T&) && UTL_TRAIT_is_constructible(E, E&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto and_then(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&)) -> invoke_result_t<F, T&> {
        using return_type = remove_cvref_t<invoke_result_t<F, T&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");
        if (has_value()) {
            return __UTL invoke(__UTL forward<F>(f), value());
        }

        return return_type{unexpect, error()};
    }

    template <UTL_CONCEPT_CXX20(invocable<T const&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const&) && UTL_TRAIT_is_constructible(E, E const&))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, E const&>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&)) -> invoke_result_t<F, T const&> {
        using return_type = remove_cvref_t<invoke_result_t<F, T const&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        return has_value() ? __UTL invoke(__UTL forward<F>(f), value())
                           : return_type{__UTL unexpect, error()};
    }

    template <UTL_CONCEPT_CXX20(invocable<T>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T) && UTL_TRAIT_is_constructible(E, E&&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto and_then(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&&)) -> invoke_result_t<F, T> {
        using return_type = remove_cvref_t<invoke_result_t<F, T&&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        if (has_value()) {
            return __UTL invoke(__UTL forward<F>(f), __UTL move(value()));
        }

        return return_type{__UTL unexpect, __UTL move(error())};
    }

    template <UTL_CONCEPT_CXX20(invocable<T const>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const) && UTL_TRAIT_is_constructible(E, E const&&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&&)) -> invoke_result_t<F, T const> {
        using return_type = remove_cvref_t<invoke_result_t<F, T const&&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        return has_value() ? __UTL invoke(__UTL forward<F>(f), __UTL move(value()))
                           : return_type{__UTL unexpect, __UTL move(error())};
    }

    template <UTL_CONCEPT_CXX20(invocable<T&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T&) && UTL_TRAIT_is_constructible(E, E&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&)) -> expected<remove_cv_t<invoke_result_t<F, T&>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T&>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T&>)),
            "Invalid return type");
        if (has_value()) {
            if constexpr (is_void_v<return_type>) {
                __UTL invoke(__UTL forward<F>(f), value());
                return expected<void, E>{};
            } else {
                return expected<return_type, E>{
                    __UTL details::expected::transforming, __UTL forward<F>(f), value()};
            }
            return details::expected::transformation(__UTL forward<F>(f), value());
        }

        return expected<return_type, E>{__UTL unexpect, error()};
    }

    template <UTL_CONCEPT_CXX20(invocable<T const&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const&) && UTL_TRAIT_is_constructible(E, E const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&))
        -> expected<remove_cv_t<invoke_result_t<F, T const&>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T const&>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T const&>)),
            "Invalid return type");

        if (has_value()) {
            if constexpr (is_void_v<return_type>) {
                __UTL invoke(__UTL forward<F>(f), value());
                return expected<void, E>{};
            } else {
                return expected<return_type, E>{
                    __UTL details::expected::transforming, __UTL forward<F>(f), value()};
            }
            return details::expected::transformation(__UTL forward<F>(f), value());
        }

        return expected<return_type, E>{__UTL unexpect, error()};
    }

    template <UTL_CONCEPT_CXX20(invocable<T>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T) && UTL_TRAIT_is_constructible(E, E&&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&&)) -> expected<remove_cv_t<invoke_result_t<F, T>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T>)),
            "Invalid return type");

        if (has_value()) {
            if constexpr (is_void_v<return_type>) {
                __UTL invoke(__UTL forward<F>(f), value());
                return expected<void, E>{};
            } else {
                return expected<return_type, E>{__UTL details::expected::transforming,
                    __UTL forward<F>(f), __UTL move(value())};
            }
            return details::expected::transformation(__UTL forward<F>(f), __UTL move(value()));
        }

        return expected<return_type, E>{__UTL unexpect, __UTL move(error())};
    }

    template <UTL_CONCEPT_CXX20(invocable<T&&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const) && UTL_TRAIT_is_constructible(E, E const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&&))
        -> expected<remove_cv_t<invoke_result_t<F, T const>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T const>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T const>)),
            "Invalid return type");

        if (has_value()) {
            if constexpr (is_void_v<return_type>) {
                __UTL invoke(__UTL forward<F>(f), value());
                return expected<void, E>{};
            } else {
                return expected<return_type, E>{__UTL details::expected::transforming,
                    __UTL forward<F>(f), __UTL move(value())};
            }
            return details::expected::transformation(__UTL forward<F>(f), __UTL move(value()));
        }

        return expected<return_type, E>{__UTL unexpect, __UTL move(error())};
    }

    template <UTL_CONCEPT_CXX20(invocable<E&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E&) && UTL_TRAIT_is_constructible(T, T&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto or_else(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E&)) -> invoke_result_t<F, E&> {
        using return_type = remove_cvref_t<invoke_result_t<F, E&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::value_type, T),
            "Invalid return type");
        if (has_value()) {
            return return_type{__UTL in_place, value()};
        }

        return __UTL invoke(__UTL forward<F>(f), error());
    }

    template <UTL_CONCEPT_CXX20(invocable<E const&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E const&) && UTL_TRAIT_is_constructible(T, T const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E const&)) -> invoke_result_t<F, E const&> {
        using return_type = remove_cvref_t<invoke_result_t<F, E const&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::value_type, T),
            "Invalid return type");

        return has_value() ? return_type{__UTL in_place, value()}
                           : __UTL
                             invoke(__UTL forward<F>(f), error());
    }

    template <UTL_CONCEPT_CXX20(invocable<E>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E) && UTL_TRAIT_is_constructible(T, T&&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto or_else(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E&&)) -> invoke_result_t<F, E> {
        using return_type = remove_cvref_t<invoke_result_t<F, E>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::value_type, T),
            "Invalid return type");

        if (has_value()) {
            return return_type{__UTL in_place, __UTL move(value())};
        }

        return __UTL invoke(__UTL forward<F>(f), __UTL move(error()));
    }

    template <UTL_CONCEPT_CXX20(invocable<E const>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E const) && UTL_TRAIT_is_constructible(T, T const))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E const&&)) -> invoke_result_t<F, E const> {
        using return_type = remove_cvref_t<invoke_result_t<F, E const&&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::value_type, T),
            "Invalid return type");

        return has_value() ? return_type{__UTL in_place, __UTL move(value())}
                           : __UTL
                             invoke(__UTL forward<F>(f), __UTL move(error()));
    }

    template <UTL_CONCEPT_CXX20(invocable<E&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E&) && UTL_TRAIT_is_constructible(T, T&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform_error(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E&)) -> expected<T, remove_cv_t<invoke_result_t<F, E&>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E&>>;
        static_assert(UTL_TRAIT_is_complete(expected<T, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E&>),
            "Invalid return type");
        if (has_value()) {
            return expected<T, return_type>{__UTL in_place, value()};
        }

        return expected<return_type, E>{
            __UTL details::expected::transforming_error, __UTL forward<F>(f), error()};
    }

    template <UTL_CONCEPT_CXX20(invocable<E const&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E const&) && UTL_TRAIT_is_constructible(T, T const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E const&))
        -> expected<T, remove_cv_t<invoke_result_t<F, E const&>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E const&>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E const&>),
            "Invalid return type");

        return has_value() ? expected<T, return_type>{__UTL in_place, value()}
                           : expected<T, return_type>(__UTL details::expected::transforming_error,
                                 __UTL forward<F>(f), error());
    }

    template <UTL_CONCEPT_CXX20(invocable<E>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E) && UTL_TRAIT_is_constructible(T, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform_error(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E)) -> expected<T, remove_cv_t<invoke_result_t<F, E>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E>>;
        static_assert(UTL_TRAIT_is_complete(expected<E, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E>),
            "Invalid return type");

        if (has_value()) {
            return expected<T, return_type>{__UTL in_place, __UTL move(value())};
        }

        return expected<T, return_type>{
            __UTL details::expected::transforming_error, __UTL forward<F>(f), __UTL move(error())};
    }

    template <UTL_CONCEPT_CXX20(invocable<E const>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E const) && UTL_TRAIT_is_constructible(T, T const))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E const))
        -> expected<T, remove_cv_t<invoke_result_t<F, E const>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E const>>;
        static_assert(UTL_TRAIT_is_complete(expected<T, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E const>),
            "Invalid return type");

        return has_value() ? expected<T, return_type>{__UTL in_place, __UTL move(value())};
            : expected<T, return_type>{__UTL details::expected::transforming_error, 
            __UTL forward<F>(f), __UTL move(error())};
    }

    template <typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_nothrow_constructible(T, Args...))>
    __UTL_HIDE_FROM_ABI inline T& emplace(Args&&... args) noexcept {
        this->emplace_value(__UTL forward<Args>(args)...);
        return *ptr;
    }

    template <typename U, typename... Args UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_nothrow_constructible(T, ::std::initializer_list<U>&, Args...))>
    __UTL_HIDE_FROM_ABI inline T& emplace(::std::initializer_list<U> il, Args&&... args) noexcept {
        this->emplace_value(il, __UTL forward<Args>(args)...);
        return *ptr;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(expected& other) noexcept(
        UTL_TRAIT_is_nothrow_swappable(T) && UTL_TRAIT_is_nothrow_swappable(E) &&
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E)) {
        if (has_value() == other.has_value()) {
            if (has_value()) {
                __UTL ranges::swap(value(), other.value());
            } else {
                __UTL ranges::swap(error(), other.error());
            }
        } else {
            cross_swap(other);
        }
    }

    __UTL_HIDE_FROM_ABI friend void swap(expected& left, expected& right) noexcept(
        noexcept(left.swap(right))) {
        left.swap(right);
    }

    template <typename T2, typename E2, typename = enable_if_t<!UTL_TRAIT_is_void(T2)>>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(
        expected const& left, expected<T2, E2> const& right) {
        if (left.has_value() != right.has_value()) {
            return false;
        }

        return left.has_value() ? left.value() == right.value() : left.error() == right.error();
    }

    template <typename T2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(
        expected const& left, T2 const& right) {
        static_assert(UTL_TRAIT_is_equality_comparable_with(T, T2), "Program ill-formed");
        static_assert(
            UTL_TRAIT_is_boolean_testable(decltype(left.value() == right)), "Program ill-formed");
        return left.has_value() && left.value() == right;
    }

    template <typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(
        expected const& left, unexpected<E2> const& right) {
        static_assert(UTL_TRAIT_is_equality_comparable_with(E, E2), "Program ill-formed");
        static_assert(UTL_TRAIT_is_boolean_testable(decltype(left.error() == right.error())),
            "Program ill-formed");
        return !left.has_value() && left.error() == right.error();
    }

private:
#if UTL_CXX17
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void cross_swap(expected& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E)) {
        if constexpr (UTL_TRAIT_is_nothrow_move_constructible(E)) {
            E tmp(__UTL move(other.error()));
            UTL_TRY {
                other.emplace_value(__UTL move(this->value()));
                this->emplace_error(__UTL move(tmp));
            } UTL_CATCH(...) {
                other.error() = __UTL move(tmp);
                UTL_RETHROW();
            }
        } else {
            T tmp(__UTL move(this->value()));
            UTL_TRY {
                this->emplace_error(__UTL move(other.error()));
                other.emplace_value(__UTL move(this->value()));
            } UTL_CATCH(...) {
                this->value() = __UTL move(tmp);
                UTL_RETHROW();
            }
        }
    }
#else
    template <typename R = enable_if<UTL_TRAIT_is_nothrow_move_constructible(E)>>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 typename R::type cross_swap(expected& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T)) {
        E tmp(__UTL move(other.error()));
        UTL_TRY {
            other.emplace_value(__UTL move(this->value()));
            this->emplace_error(__UTL move(tmp));
        } UTL_CATCH(...) {
            other.error() = __UTL move(tmp);
            UTL_RETHROW();
        }
    }

    template <typename R = enable_if<!UTL_TRAIT_is_nothrow_move_constructible(E)>>
    __UTL_HIDE_FROM_ABIinline UTL_CONSTEXPR_CXX14 typename R::type cross_swap(expected& other)
        UTL_THROWS {
        T tmp(__UTL move(this->value()));
        UTL_TRY {
            this->emplace_error(__UTL move(other.error()));
            other.emplace_value(__UTL move(this->value()));
        } UTL_CATCH(...) {
            this->value() = __UTL move(tmp);
            UTL_RETHROW();
        }
    }
#endif
};

template <typename E>
class expected<void, E>;

template <typename E>
class expected<void const, E> : expected<void, E> {};
template <typename E>
class expected<void volatile, E> : expected<void, E> {};
template <typename E>
class expected<void const volatile, E> : expected<void, E> {};

UTL_NAMESPACE_END

#undef __UTL_TRAIT_assingment_initializable
#undef __UTL_DELETE_OR_UNDEFINED
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_PURE
#undef __UTL_ATTRIBUTE_EXPECTED_INLINE_PURE
