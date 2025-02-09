// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if !defined(UTL_EXPECTED_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/initializer_list/utl_initializer_list_fwd.h"

#include "utl/expected/utl_bad_expected_access.h"
#include "utl/expected/utl_expected_common.h"
#include "utl/expected/utl_expected_details_cpp17.h"
#include "utl/expected/utl_unexpected.h"
#include "utl/functional/utl_invoke.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_is_assignable.h"
#include "utl/type_traits/utl_is_boolean.h"
#include "utl/type_traits/utl_is_boolean_testable.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_destructible.h"
#include "utl/type_traits/utl_is_equality_comparable.h"
#include "utl/type_traits/utl_is_function.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_assignable.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_void.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_in_place.h"
#include "utl/utility/utl_move.h"

#define __UTL_ATTRIBUTE_EXPECTED_INLINE_PURE \
    (ALWAYS_INLINE)(PURE)(NODISCARD)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_PURE
#define __UTL_ATTRIBUTE_EXPECTED_INLINE_CONST \
    (ALWAYS_INLINE)(CONST)(NODISCARD)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_CONST
#define __UTL_ATTRIBUTE_GETTER (ALWAYS_INLINE)(NODISCARD) __UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_GETTER

UTL_NAMESPACE_BEGIN

template <typename T, typename E>
class __UTL_PUBLIC_TEMPLATE expected :
    private details::expected::storage_base<expected<T, E>, T, E> {
    static_assert(UTL_TRAIT_is_destructible(T), "Invalid value type");
    static_assert(!UTL_TRAIT_is_reference(T), "Invalid value type");
    static_assert(!UTL_TRAIT_is_function(T), "Invalid value type");
    static_assert(!UTL_TRAIT_is_same(remove_cvref_t<T>, unexpect_t), "Invalid value type");
    static_assert(!__UTL_TRAIT_in_place_tag(remove_cvref_t<T>), "Invalid value type");
    static_assert(!__UTL_TRAIT_is_unexpected(remove_cvref_t<T>), "Invalid value type");
    static_assert(UTL_TRAIT_is_complete(unexpected<E>), "Invalid error type");
    using base_type = details::expected::storage_base<expected, T, E>;

    template <typename T1, typename E1, typename T1Qual, typename E1Qual>
    using can_convert = conjunction<is_constructible<T, T1Qual>, is_constructible<E, E1Qual>,
        conditional_t<!UTL_TRAIT_is_boolean(T),
            negation<disjunction<
                bool_constant<UTL_TRAIT_is_same(T, T1) && UTL_TRAIT_is_same(E, E1)>,
                is_constructible<T, expected<T1, E1>&>, is_constructible<T, expected<T1, E1>>,
                is_constructible<T, expected<T1, E1> const&>,
                is_constructible<T, expected<T1, E1> const>, is_convertible<expected<T1, E1>&, T>,
                is_convertible<expected<T1, E1>&&, T>, is_convertible<expected<T1, E1> const&, T>,
                is_convertible<expected<T1, E1> const&&, T>>>,
            true_type>,
        negation<disjunction<is_constructible<unexpected<E>, expected<T1, E1>&>,
            is_constructible<unexpected<E>, expected<T1, E1>>,
            is_constructible<unexpected<E>, expected<T1, E1> const&>,
            is_constructible<unexpected<E>, expected<T1, E1> const>>>>;

    template <typename U> /* __UTL details::is_unexpected_type_v<U> */
    using unexpected_error_type = decltype(__UTL declval<U>().error());

    template <typename T1, typename E1>
    friend class expected;

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;
    template <typename U>
    using rebind = expected<U, error_type>;

    template <typename U = is_implicit_constructible<T> UTL_CONSTRAINT_CXX11(U::value)>
    __UTL_HIDE_FROM_ABI inline constexpr expected() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T))
        : base_type(__UTL in_place) {}

    template <typename U = is_explicit_constructible<T>, int = 1 UTL_CONSTRAINT_CXX11(U::value)>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T))
        : base_type(__UTL in_place) {}

    __UTL_HIDE_FROM_ABI inline constexpr expected(expected const& other) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    template <typename T1, typename E1 UTL_CONSTRAINT_CXX11(UTL_TRAIT_conjunction(
        negation<conjunction<is_convertible<T1 const&, T>, is_convertible<E1 const&, E>>>,
        can_convert<T1, E1, T1 const&, E1 const&>))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(expected<T1, E1> const& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, T1 const&) &&
        UTL_TRAIT_is_nothrow_constructible(E, E1 const&))
        : base_type(__UTL details::expected::converting, other.has_value(), other.data_ref()) {}

    template <typename T1, typename E1 UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(is_convertible<T1 const&, T>, is_convertible<E1 const&, E>,
            can_convert<T1, E1, T1 const&, E1 const&>))>
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected<T1, E1> const& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, T1 const&) &&
        UTL_TRAIT_is_nothrow_constructible(E, E1 const&))
        : base_type(__UTL details::expected::converting, other.has_value(), other.data_ref()) {}

    template <typename T1, typename E1 UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(negation<conjunction<is_convertible<T1, T>, is_convertible<E1, E>>>,
            can_convert<T1, E1, T1, E1>))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(expected<T1, E1>&& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, T1) && UTL_TRAIT_is_nothrow_constructible(E, E1))
        : base_type(__UTL details::expected::converting, other.has_value(),
              __UTL move(other.data_ref())) {}

    template <typename T1, typename E1 UTL_CONSTRAINT_CXX11(UTL_TRAIT_conjunction(
        is_convertible<T1, T>, is_convertible<E1, E>, can_convert<T1, E1, T1, E1>))>
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected<T1, E1>&& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, T1) && UTL_TRAIT_is_nothrow_constructible(E, E1))
        : base_type(__UTL details::expected::converting, other.has_value(),
              __UTL move(other.data_ref())) {}

    template <typename U = T UTL_CONSTRAINT_CXX11(!__UTL_TRAIT_in_place_tag(remove_cvref_t<U>) &&
        !UTL_TRAIT_is_same(expected, remove_cvref_t<U>) && UTL_TRAIT_is_constructible(T, U) &&
        !__UTL_TRAIT_is_unexpected(remove_cvref_t<U>) && UTL_TRAIT_is_convertible(U, T) &&
        (!UTL_TRAIT_is_boolean(T) || !__UTL_TRAIT_is_expected(remove_cvref_t<U>)))>
    __UTL_HIDE_FROM_ABI inline constexpr expected(U&& value) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, U))
        : base_type(__UTL in_place, __UTL forward<U>(value)) {}

    template <typename U = T UTL_CONSTRAINT_CXX11(!__UTL_TRAIT_in_place_tag(remove_cvref_t<U>) &&
        !UTL_TRAIT_is_same(expected, remove_cvref_t<U>) && UTL_TRAIT_is_constructible(T, U) &&
        !__UTL_TRAIT_is_unexpected(remove_cvref_t<U>) && !UTL_TRAIT_is_convertible(U, T) &&
        (!UTL_TRAIT_is_boolean(T) || !__UTL_TRAIT_is_expected(remove_cvref_t<U>)))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(U&& value) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, U))
        : base_type(__UTL in_place, __UTL forward<U>(value)) {}

    template <typename U UTL_CONSTRAINT_CXX11(__UTL_TRAIT_is_unexpected(remove_cvref_t<U>) &&
        UTL_TRAIT_is_constructible(E, unexpected_error_type<U>) &&
        UTL_TRAIT_is_convertible(unexpected_error_type<U>, E))>
    __UTL_HIDE_FROM_ABI inline constexpr expected(U&& e) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, unexpected_error_type<U>))
        : base_type(__UTL unexpect, __UTL forward_like<U>(e.error())) {}

    template <typename U UTL_CONSTRAINT_CXX11(__UTL_TRAIT_is_unexpected(remove_cvref_t<U>) &&
        UTL_TRAIT_is_constructible(E, unexpected_error_type<U>) &&
        !UTL_TRAIT_is_convertible(unexpected_error_type<U>, E))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(U&& e) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, unexpected_error_type<U>))
        : base_type(__UTL unexpect, __UTL forward_like<U>(e.error())) {}

    template <typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(T, Args...))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(in_place_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...))
        : base_type(__UTL in_place, __UTL forward<Args>(args)...) {}

    template <typename U, typename... Args UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_constructible(T, ::std::initializer_list<U>&, Args...))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(in_place_t,
        ::std::initializer_list<U> il,
        Args&&... args) noexcept(UTL_TRAIT_is_nothrow_constructible(T, ::std::initializer_list<U>&,
        Args...))
        : base_type(__UTL in_place, il, __UTL forward<Args>(args)...) {}

    template <typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(E, Args...))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(unexpect_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...))
        : base_type(__UTL unexpect, __UTL forward<Args>(args)...) {}

    template <typename U, typename... Args UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_constructible(E, ::std::initializer_list<U>&, Args...))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(unexpect_t,
        ::std::initializer_list<U> il,
        Args&&... args) noexcept(UTL_TRAIT_is_nothrow_constructible(E, ::std::initializer_list<U>&,
        Args...))
        : base_type(__UTL unexpect, il, __UTL forward<Args>(args)...) {}

    __UTL_HIDE_FROM_ABI inline ~expected() noexcept = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected& operator=(expected const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(T) &&
        UTL_TRAIT_is_nothrow_copy_assignable(E)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected& operator=(expected&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(T) &&
        UTL_TRAIT_is_nothrow_move_assignable(E)) = default;

    template <typename U = T UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_same(expected, remove_cvref_t<U>) &&
        !__UTL_TRAIT_is_unexpected(remove_cvref_t<U>) && UTL_TRAIT_is_constructible(T, U) &&
        UTL_TRAIT_is_assignable(T&, U) &&
        (UTL_TRAIT_is_nothrow_constructible(T, U) || UTL_TRAIT_is_nothrow_move_constructible(T) ||
            UTL_TRAIT_is_nothrow_move_constructible(E)))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected& operator=(U&& u) noexcept(
        UTL_TRAIT_is_nothrow_assignable(T&, U) && UTL_TRAIT_is_nothrow_constructible(T, U)) {
        if (has_value()) {
            this->value_ref() = __UTL forward<U>(u);
        } else {
            this->reinitialize_as_value(__UTL forward<U>(u));
        }

        return *this;
    }

    template <typename U UTL_CONSTRAINT_CXX11(__UTL_TRAIT_is_unexpected(remove_cvref_t<U>) &&
        UTL_TRAIT_is_constructible(E, unexpected_error_type<U>) &&
        UTL_TRAIT_is_assignable(E&, unexpected_error_type<U>) &&
        (UTL_TRAIT_is_nothrow_constructible(E, unexpected_error_type<U>) ||
            UTL_TRAIT_is_nothrow_move_constructible(T) ||
            UTL_TRAIT_is_nothrow_move_constructible(E)))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected& operator=(U&& u) noexcept(
        UTL_TRAIT_is_nothrow_assignable(E&, unexpected_error_type<U>) &&
        UTL_TRAIT_is_nothrow_constructible(E, unexpected_error_type<U>)) {
        if (!has_value()) {
            this->error_ref() = __UTL forward_like<U>(u.error());
        } else {
            this->reinitialize_as_error(__UTL forward_like<U>(u.error()));
        }

        return *this;
    }

    using base_type::has_value;

    UTL_ATTRIBUTE(GETTER) inline constexpr T const& value() const& UTL_THROWS {
        static_assert(UTL_TRAIT_is_copy_constructible(E), "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
        return this->value_ref();
    }
    UTL_ATTRIBUTE(GETTER) inline UTL_CONSTEXPR_CXX14 T& value() & UTL_THROWS {
        static_assert(UTL_TRAIT_is_copy_constructible(E), "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
        return this->value_ref();
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr T const&& value() const&& UTL_THROWS {
        static_assert(UTL_TRAIT_is_copy_constructible(E) ||
                UTL_TRAIT_is_constructible(E, decltype(__UTL move(this->error_ref()))),
            "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
        return __UTL move(this->value_ref());
    }
    UTL_ATTRIBUTE(GETTER) inline UTL_CONSTEXPR_CXX14 T&& value() && UTL_THROWS {
        static_assert(UTL_TRAIT_is_copy_constructible(E) ||
                UTL_TRAIT_is_constructible(E, decltype(__UTL move(this->error_ref()))),
            "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
        return __UTL move(this->value_ref());
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E const& error() const& noexcept {
        return UTL_CONSTANT_P(this->error_ref()) ? this->error_ref()
                                                 : (UTL_ASSERT(!has_value()), this->error_ref());
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline UTL_CONSTEXPR_CXX14 E& error() & noexcept {
        if (!UTL_CONSTANT_P(this->error_ref())) {
            UTL_ASSERT(!has_value());
        }

        return this->error_ref();
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E const&& error() const&& noexcept {
        return UTL_CONSTANT_P(this->error_ref())
            ? __UTL move(this->error_ref())
            : (UTL_ASSERT(!has_value()), __UTL move(this->error_ref()));
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline UTL_CONSTEXPR_CXX14 E&& error() && noexcept {
        if (!UTL_CONSTANT_P(this->error_ref())) {
            UTL_ASSERT(!has_value());
        }

        return this->error_ref();
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline UTL_CONSTEXPR_CXX14 T* operator->() noexcept {
        if (!UTL_CONSTANT_P(this->value_ref())) {
            UTL_ASSERT(has_value());
        }
        return this->value_ptr();
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T const* operator->() const noexcept {
        return UTL_CONSTANT_P(this->value_ref()) ? this->value_ptr()
                                                 : (UTL_ASSERT(has_value()), this->value_ptr());
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline UTL_CONSTEXPR_CXX14 T& operator*() & noexcept {
        if (!UTL_CONSTANT_P(this->value_ref())) {
            UTL_ASSERT(has_value());
        }
        return this->value_ref();
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline UTL_CONSTEXPR_CXX14 T&& operator*() && noexcept {
        if (!UTL_CONSTANT_P(this->value_ref())) {
            UTL_ASSERT(has_value());
        }
        return __UTL move(this->value_ref());
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T const& operator*() const& noexcept {
        return UTL_CONSTANT_P(this->value_ref()) ? this->value_ref()
                                                 : (UTL_ASSERT(has_value()), this->value_ref());
    }
    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T const&& operator*() const&& noexcept {
        return UTL_CONSTANT_P(this->value_ref())
            ? __UTL move(this->value_ref())
            : (UTL_ASSERT(has_value()), __UTL move(this->value_ref()));
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_PURE) inline constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_copy_constructible(T) && UTL_TRAIT_is_convertible(U, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr T value_or(U&& default_val) const& noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, T)) {
        return has_value() ? **this : static_cast<T>(__UTL forward<U>(default_val));
    }

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_move_constructible(T) && UTL_TRAIT_is_convertible(U, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 T value_or(U&& default_val) && noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, T)) {
        return has_value() ? __UTL move(**this) : static_cast<T>(__UTL forward<U>(default_val));
    }

    template <typename U = E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_copy_constructible(E) && UTL_TRAIT_is_convertible(U, E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr E error_or(U&& default_val) const& noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, E)) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val)) : this->error_ref();
    }

    template <typename U = E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_move_constructible(E) && UTL_TRAIT_is_convertible(U, E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 E error_or(U&& default_val) && noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, E)) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val))
                           : __UTL move(this->error_ref());
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T&) && UTL_TRAIT_is_constructible(E, E&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto and_then(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&) && UTL_TRAIT_is_nothrow_constructible(E, E&))
        -> invoke_result_t<F, T&> {
        using return_type = remove_cvref_t<invoke_result_t<F, T&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");
        if (has_value()) {
            return __UTL invoke(__UTL forward<F>(f), this->value_ref());
        }

        return return_type{__UTL unexpect, this->error_ref()};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const&) && UTL_TRAIT_is_constructible(E, E const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&) &&
        UTL_TRAIT_is_nothrow_constructible(E, E const&)) -> invoke_result_t<F, T const&> {
        using return_type = remove_cvref_t<invoke_result_t<F, T const&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        return has_value() ? __UTL invoke(__UTL forward<F>(f), this->value_ref())
                           : return_type{__UTL unexpect, this->error_ref()};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T) && UTL_TRAIT_is_constructible(E, E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto and_then(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T) && UTL_TRAIT_is_nothrow_constructible(E, E))
        -> invoke_result_t<F, T> {
        using return_type = remove_cvref_t<invoke_result_t<F, T>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        if (has_value()) {
            return __UTL invoke(__UTL forward<F>(f), __UTL move(this->value_ref()));
        }

        return return_type{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const) && UTL_TRAIT_is_constructible(E, E const))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const) &&
        UTL_TRAIT_is_nothrow_constructible(E, E const)) -> invoke_result_t<F, T const> {
        using return_type = remove_cvref_t<invoke_result_t<F, T const>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        return has_value() ? __UTL invoke(__UTL forward<F>(f), __UTL move(this->value_ref()))
                           : return_type{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T&) && UTL_TRAIT_is_constructible(E, E&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform(F&& f) & UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F, T&>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T&>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T&>)),
            "Invalid return type");
        if (has_value()) {
            return transform_impl<return_type>(__UTL forward<F>(f));
        }

        return expected<return_type, E>{__UTL unexpect, this->error_ref()};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const&) && UTL_TRAIT_is_constructible(E, E const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform(F&& f) const& UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F, T const&>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T const&>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T const&>)),
            "Invalid return type");

        if (has_value()) {
            return transform_impl<return_type>(__UTL forward<F>(f));
        }

        return expected<return_type, E>{__UTL unexpect, this->error_ref()};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T) && UTL_TRAIT_is_constructible(E, E&&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform(F&& f) && UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F, T>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T>)),
            "Invalid return type");

        if (has_value()) {
            return __UTL move(*this).template transform_impl<return_type>(__UTL forward<F>(f));
        }

        return expected<return_type, E>{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const) && UTL_TRAIT_is_constructible(E, E const))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform(F&& f) const&& UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F, T const>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T const>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T const>)),
            "Invalid return type");

        if (has_value()) {
            return __UTL move(*this).template transform_impl<return_type>(__UTL forward<F>(f));
        }

        return expected<return_type, E>{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E&) && UTL_TRAIT_is_constructible(T, T&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto or_else(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E&) && UTL_TRAIT_is_nothrow_constructible(T, T&))
        -> invoke_result_t<F, E&> {
        using return_type = remove_cvref_t<invoke_result_t<F, E&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::value_type, T),
            "Invalid return type");
        if (has_value()) {
            return return_type{__UTL in_place, this->value_ref()};
        }

        return __UTL invoke(__UTL forward<F>(f), this->error_ref());
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E const&) && UTL_TRAIT_is_constructible(T, T const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E const&) &&
        UTL_TRAIT_is_nothrow_constructible(T, T const&)) -> invoke_result_t<F, E const&> {
        using return_type = remove_cvref_t<invoke_result_t<F, E const&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::value_type, T),
            "Invalid return type");

        return has_value() ? return_type{__UTL in_place, this->value_ref()}
                           : __UTL
                             invoke(__UTL forward<F>(f), this->error_ref());
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E) && UTL_TRAIT_is_constructible(T, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto or_else(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E) && UTL_TRAIT_is_nothrow_constructible(T, T))
        -> invoke_result_t<F, E> {
        using return_type = remove_cvref_t<invoke_result_t<F, E>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::value_type, T),
            "Invalid return type");

        if (has_value()) {
            return return_type{__UTL in_place, __UTL move(this->value_ref())};
        }

        return __UTL invoke(__UTL forward<F>(f), __UTL move(this->error_ref()));
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E const) && UTL_TRAIT_is_constructible(T, T const))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E const) &&
        UTL_TRAIT_is_nothrow_constructible(T, T const)) -> invoke_result_t<F, E const> {
        using return_type = remove_cvref_t<invoke_result_t<F, E const>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::value_type, T),
            "Invalid return type");

        return has_value() ? return_type{__UTL in_place, __UTL move(this->value_ref())}
                           : __UTL
                             invoke(__UTL forward<F>(f), __UTL move(this->error_ref()));
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E&) && UTL_TRAIT_is_constructible(T, T&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform_error(F&& f) & UTL_THROWS
        -> expected<T, remove_cv_t<invoke_result_t<F, E&>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E&>>;
        static_assert(UTL_TRAIT_is_complete(expected<T, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E&>),
            "Invalid return type");
        if (has_value()) {
            return expected<T, return_type>{__UTL in_place, this->value_ref()};
        }

        return expected<return_type, E>{
            __UTL details::expected::transforming_error, __UTL forward<F>(f), this->error_ref()};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E const&) && UTL_TRAIT_is_constructible(T, T const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) const& UTL_THROWS
        -> expected<T, remove_cv_t<invoke_result_t<F, E const&>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E const&>>;
        static_assert(UTL_TRAIT_is_complete(expected<T, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E const&>),
            "Invalid return type");

        return has_value() ? expected<T, return_type>{__UTL in_place, this->value_ref()}
                           : expected<T, return_type>(__UTL details::expected::transforming_error,
                                 __UTL forward<F>(f), this->error_ref());
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E) && UTL_TRAIT_is_constructible(T, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform_error(F&& f) && UTL_THROWS
        -> expected<T, remove_cv_t<invoke_result_t<F, E>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E>>;
        static_assert(UTL_TRAIT_is_complete(expected<T, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E>),
            "Invalid return type");

        if (has_value()) {
            return expected<T, return_type>{__UTL in_place, __UTL move(this->value_ref())};
        }

        return expected<T, return_type>{__UTL details::expected::transforming_error,
            __UTL forward<F>(f), __UTL move(this->error_ref())};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E const) && UTL_TRAIT_is_constructible(T, T const))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) const&& UTL_THROWS
        -> expected<T, remove_cv_t<invoke_result_t<F, E const>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E const>>;
        static_assert(UTL_TRAIT_is_complete(expected<T, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E const>),
            "Invalid return type");

        return has_value() ? expected<T, return_type>{__UTL in_place, __UTL move(this->value_ref())}
                           : expected<T, return_type>{__UTL details::expected::transforming_error,
                                 __UTL forward<F>(f), __UTL move(this->error_ref())};
    }

    template <typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_nothrow_constructible(T, Args...))>
    __UTL_HIDE_FROM_ABI inline T& emplace(Args&&... args) noexcept {
        return this->emplace_value(__UTL forward<Args>(args)...);
    }

    template <typename U, typename... Args UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_nothrow_constructible(T, ::std::initializer_list<U>&, Args...))>
    __UTL_HIDE_FROM_ABI inline T& emplace(::std::initializer_list<U> il, Args&&... args) noexcept {
        return this->emplace_value(il, __UTL forward<Args>(args)...);
    }

    using base_type::swap;

    template <typename T2, typename E2 UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(T2))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(expected const& left,
        expected<T2, E2> const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(T,
                                                    T2) &&
        UTL_TRAIT_is_nothrow_equality_comparable_with(E, E2)) {
        return left.has_value() == right.has_value() &&
            (left.has_value() ? left.value_ref() == right.value()
                              : left.error_ref() == right.error());
    }

    template <typename T2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(expected const& left,
        T2 const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(T, T2)) {
        static_assert(UTL_TRAIT_is_equality_comparable_with(T, T2), "Program ill-formed");
        static_assert(UTL_TRAIT_is_boolean_testable(decltype(left.value_ref() == right)),
            "Program ill-formed");
        return left.has_value() && left.value_ref() == right;
    }

    template <typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(expected const& left,
        unexpected<E2> const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(E,
        E2)) {
        static_assert(UTL_TRAIT_is_equality_comparable_with(E, E2), "Program ill-formed");
        static_assert(UTL_TRAIT_is_boolean_testable(decltype(left.error_ref() == right.error())),
            "Program ill-formed");
        return !left.has_value() && left.error_ref() == right.error();
    }

    template <typename T2, typename E2 UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(T2))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator!=(expected const& left,
        expected<T2, E2> const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(T,
                                                    T2) &&
        UTL_TRAIT_is_nothrow_equality_comparable_with(E, E2)) {
        return !(left == right);
    }

    template <typename T2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator!=(expected const& left,
        T2 const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(T, T2)) {
        return !(left == right);
    }

    template <typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator!=(expected const& left,
        unexpected<E2> const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(E,
        E2)) {
        return !(left == right);
    }

