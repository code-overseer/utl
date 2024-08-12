// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/compare/utl_compare_fwd.h"

#include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

#define __UTL_ATTRIBUTE_COMPARE_API (NODISCARD)(CONST)(ALWAYS_INLINE)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_COMPARE_API

class __UTL_ABI_PUBLIC partial_ordering :
    details::compare::less_value<partial_ordering>,
    details::compare::equivalent_value<partial_ordering>,
    details::compare::greater_value<partial_ordering>,
    details::compare::unordered_value<partial_ordering> {
    using value_t = details::compare::value_t;
    using order_t = details::compare::order_t;
    using unorder_t = details::compare::unorder_t;
    using zero_t = details::compare::zero_t;
    friend class strong_ordering;
    friend class weak_ordering;
    friend details::compare::less_value<partial_ordering>;
    friend details::compare::equivalent_value<partial_ordering>;
    friend details::compare::greater_value<partial_ordering>;
    friend details::compare::unordered_value<partial_ordering>;

public:
    // TODO test for ODR issues
    using details::compare::less_value<partial_ordering>::less;
    using details::compare::equivalent_value<partial_ordering>::equivalent;
    using details::compare::greater_value<partial_ordering>::greater;
    using details::compare::unordered_value<partial_ordering>::unordered;

#if UTL_CXX20
    template <same_as<std::partial_ordering> T>
    __UTL_HIDE_FROM_ABI constexpr partial_ordering(T p) noexcept
        : value(p == T::less           ? less
                  : p == T::equivalent ? equivalent
                  : p == T::greater    ? greater
                                       : unordered) {}

    template <same_as<std::partial_ordering> T>
    __UTL_HIDE_FROM_ABI constexpr operator T() noexcept {
        return *this == less      ? T::less
            : *this == equivalent ? T::equivalent
            : *this == greater    ? T::greater
                                  : T::unordered;
    }
#endif

    __UTL_HIDE_FROM_ABI constexpr partial_ordering(partial_ordering const&) noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr partial_ordering(partial_ordering&&) noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr partial_ordering& operator=(
        partial_ordering const&) noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr partial_ordering& operator=(
        partial_ordering&&) noexcept = default;

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator==(
        partial_ordering l, partial_ordering r) noexcept {
        return l.value == r.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator==(partial_ordering l, zero_t) noexcept {
        return l.value == 0;
    }

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator<(zero_t, partial_ordering r) noexcept {
        return r.value == greater.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator<(partial_ordering l, zero_t) noexcept {
        return l.value == less.value;
    }

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator>(zero_t, partial_ordering r) noexcept {
        return r.value == less.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator>(partial_ordering l, zero_t) noexcept {
        return l.value == greater.value;
    }

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator<=(zero_t, partial_ordering r) noexcept {
        return r.value == greater.value || r.value == equivalent.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator<=(partial_ordering l, zero_t) noexcept {
        return l.value == less.value || l.value == equivalent.value;
    }

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator>=(zero_t, partial_ordering r) noexcept {
        return r.value == less.value || r.value == equivalent.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator>=(partial_ordering l, zero_t) noexcept {
        return l.value == greater.value || l.value == equivalent.value;
    }

#if UTL_CXX20
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr partial_ordering operator<=>(
        partial_ordering l, zero_t) noexcept {
        return l;
    }

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr partial_ordering operator<=>(
        zero_t, partial_ordering r) noexcept {
        switch (r.value) {
        case value_t(order_t::less):
            return greater;
        case value_t(order_t::greater):
            return less;
        default:
            return r;
        }
    }
#endif

private:
    __UTL_HIDE_FROM_ABI constexpr explicit partial_ordering(order_t value) noexcept
        : value(value_t(value)) {}
    __UTL_HIDE_FROM_ABI constexpr explicit partial_ordering(unorder_t value) noexcept
        : value(value_t(unorder_t::unordered)) {}

    value_t value;
};

#undef __UTL_ATTRIBUTE_COMPARE_API
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_COMPARE_API

UTL_NAMESPACE_END
