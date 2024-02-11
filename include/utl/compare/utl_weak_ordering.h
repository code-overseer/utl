// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/compare/utl_compare_fwd.h"
#include "utl/compare/utl_partial_ordering.h"
#include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

class weak_ordering {
    using value_t = compare::details::value_t;
    using order_t = compare::details::order_t;
    using zero_t = compare::details::obscure::zero_t;
    friend class strong_ordering;
    friend class partial_ordering;
public:
    static const weak_ordering less;
    static const weak_ordering equivalent;
    static const weak_ordering greater;

    constexpr operator partial_ordering() const { return partial_ordering(order_t(value)); }

#ifdef UTL_CXX20
    template<same_as<std::weak_ordering> T>
    constexpr weak_ordering(T p) noexcept : value(
        p == T::less ? less :
        p == T::equivalent ? equivalent :
        greater
    ) {}

    template<same_as<std::weak_ordering> T>
    constexpr operator T() noexcept {
        return *this == less ? T::less :
            *this == equivalent ? T::equivalent :
            T::greater;
    }
#endif
    constexpr weak_ordering(weak_ordering const&) noexcept = default;
    constexpr weak_ordering(weak_ordering&&) noexcept = default;
    constexpr weak_ordering& operator=(weak_ordering const&) noexcept = default;
    constexpr weak_ordering& operator=(weak_ordering&&) noexcept = default;

    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator==(weak_ordering l, weak_ordering r) noexcept { return l.value == r.value; }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator==(weak_ordering l, zero_t) noexcept { return l.value == 0; }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator<(zero_t, weak_ordering r) noexcept { return r.value == greater.value; }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator<(weak_ordering l, zero_t) noexcept { return l.value == less.value; }

    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator>(zero_t, weak_ordering r) noexcept { return r.value == less.value; }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator>(weak_ordering l, zero_t) noexcept { return l.value == greater.value; }

    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator<=(zero_t, weak_ordering r) noexcept { return r.value == greater.value || r.value == equivalent.value; }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator<=(weak_ordering l, zero_t) noexcept { return l.value == less.value || l.value == equivalent.value; }

    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator>=(zero_t, weak_ordering r) noexcept { return r.value == less.value || r.value == equivalent.value; }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator>=(weak_ordering l, zero_t) noexcept { return l.value == greater.value || l.value == equivalent.value; }

#ifdef UTL_CXX20
    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr weak_ordering operator<=>(weak_ordering l, zero_t) noexcept {
        return l;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr weak_ordering operator<=>(zero_t, weak_ordering r) noexcept {
        return weak_ordering(order_t(-r.value));
    }
#endif

private:

    constexpr explicit weak_ordering(order_t value) noexcept : value(value_t(value)) {}
    value_t value;
};

UTL_INLINE_CXX17 constexpr weak_ordering weak_ordering::less{compare::details::order_t::less};
UTL_INLINE_CXX17 constexpr weak_ordering weak_ordering::equivalent{compare::details::order_t::equal};
UTL_INLINE_CXX17 constexpr weak_ordering weak_ordering::greater{compare::details::order_t::greater};

UTL_NAMESPACE_END