private:
#if UTL_CXX17
    template <typename R, typename F>
    __UTL_HIDE_FROM_ABI inline constexpr expected<R, E> transform_impl(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&)) {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_void(R)) {
            __UTL invoke(__UTL forward<F>(f), this->value_ref());
            return expected<R, E>{};
        } else {
            return expected<R, E>{
                __UTL details::expected::transforming, __UTL forward<F>(f), this->value_ref()};
        }
    }
    template <typename R, typename F>
    __UTL_HIDE_FROM_ABI inline constexpr expected<R, E> transform_impl(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&)) {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_void(R)) {
            __UTL invoke(__UTL forward<F>(f), this->value_ref());
            return expected<R, E>{};
        } else {
            return expected<R, E>{
                __UTL details::expected::transforming, __UTL forward<F>(f), this->value_ref()};
        }
    }
    template <typename R, typename F>
    __UTL_HIDE_FROM_ABI inline constexpr expected<R, E> transform_impl(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&&)) {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_void(R)) {
            __UTL invoke(__UTL forward<F>(f), __UTL move(this->value_ref()));
            return expected<R, E>{};
        } else {
            return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f),
                __UTL move(this->value_ref())};
        }
    }
    template <typename R, typename F>
    __UTL_HIDE_FROM_ABI inline constexpr expected<R, E> transform_impl(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&&)) {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_void(R)) {
            __UTL invoke(__UTL forward<F>(f), __UTL move(this->value_ref()));
            return expected<R, E>{};
        } else {
            return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f),
                __UTL move(this->value_ref())};
        }
    }
