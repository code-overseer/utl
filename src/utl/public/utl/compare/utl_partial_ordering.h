// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_attributes.h"

#include "utl/compare/utl_compare_fwd.h"

#include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

#define __UTL_ATTRIBUTE_COMPARE_API (NODISCARD)(CONST)(ALWAYS_INLINE)__UTL_ATTRIBUTE_HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_COMPARE_API

class UTL_ABI_PUBLIC partial_ordering {
    using value_t = details::compare::value_t;
    using order_t = details::compare::order_t;
    using unorder_t = details::compare::unorder_t;
    using zero_t = details::compare::zero_t;
    friend class strong_ordering;
    friend class weak_ordering;

public:
    static partial_ordering const less;
    static partial_ordering const equivalent;
    static partial_ordering const greater;
    static partial_ordering const unordered;

#if UTL_CXX20
    template <same_as<std::partial_ordering> T>
    UTL_HIDE_FROM_ABI constexpr partial_ordering(T p) noexcept
        : value(p == T::less           ? less
                  : p == T::equivalent ? equivalent
                  : p == T::greater    ? greater
                                       : unordered) {}

    template <same_as<std::partial_ordering> T>
    UTL_HIDE_FROM_ABI constexpr operator T() noexcept {
        return *this == less      ? T::less
            : *this == equivalent ? T::equivalent
            : *this == greater    ? T::greater
                                  : T::unordered;
    }
#endif

    UTL_HIDE_FROM_ABI constexpr partial_ordering(partial_ordering const&) noexcept = default;
    UTL_HIDE_FROM_ABI constexpr partial_ordering(partial_ordering&&) noexcept = default;
    UTL_HIDE_FROM_ABI constexpr partial_ordering& operator=(
        partial_ordering const&) noexcept = default;
    UTL_HIDE_FROM_ABI constexpr partial_ordering& operator=(partial_ordering&&) noexcept = default;

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
    UTL_HIDE_FROM_ABI constexpr explicit partial_ordering(order_t value) noexcept
        : value(value_t(value)) {}
    UTL_HIDE_FROM_ABI constexpr explicit partial_ordering(unorder_t value) noexcept
        : value(value_t(unorder_t::unordered)) {}
    value_t value;
};

UTL_ABI_PUBLIC_DATA constexpr partial_ordering partial_ordering::less{
    details::compare::order_t::less};
UTL_ABI_PUBLIC_DATA constexpr partial_ordering partial_ordering::equivalent{
    details::compare::order_t::equal};
UTL_ABI_PUBLIC_DATA constexpr partial_ordering partial_ordering::greater{
    details::compare::order_t::greater};
UTL_ABI_PUBLIC_DATA constexpr partial_ordering partial_ordering::unordered{
    details::compare::unorder_t::unordered};

#undef __UTL_ATTRIBUTE_COMPARE_API
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_COMPARE_API

UTL_NAMESPACE_END
