// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#if UTL_CXX20
#  include "utl_strong_ordering.h"
#endif

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
class pointer_comparable {
    using null_type = decltype(nullptr);
    friend constexpr bool operator==(T const& lhs, T const& rhs) noexcept {
        return lhs.get() == rhs.get();
    }

    friend constexpr bool operator==(T const& lhs, null_type) noexcept {
        return lhs.get() == nullptr;
    }

#if UTL_CXX20
    friend constexpr strong_ordering operator<=>(T const& lhs, T const& rhs) noexcept {
        return lhs.get() <=> rhs.get();
    }

    friend constexpr strong_ordering operator<=>(T const& lhs, null_type) noexcept {
        return lhs.get() <=> nullptr;
    }
#else
    /**
     * Comparison operators for pre-C++20 compilers
     */

    friend constexpr bool operator<(T const& lhs, T const& rhs) noexcept {
        return lhs.get() < rhs.get();
    }

    friend constexpr bool operator!=(T const& lhs, T const& rhs) noexcept { return !(lhs == rhs); }

    friend constexpr bool operator>(T const& lhs, T const& rhs) noexcept { return rhs < lhs; }

    friend constexpr bool operator>=(T const& lhs, T const& rhs) noexcept { return !(lhs < rhs); }

    friend constexpr bool operator<=(T const& lhs, T const& rhs) noexcept { return !(rhs < lhs); }

    friend constexpr bool operator==(null_type, T const& rhs) noexcept {
        return nullptr == rhs.get();
    }

    friend constexpr bool operator<(T const& lhs, null_type) noexcept {
        return lhs.get() < nullptr;
    }

    friend constexpr bool operator<(null_type, T const& rhs) noexcept {
        return nullptr < rhs.get();
    }

    friend constexpr bool operator!=(T const& lhs, null_type) noexcept {
        return lhs.get() != nullptr;
    }

    friend constexpr bool operator!=(null_type, T const& rhs) noexcept {
        return nullptr != rhs.get();
    }

    friend constexpr bool operator>(T const& lhs, null_type) noexcept { return nullptr < lhs; }

    friend constexpr bool operator>(null_type, T const& rhs) noexcept { return rhs < nullptr; }

    friend constexpr bool operator>=(T const& lhs, null_type) noexcept { return !(lhs < nullptr); }

    friend constexpr bool operator>=(null_type, T const& rhs) noexcept { return !(nullptr < rhs); }

    friend constexpr bool operator<=(T const& lhs, null_type) noexcept { return !(lhs > nullptr); }

    friend constexpr bool operator<=(null_type, T const& rhs) noexcept { return !(nullptr > rhs); }

#endif
};

UTL_NAMESPACE_END