#else  // UTL_CXX17
    template <typename R, typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&)) {
        UTL_ASSERT(has_value());
        __UTL invoke(__UTL forward<F>(f), this->value_ref());
        return expected<R, E>{};
    }
    template <typename R, typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&)) {
        UTL_ASSERT(has_value());
        __UTL invoke(__UTL forward<F>(f), this->value_ref());
        return expected<R, E>{};
    }
    template <typename R, typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&&)) {
        UTL_ASSERT(has_value());
        __UTL invoke(__UTL forward<F>(f), __UTL move(this->value_ref()));
        return expected<R, E>{};
    }
    template <typename R, typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&&)) {
        UTL_ASSERT(has_value());
        __UTL invoke(__UTL forward<F>(f), __UTL move(this->value_ref()));
        return expected<R, E>{};
    }

    template <typename R, typename F UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&)) {
        UTL_ASSERT(has_value());
        return expected<R, E>{
            __UTL details::expected::transforming, __UTL forward<F>(f), this->value_ref()};
    }

    template <typename R, typename F UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&)) {
        UTL_ASSERT(has_value());
        return expected<R, E>{
            __UTL details::expected::transforming, __UTL forward<F>(f), this->value_ref()};
    }

    template <typename R, typename F UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T&&)) {
        UTL_ASSERT(has_value());
        return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f),
            __UTL move(this->value_ref())};
    }

    template <typename R, typename F UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, T const&&)) {
        UTL_ASSERT(has_value());
        return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f),
            __UTL move(this->value_ref())};
    }
