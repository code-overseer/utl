// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/compare/utl_compare_fwd.h"

#include "utl/compare/utl_partial_ordering.h"
#include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

#define __UTL_ATTRIBUTE_COMPARE_API (NODISCARD)(CONST)(ALWAYS_INLINE)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_COMPARE_API

class __UTL_ABI_PUBLIC weak_ordering :
    details::compare::less_value<weak_ordering>,
    details::compare::equivalent_value<weak_ordering>,
    details::compare::greater_value<weak_ordering> {
    using value_t = details::compare::value_t;
    using order_t = details::compare::order_t;
    using zero_t = details::compare::zero_t;
    friend class strong_ordering;
    friend class partial_ordering;
    friend details::compare::less_value<weak_ordering>;
    friend details::compare::equivalent_value<weak_ordering>;
    friend details::compare::greater_value<weak_ordering>;

public:
    using details::compare::less_value<weak_ordering>::less;
    using details::compare::equivalent_value<weak_ordering>::equivalent;
    using details::compare::greater_value<weak_ordering>::greater;

    __UTL_HIDE_FROM_ABI constexpr operator partial_ordering() const noexcept {
        return partial_ordering(order_t(value));
    }

#if UTL_CXX20
    template <same_as<std::strong_ordering> T>
    __UTL_HIDE_FROM_ABI constexpr weak_ordering(T p) noexcept
        : value(p == T::less        ? less.value
                  : p == T::greater ? greater.value
                                    : equivalent.value) {}

    template <same_as<std::weak_ordering> T>
    __UTL_HIDE_FROM_ABI constexpr weak_ordering(T p) noexcept
        : value(p == T::less           ? less.value
                  : p == T::equivalent ? equivalent.value
                                       : greater.value) {}

    template <same_as<std::weak_ordering> T>
    __UTL_HIDE_FROM_ABI constexpr operator T() noexcept {
        return *this == less ? T::less : *this == equivalent ? T::equivalent : T::greater;
    }
#endif
    __UTL_HIDE_FROM_ABI constexpr weak_ordering(weak_ordering const&) noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr weak_ordering(weak_ordering&&) noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr weak_ordering& operator=(weak_ordering const&) noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr weak_ordering& operator=(weak_ordering&&) noexcept = default;

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator==(weak_ordering l, weak_ordering r) noexcept {
        return l.value == r.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator==(weak_ordering l, zero_t) noexcept {
        return l.value == 0;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator<(zero_t, weak_ordering r) noexcept {
        return r.value == greater.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator<(weak_ordering l, zero_t) noexcept {
        return l.value == less.value;
    }

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator>(zero_t, weak_ordering r) noexcept {
        return r.value == less.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator>(weak_ordering l, zero_t) noexcept {
        return l.value == greater.value;
    }

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator<=(zero_t, weak_ordering r) noexcept {
        return r.value == greater.value || r.value == equivalent.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator<=(weak_ordering l, zero_t) noexcept {
        return l.value == less.value || l.value == equivalent.value;
    }

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator>=(zero_t, weak_ordering r) noexcept {
        return r.value == less.value || r.value == equivalent.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator>=(weak_ordering l, zero_t) noexcept {
        return l.value == greater.value || l.value == equivalent.value;
    }

#if UTL_CXX20
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr weak_ordering operator<=>(weak_ordering l, zero_t) noexcept {
        return l;
    }

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr weak_ordering operator<=>(zero_t, weak_ordering r) noexcept {
        return weak_ordering(order_t(-r.value));
    }
#endif

private:
    __UTL_HIDE_FROM_ABI constexpr explicit weak_ordering(order_t value) noexcept
        : value(value_t(value)) {}

    value_t value;
};

#undef __UTL_ATTRIBUTE_COMPARE_API
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_COMPARE_API

UTL_NAMESPACE_END
