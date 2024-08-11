// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_default_constructible.h"
#include "utl/type_traits/utl_is_empty.h"
#include "utl/type_traits/utl_is_final.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace compressed_pair {

template <typename T0>
using optimizable UTL_NODEBUG = conjunction<is_empty<T0>, negation<is_final<T0>>>;

struct default_initialize_t {
    explicit constexpr default_initialize_t() noexcept = default;
};
struct value_initialize_t {
    explicit constexpr value_initialize_t() noexcept = default;
};

UTL_INLINE_CXX17 constexpr default_initialize_t default_initialize{};
UTL_INLINE_CXX17 constexpr value_initialize_t value_initialize{};

template <typename T, size_t Idx, typename = void>
struct element {
    using reference UTL_NODEBUG = T&;
    using const_reference UTL_NODEBUG = T const&;
    using value_type UTL_NODEBUG = T;
    UTL_HIDE_FROM_ABI constexpr element() noexcept(
        is_nothrow_default_constructible<T>::value) = default;
    UTL_HIDE_FROM_ABI constexpr element(default_initialize_t) noexcept(
        is_nothrow_default_constructible<T>::value)
        : element() {}
    UTL_HIDE_FROM_ABI constexpr element(value_initialize_t) noexcept(
        is_nothrow_default_constructible<T>::value)
        : value() {}
    template <typename... Args>
    UTL_HIDE_FROM_ABI constexpr element(Args&&... args) : value(forward<Args>(args)...) {}

    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr reference get() & noexcept { return value; }
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr const_reference get() const& noexcept {
        return value;
    }
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr value_type&& get() && noexcept {
        return move(value);
    }
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr value_type const&& get() const&& noexcept {
        return move(value);
    }

    T value;
};

template <typename T, size_t Idx>
struct UTL_ATTRIBUTE(EMPTY_BASES) element<T, Idx, enable_if_t<optimizable<T>::value>> : private T {
    using reference UTL_NODEBUG = T&;
    using const_reference UTL_NODEBUG = T const&;
    using value_type UTL_NODEBUG = T;

    UTL_HIDE_FROM_ABI constexpr element() noexcept(
        is_nothrow_default_constructible<T>::value) = default;
    UTL_HIDE_FROM_ABI constexpr element(default_initialize_t) noexcept(
        is_nothrow_default_constructible<T>::value){};
    UTL_HIDE_FROM_ABI constexpr element(value_initialize_t) noexcept(
        is_nothrow_default_constructible<T>::value)
        : value_type() {}
    template <typename... Args>
    UTL_HIDE_FROM_ABI constexpr element(Args&&... args) noexcept(
        is_nothrow_constructible<T, Args...>::value)
        : value_type(forward<Args>(args)...) {}

    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr reference get() & noexcept { return *this; }
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr const_reference get() const& noexcept {
        return *this;
    }
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr value_type&& get() && noexcept {
        return move(*this);
    }
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI) constexpr value_type const&& get() const&& noexcept {
        return move(*this);
    }
};

} // namespace compressed_pair
} // namespace details

template <typename T0, typename T1>
class UTL_ATTRIBUTE(EMPTY_BASES) compressed_pair :
    details::compressed_pair::element<T0, 0>,
    details::compressed_pair::element<T1, 1> {

    using first_base UTL_NODEBUG = details::compressed_pair::element<T0, 0>;
    using second_base UTL_NODEBUG = details::compressed_pair::element<T1, 1>;

public:
    using first_type UTL_NODEBUG = T0;
    using second_type UTL_NODEBUG = T1;

    UTL_HIDE_FROM_ABI constexpr compressed_pair() noexcept(
        is_nothrow_default_constructible<first_base>::value &&
        is_nothrow_default_constructible<second_base>::value) = default;

    template <typename U0, typename U1>
    UTL_HIDE_FROM_ABI constexpr compressed_pair(U0&& first, U1&& second) noexcept(
        is_nothrow_constructible<first_base, U0>::value &&
        is_nothrow_constructible<second_base, U1>::value)
        : first_base(forward<U0>(first))
        , second_base(forward<U1>(second)) {}

#define __UTL_DEFINE_GETTERS(NAME)                                                     \
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)                                    \
    constexpr NAME##_type& NAME()& noexcept { return NAME##_base::get(); }             \
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)                                    \
    constexpr NAME##_type const& NAME() const& noexcept { return NAME##_base::get(); } \
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)                                    \
    constexpr NAME##_type&& NAME()&& noexcept { return NAME##_base::get(); }           \
    UTL_ATTRIBUTES(NODISCARD, CONST, HIDE_FROM_ABI)                                    \
    constexpr NAME##_type const&& NAME() const&& noexcept { return NAME##_base::get(); }

    __UTL_DEFINE_GETTERS(first);
    __UTL_DEFINE_GETTERS(second);
#undef __UTL_DEFINE_GETTERS
};

UTL_NAMESPACE_END