#endif // UTL_CXX17
};

template <typename E>
class __UTL_PUBLIC_TEMPLATE expected<void, E> :
    __UTL details::expected::void_storage_base<expected<void, E>, E> {
    static_assert(UTL_TRAIT_is_complete(unexpected<E>), "Invalid error type");
    using base_type = details::expected::void_storage_base<expected, E>;

    template <typename T1, typename E1, typename E1Qual>
    using can_convert = conjunction<is_void<T1>, is_constructible<E, E1Qual>,
        negation<disjunction<is_constructible<unexpected<E>, expected<T1, E1>&>,
            is_constructible<unexpected<E>, expected<T1, E1>>,
            is_constructible<unexpected<E>, expected<T1, E1> const&>,
            is_constructible<unexpected<E>, expected<T1, E1> const>>>>;

    template <typename U> /* __UTL details::is_unexpected_type_v<U> */
    using unexpected_error_type = decltype(__UTL declval<U>().error());

    template <typename T1, typename E1>
    friend class expected;

public:
    __UTL_HIDE_FROM_ABI inline constexpr expected() noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected const& other) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;

    template <typename T1, typename E1 UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(is_convertible<E1 const&, E>, can_convert<T1, E1, E1 const&>))>
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected<T1, E1> const& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, E1 const&))
        : base_type(__UTL details::expected::converting, other.has_value(), other.data_ref()) {}

    template <typename T1, typename E1 UTL_CONSTRAINT_CXX11(UTL_TRAIT_conjunction(
        negation<is_convertible<E1 const&, E>>, can_convert<T1, E1, E1 const&>))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(expected<T1, E1> const& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, E1 const&))
        : base_type(__UTL details::expected::converting, other.has_value(), other.data_ref()) {}

    template <typename T1, typename E1 UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(is_convertible<E1, E>, can_convert<T1, E1, E1>))>
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected<T1, E1>&& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, E1))
        : base_type(__UTL details::expected::converting, other.has_value(),
              __UTL move(other.data_ref())) {}

    template <typename T1, typename E1 UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(negation<is_convertible<E1, E>>, can_convert<T1, E1, E1>))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(expected<T1, E1>&& other) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, E1))
        : base_type(__UTL details::expected::converting, other.has_value(),
              __UTL move(other.data_ref())) {}

    template <typename U UTL_CONSTRAINT_CXX11(__UTL_TRAIT_is_unexpected(remove_cvref_t<U>) &&
        UTL_TRAIT_is_constructible(E, unexpected_error_type<U>) &&
        UTL_TRAIT_is_convertible(unexpected_error_type<U>, E))>
    __UTL_HIDE_FROM_ABI inline constexpr expected(U&& e) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, unexpected_error_type<U>))
        : base_type(__UTL unexpect, __UTL forward_like<U>(e.error())) {}

    template <typename U UTL_CONSTRAINT_CXX11(__UTL_TRAIT_is_unexpected(remove_cvref_t<U>) &&
        UTL_TRAIT_is_constructible(E, unexpected_error_type<U>) &&
        !UTL_TRAIT_is_convertible(unexpected_error_type<U>, E))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(U&& e) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, unexpected_error_type<U>))
        : base_type(__UTL unexpect, __UTL forward_like<U>(e.error())) {}

    __UTL_HIDE_FROM_ABI constexpr explicit expected(in_place_t) noexcept
        : base_type(__UTL in_place) {}

    template <typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(E, Args...))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(unexpect_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...))
        : base_type(__UTL unexpect, __UTL forward<Args>(args)...) {}

    template <typename U, typename... Args UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_constructible(E, ::std::initializer_list<U>&, Args...))>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(unexpect_t,
        ::std::initializer_list<U> il,
        Args&&... args) noexcept(UTL_TRAIT_is_nothrow_constructible(E, ::std::initializer_list<U>&,
        Args...))
        : base_type(__UTL unexpect, il, __UTL forward<Args>(args)...) {}

    __UTL_HIDE_FROM_ABI inline ~expected() noexcept = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected& operator=(expected const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(E)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected& operator=(expected&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(E)) = default;

    template <typename U UTL_CONSTRAINT_CXX11(__UTL_TRAIT_is_unexpected(remove_cvref_t<U>) &&
        UTL_TRAIT_is_constructible(E, unexpected_error_type<U>) &&
        UTL_TRAIT_is_assignable(E&, unexpected_error_type<U>) &&
        (UTL_TRAIT_is_nothrow_constructible(E, unexpected_error_type<U>) ||
            UTL_TRAIT_is_nothrow_move_constructible(E)))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected& operator=(U&& u) noexcept(
        UTL_TRAIT_is_nothrow_assignable(E&, unexpected_error_type<U>)) {
        if (!has_value()) {
            this->error_ref() = __UTL forward_like<U>(u.error());
        } else {
            this->reinitialize_as_error(__UTL forward_like<U>(u.error()));
        }

        return *this;
    }

    using base_type::has_value;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 void value() const& UTL_THROWS {
        static_assert(UTL_TRAIT_is_copy_constructible(E), "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 void value() && UTL_THROWS {
        static_assert(UTL_TRAIT_is_copy_constructible(E) ||
                UTL_TRAIT_is_constructible(E, decltype(__UTL move(this->error_ref()))),
            "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E const& error() const& noexcept {
        UTL_ASSERT(!has_value());
        return this->error_ref();
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline UTL_CONSTEXPR_CXX14 E& error() & noexcept {
        UTL_ASSERT(!has_value());
        return this->error_ref();
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E const&& error() const&& noexcept {
        UTL_ASSERT(!has_value());
        return __UTL move(this->error_ref());
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline UTL_CONSTEXPR_CXX14 E&& error() && noexcept {
        UTL_ASSERT(!has_value());
        return __UTL move(this->error_ref());
    }

    __UTL_HIDE_FROM_ABI inline constexpr void operator*() const noexcept {}

    UTL_ATTRIBUTE(EXPECTED_INLINE_PURE) inline constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    template <typename U = E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_copy_constructible(E) && UTL_TRAIT_is_convertible(U, E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr E error_or(U&& default_val) const& noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, E)) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val)) : this->error_ref();
    }

    template <typename U = E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_move_constructible(E) && UTL_TRAIT_is_convertible(U, E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 E error_or(U&& default_val) && noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, E)) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val))
                           : __UTL move(this->error_ref());
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F) && UTL_TRAIT_is_constructible(E, E&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto and_then(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F)) -> invoke_result_t<F> {
        using return_type = remove_cvref_t<invoke_result_t<F>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");
        if (has_value()) {
            return __UTL invoke(__UTL forward<F>(f));
        }

        return return_type{__UTL unexpect, this->error_ref()};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F) && UTL_TRAIT_is_constructible(E, E const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F)) -> invoke_result_t<F> {
        using return_type = remove_cvref_t<invoke_result_t<F>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        return has_value() ? __UTL invoke(__UTL forward<F>(f))
                           : return_type{__UTL unexpect, this->error_ref()};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F) && UTL_TRAIT_is_constructible(E, E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto and_then(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F) && UTL_TRAIT_is_nothrow_constructible(E, E))
        -> invoke_result_t<F> {
        using return_type = remove_cvref_t<invoke_result_t<F>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        if (has_value()) {
            return __UTL invoke(__UTL forward<F>(f));
        }

        return return_type{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F) && UTL_TRAIT_is_constructible(E, E const))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F) && UTL_TRAIT_is_nothrow_constructible(E, E const))
        -> invoke_result_t<F> {
        using return_type = remove_cvref_t<invoke_result_t<F>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        return has_value() ? __UTL invoke(__UTL forward<F>(f))
                           : return_type{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F) && UTL_TRAIT_is_constructible(E, E&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform(F&& f) & UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F>)),
            "Invalid return type");
        if (has_value()) {
            transform_impl<return_type>(__UTL forward<F>(f));
        }

        return expected<return_type, E>{__UTL unexpect, this->error_ref()};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F) && UTL_TRAIT_is_constructible(E, E const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform(F&& f) const& UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F>)),
            "Invalid return type");

        if (has_value()) {
            transform_impl<return_type>(__UTL forward<F>(f));
        }

        return expected<return_type, E>{__UTL unexpect, this->error_ref()};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F) && UTL_TRAIT_is_constructible(E, E&&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform(F&& f) && UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F>)),
            "Invalid return type");

        if (has_value()) {
            __UTL move(*this).template transform_impl<return_type>(__UTL forward<F>(f));
        }

        return expected<return_type, E>{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F) && UTL_TRAIT_is_constructible(E, E const))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform(F&& f) const&& UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F>)),
            "Invalid return type");

        if (has_value()) {
            __UTL move(*this).template transform_impl<return_type>(__UTL forward<F>(f));
        }

        return expected<return_type, E>{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_invocable(F, E&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto or_else(F&& f) & noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E&)) -> invoke_result_t<F, E&> {
        using return_type = remove_cvref_t<invoke_result_t<F, E&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_void(typename return_type::value_type),
            "Invalid return type");

        if (has_value()) {
            return return_type{__UTL in_place};
        }

        return __UTL invoke(__UTL forward<F>(f), this->error_ref());
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) const& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E const&)) -> invoke_result_t<F, E const&> {
        using return_type = remove_cvref_t<invoke_result_t<F, E const&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_void(typename return_type::value_type),
            "Invalid return type");

        return has_value() ? return_type{__UTL in_place}
                           : __UTL
                             invoke(__UTL forward<F>(f), this->error_ref());
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto or_else(F&& f) && noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E)) -> invoke_result_t<F, E> {
        using return_type = remove_cvref_t<invoke_result_t<F, E>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_void(typename return_type::value_type),
            "Invalid return type");

        if (has_value()) {
            return return_type{__UTL in_place};
        }

        return __UTL invoke(__UTL forward<F>(f), __UTL move(this->error_ref()));
    }

    template <typename F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, E const))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) const&& noexcept(
        UTL_TRAIT_is_nothrow_invocable(F, E const)) -> invoke_result_t<F, E const> {
        using return_type = remove_cvref_t<invoke_result_t<F, E const>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_void(typename return_type::value_type),
            "Invalid return type");

        return has_value() ? return_type{__UTL in_place}
                           : __UTL
                             invoke(__UTL forward<F>(f), __UTL move(this->error_ref()));
    }

    template <typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_invocable(F, E&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform_error(F&& f) & UTL_THROWS
        -> expected<void, remove_cv_t<invoke_result_t<F, E&>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E&>>;
        static_assert(UTL_TRAIT_is_complete(expected<void, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E&>),
            "Invalid return type");
        if (has_value()) {
            return expected<void, return_type>{__UTL in_place};
        }

        return expected<void, return_type>{
            __UTL details::expected::transforming_error, __UTL forward<F>(f), this->error_ref()};
    }

    template <typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_invocable(F, E const&))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) const& UTL_THROWS
        -> expected<void, remove_cv_t<invoke_result_t<F, E const&>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E const&>>;
        static_assert(UTL_TRAIT_is_complete(expected<void, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E const&>),
            "Invalid return type");

        return has_value()
            ? expected<void, return_type>{__UTL in_place}
            : expected<void, return_type>(__UTL details::expected::transforming_error,
                  __UTL forward<F>(f), this->error_ref());
    }

    template <typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_invocable(F, E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 auto transform_error(F&& f) && UTL_THROWS
        -> expected<void, remove_cv_t<invoke_result_t<F, E>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E>>;
        static_assert(UTL_TRAIT_is_complete(expected<void, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E>),
            "Invalid return type");

        if (has_value()) {
            return expected<void, return_type>{__UTL in_place};
        }

        return expected<void, return_type>{__UTL details::expected::transforming_error,
            __UTL forward<F>(f), __UTL move(this->error_ref())};
    }

    template <typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_invocable(F, E const))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) const&& UTL_THROWS
        -> expected<void, remove_cv_t<invoke_result_t<F, E const>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E const>>;
        static_assert(UTL_TRAIT_is_complete(expected<void, return_type>) &&
                UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, E const>),
            "Invalid return type");

        return has_value()
            ? expected<void, return_type>{__UTL in_place}
            : expected<void, return_type>{__UTL details::expected::transforming_error,
                  __UTL forward<F>(f), __UTL move(this->error_ref())};
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void emplace() noexcept { return this->emplace_value(); }

    using base_type::swap;

    template <typename T2, typename E2 UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(T2))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(expected const& left,
        expected<T2, E2> const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(E,
        E2)) {
        static_assert(UTL_TRAIT_is_equality_comparable_with(E, E2), "Program ill-formed");
        return right.has_value() == right.has_value() &&
            (left.has_value() || left.error_ref() == right.error());
    }

    template <typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(expected const& left,
        unexpected<E2> const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(E,
        E2)) {
        static_assert(UTL_TRAIT_is_equality_comparable_with(E, E2), "Program ill-formed");
        return !left.has_value() && left.error_ref() == right.error();
    }

    template <typename T2, typename E2 UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(T2))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator!=(expected const& left,
        expected<T2, E2> const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(E,
        E2)) {
        return !(left == right);
    }

    template <typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator!=(expected const& left,
        unexpected<E2> const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(E,
        E2)) {
        return !(left == right);
    }

