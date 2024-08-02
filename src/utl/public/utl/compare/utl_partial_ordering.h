// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_attributes.h"

#include "utl/compare/utl_compare_fwd.h"

#include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

#define UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_ATTRIBUTES(NODISCARD, CONST)

class partial_ordering :
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
    UTL_HIDE_FROM_ABI constexpr partial_ordering(T p) noexcept
        : value_(p == T::less          ? less
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

    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr bool operator==(
        partial_ordering l, partial_ordering r) noexcept {
        return l.value_ == r.value_;
    }
    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr bool operator==(
        partial_ordering l, zero_t) noexcept {
        return l.value_ == 0;
    }

    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr bool operator<(
        zero_t, partial_ordering r) noexcept {
        return r.value_ == greater.value_;
    }
    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr bool operator<(
        partial_ordering l, zero_t) noexcept {
        return l.value_ == less.value_;
    }

    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr bool operator>(
        zero_t, partial_ordering r) noexcept {
        return r.value_ == less.value_;
    }
    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr bool operator>(
        partial_ordering l, zero_t) noexcept {
        return l.value_ == greater.value_;
    }

    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr bool operator<=(
        zero_t, partial_ordering r) noexcept {
        return r.value_ == greater.value_ || r.value_ == equivalent.value_;
    }
    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr bool operator<=(
        partial_ordering l, zero_t) noexcept {
        return l.value_ == less.value_ || l.value_ == equivalent.value_;
    }

    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr bool operator>=(
        zero_t, partial_ordering r) noexcept {
        return r.value_ == less.value_ || r.value_ == equivalent.value_;
    }
    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr bool operator>=(
        partial_ordering l, zero_t) noexcept {
        return l.value_ == greater.value_ || l.value_ == equivalent.value_;
    }

#if UTL_CXX20
    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr partial_ordering operator<=>(
        partial_ordering l, zero_t) noexcept {
        return l;
    }

    UTL_PARTIAL_ORDERING_ATTRIBUTES UTL_HIDE_FROM_ABI friend constexpr partial_ordering operator<=>(
        zero_t, partial_ordering r) noexcept {
        switch (r.value_) {
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
        : value_(value_t(value)) {}
    UTL_HIDE_FROM_ABI constexpr explicit partial_ordering(unorder_t value) noexcept
        : value_(value_t(unorder_t::unordered)) {}

    value_t value_;
};

UTL_NAMESPACE_END

#undef UTL_PARTIAL_ORDERING_ATTRIBUTES
