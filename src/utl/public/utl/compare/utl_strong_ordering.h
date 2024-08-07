// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/compare/utl_compare_fwd.h"

#include "utl/compare/utl_partial_ordering.h"
#include "utl/compare/utl_weak_ordering.h"
#include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

#define __UTL_ATTRIBUTE_COMPARE_API (NODISCARD)(CONST)(ALWAYS_INLINE)__UTL_ATTRIBUTE_HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_COMPARE_API

class UTL_ABI_PUBLIC strong_ordering :
    details::compare::less_value<strong_ordering>,
    details::compare::equivalent_value<strong_ordering>,
    details::compare::greater_value<strong_ordering>,
    details::compare::equal_value<strong_ordering> {
    using value_t = details::compare::value_t;
    using order_t = details::compare::order_t;
    using zero_t = details::compare::zero_t;
    friend details::compare::less_value<strong_ordering>;
    friend details::compare::equivalent_value<strong_ordering>;
    friend details::compare::greater_value<strong_ordering>;
    friend details::compare::equal_value<strong_ordering>;

public:
    using details::compare::less_value<strong_ordering>::less;
    using details::compare::equivalent_value<strong_ordering>::equivalent;
    using details::compare::greater_value<strong_ordering>::greater;
    using details::compare::equal_value<strong_ordering>::equal;

    UTL_HIDE_FROM_ABI constexpr operator partial_ordering() const {
        return partial_ordering(order_t(value));
    }
    UTL_HIDE_FROM_ABI constexpr operator weak_ordering() const {
        return weak_ordering(order_t(value));
    }
#if UTL_CXX20
    template <same_as<std::strong_ordering> T>
    UTL_HIDE_FROM_ABI constexpr strong_ordering(T p) noexcept
        : value(p == T::less           ? less
                  : p == T::equivalent ? equivalent
                  : p == T::equal      ? equal
                                       : greater) {}

    template <same_as<std::strong_ordering> T>
    UTL_HIDE_FROM_ABI constexpr operator T() noexcept {
        return *this == less      ? T::less
            : *this == equivalent ? T::equivalent
            : *this == equal      ? T::equal
                                  : T::greater;
    }
#endif
    UTL_HIDE_FROM_ABI constexpr strong_ordering(strong_ordering const&) noexcept = default;
    UTL_HIDE_FROM_ABI constexpr strong_ordering(strong_ordering&&) noexcept = default;
    UTL_HIDE_FROM_ABI constexpr strong_ordering& operator=(
        strong_ordering const&) noexcept = default;
    UTL_HIDE_FROM_ABI constexpr strong_ordering& operator=(strong_ordering&&) noexcept = default;

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator==(
        strong_ordering l, strong_ordering r) noexcept {
        return l.value == r.value;
    }
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr bool operator==(strong_ordering l, zero_t) noexcept {
        return l.value == 0;
    }

#define UTL_ORDERING_COMPARISONS(SYMBOL)                                        \
    UTL_ATTRIBUTE(COMPARE_API)                                                  \
    friend constexpr bool operator SYMBOL(zero_t, strong_ordering r) noexcept { \
        return 0 SYMBOL r.value;                                                \
    }                                                                           \
    UTL_ATTRIBUTE(COMPARE_API)                                                  \
    friend constexpr bool operator SYMBOL(strong_ordering l, zero_t) noexcept { \
        return l.value SYMBOL 0;                                                \
    }

    UTL_ORDERING_COMPARISONS(<)
    UTL_ORDERING_COMPARISONS(>)
    UTL_ORDERING_COMPARISONS(<=)
    UTL_ORDERING_COMPARISONS(>=)

#undef UTL_ORDERING_COMPARISONS

#if UTL_CXX20
    UTL_ATTRIBUTE(COMPARE_API) friend constexpr strong_ordering operator<=>(
        strong_ordering l, zero_t) noexcept {
        return l;
    }

    UTL_ATTRIBUTE(COMPARE_API) friend constexpr strong_ordering operator<=>(
        zero_t, strong_ordering r) noexcept {
        return strong_ordering(order_t(-r.value));
    }
#endif

private:
    UTL_HIDE_FROM_ABI constexpr explicit strong_ordering(order_t value) noexcept
        : value(value_t(value)) {}

    value_t value;
};

#undef __UTL_ATTRIBUTE_COMPARE_API
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_COMPARE_API

UTL_NAMESPACE_END