private:
#if UTL_CXX17
    template <typename R, typename F>
    __UTL_HIDE_FROM_ABI inline constexpr expected<R, E> transform_impl(F&& f) & UTL_THROWS {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_void(R)) {
            __UTL invoke(__UTL forward<F>(f));
            return expected<R, E>{};
        } else {
            return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f)};
        }
    }
    template <typename R, typename F>
    __UTL_HIDE_FROM_ABI inline constexpr expected<R, E> transform_impl(F&& f) const& UTL_THROWS {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_void(R)) {
            __UTL invoke(__UTL forward<F>(f));
            return expected<R, E>{};
        } else {
            return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f)};
        }
    }
    template <typename R, typename F>
    __UTL_HIDE_FROM_ABI inline constexpr expected<R, E> transform_impl(F&& f) && UTL_THROWS {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_void(R)) {
            __UTL invoke(__UTL forward<F>(f));
            return expected<R, E>{};
        } else {
            return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f)};
        }
    }
    template <typename R, typename F>
    __UTL_HIDE_FROM_ABI inline constexpr expected<R, E> transform_impl(F&& f) const&& UTL_THROWS {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_void(R)) {
            __UTL invoke(__UTL forward<F>(f));
            return expected<R, E>{};
        } else {
            return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f)};
        }
    }
