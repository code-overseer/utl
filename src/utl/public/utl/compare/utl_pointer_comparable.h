// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl_compare_fwd.h"

UTL_NAMESPACE_BEGIN

/**
 * CRTP (Curiously Recurring Template Pattern) base class providing comparison operators for
 * pointer-like types.
 *
 * This class template is intended to be used as a base class for pointer-like types to enable
 * comparison operations such as equality, ordering, and relational operations. It provides
 * comparison operators for comparing instances of the derived class with other instances and with
 * null pointers.
 *
 * @tparam T The derived pointer-like type.
 */
template <typename T>
class __UTL_PUBLIC_TEMPLATE pointer_comparable {
    using null_type UTL_NODEBUG = decltype(nullptr);

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator==(
        T const& lhs, T const& rhs) noexcept {
        return lhs.get() == rhs.get();
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator==(
        T const& lhs, null_type) noexcept {
        return lhs.get() == nullptr;
    }

#if UTL_CXX20
    template <same_as<T> U>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr auto operator<=>(
        T const& lhs, U const& rhs) noexcept {
        return lhs.get() <=> rhs.get();
    }

    template <same_as<null_type> U>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr auto operator<=>(
        T const& lhs, U) noexcept {
        return lhs.get() <=> nullptr;
    }
#endif
    /**
     * Comparison operators for pre-C++20 compilers
     */

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator<(
        T const& lhs, T const& rhs) noexcept {
        return lhs.get() < rhs.get();
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator!=(
        T const& lhs, T const& rhs) noexcept {
        return !(lhs == rhs);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator>(
        T const& lhs, T const& rhs) noexcept {
        return rhs < lhs;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator>=(
        T const& lhs, T const& rhs) noexcept {
        return !(lhs < rhs);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator<=(
        T const& lhs, T const& rhs) noexcept {
        return !(rhs < lhs);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator==(
        null_type, T const& rhs) noexcept {
        return nullptr == rhs.get();
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator<(
        T const& lhs, null_type) noexcept {
        return lhs.get() < nullptr;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator<(
        null_type, T const& rhs) noexcept {
        return nullptr < rhs.get();
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator!=(
        T const& lhs, null_type) noexcept {
        return lhs.get() != nullptr;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator!=(
        null_type, T const& rhs) noexcept {
        return nullptr != rhs.get();
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator>(
        T const& lhs, null_type) noexcept {
        return nullptr < lhs;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator>(
        null_type, T const& rhs) noexcept {
        return rhs < nullptr;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator>=(
        T const& lhs, null_type) noexcept {
        return !(lhs < nullptr);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator>=(
        null_type, T const& rhs) noexcept {
        return !(nullptr < rhs);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator<=(
        T const& lhs, null_type) noexcept {
        return !(lhs > nullptr);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr bool operator<=(
        null_type, T const& rhs) noexcept {
        return !(nullptr > rhs);
    }
};

UTL_NAMESPACE_END