#else  // UTL_CXX17
    template <typename R, typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) &
        UTL_THROWS {
        UTL_ASSERT(has_value());
        __UTL invoke(__UTL forward<F>(f));
        return expected<R, E>{};
    }
    template <typename R, typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) const&
        UTL_THROWS {
        UTL_ASSERT(has_value());
        __UTL invoke(__UTL forward<F>(f));
        return expected<R, E>{};
    }
    template <typename R, typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) &&
        UTL_THROWS {
        UTL_ASSERT(has_value());
        __UTL invoke(__UTL forward<F>(f));
        return expected<R, E>{};
    }
    template <typename R, typename F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) const&&
        UTL_THROWS {
        UTL_ASSERT(has_value());
        __UTL invoke(__UTL forward<F>(f));
        return expected<R, E>{};
    }

    template <typename R, typename F UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) &
        UTL_THROWS {
        UTL_ASSERT(has_value());
        return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f)};
    }

    template <typename R, typename F UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) const&
        UTL_THROWS {
        UTL_ASSERT(has_value());
        return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f)};
    }

    template <typename R, typename F UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) &&
        UTL_THROWS {
        UTL_ASSERT(has_value());
        return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f)};
    }

    template <typename R, typename F UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_void(R))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 expected<R, E> transform_impl(F&& f) const&&
        UTL_THROWS {
        UTL_ASSERT(has_value());
        return expected<R, E>{__UTL details::expected::transforming, __UTL forward<F>(f)};
    }
#endif // UTL_CXX17
};

template <typename E>
class __UTL_PUBLIC_TEMPLATE expected<void const, E> : private expected<void, E> {
public:
    using typename expected<void, E>::value_type;
    using typename expected<void, E>::error_type;
    using typename expected<void, E>::unexpected_type;
    template <typename U>
    using rebind = typename expected<void, E>::template rebind<U>;
    using expected<void, E>::expected;
    using expected<void, E>::operator=;
    using expected<void, E>::value;
    using expected<void, E>::error;
    using expected<void, E>::has_value;
    using expected<void, E>::operator bool;
    using expected<void, E>::operator*;
    using expected<void, E>::error_or;
    using expected<void, E>::emplace;
    using expected<void, E>::swap;
    using expected<void, E>::transform;
    using expected<void, E>::and_then;
    using expected<void, E>::or_else;
    using expected<void, E>::transform_error;
};
template <typename E>
class __UTL_PUBLIC_TEMPLATE expected<void volatile, E> : expected<void, E> {
public:
    using typename expected<void, E>::value_type;
    using typename expected<void, E>::error_type;
    using typename expected<void, E>::unexpected_type;
    template <typename U>
    using rebind = typename expected<void, E>::template rebind<U>;
    using expected<void, E>::expected;
    using expected<void, E>::operator=;
    using expected<void, E>::value;
    using expected<void, E>::error;
    using expected<void, E>::has_value;
    using expected<void, E>::operator bool;
    using expected<void, E>::operator*;
    using expected<void, E>::error_or;
    using expected<void, E>::emplace;
    using expected<void, E>::swap;
    using expected<void, E>::transform;
    using expected<void, E>::and_then;
    using expected<void, E>::or_else;
    using expected<void, E>::transform_error;
};
template <typename E>
class __UTL_PUBLIC_TEMPLATE expected<void const volatile, E> : expected<void, E> {
public:
    using typename expected<void, E>::value_type;
    using typename expected<void, E>::error_type;
    using typename expected<void, E>::unexpected_type;
    template <typename U>
    using rebind = typename expected<void, E>::template rebind<U>;
    using expected<void, E>::expected;
    using expected<void, E>::operator=;
    using expected<void, E>::value;
    using expected<void, E>::error;
    using expected<void, E>::has_value;
    using expected<void, E>::operator bool;
    using expected<void, E>::operator*;
    using expected<void, E>::error_or;
    using expected<void, E>::emplace;
    using expected<void, E>::swap;
    using expected<void, E>::transform;
    using expected<void, E>::and_then;
    using expected<void, E>::or_else;
    using expected<void, E>::transform_error;
};

UTL_NAMESPACE_END

#undef __UTL_ATTRIBUTE_GETTER
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_GETTER
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_PURE
#undef __UTL_ATTRIBUTE_EXPECTED_INLINE_PURE
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_CONST
#undef __UTL_ATTRIBUTE_EXPECTED_INLINE_CONST
