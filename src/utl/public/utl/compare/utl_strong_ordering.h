// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/compare/utl_compare_fwd.h"
#include "utl/compare/utl_partial_ordering.h"
#include "utl/compare/utl_weak_ordering.h"
#include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

class strong_ordering {
    using value_t = details::compare::value_t;
    using order_t = details::compare::order_t;
    using zero_t = details::compare::zero_t;

public:
    static strong_ordering const less;
    static strong_ordering const equivalent;
    static strong_ordering const equal;
    static strong_ordering const greater;

    constexpr operator partial_ordering() const { return partial_ordering(order_t(value)); }
    constexpr operator weak_ordering() const { return weak_ordering(order_t(value)); }
#if UTL_CXX20
    template <same_as<std::strong_ordering> T>
    constexpr strong_ordering(T p) noexcept
        : value(p == T::less           ? less
                  : p == T::equivalent ? equivalent
                  : p == T::equal      ? equal
                                       : greater) {}

    template <same_as<std::strong_ordering> T>
    constexpr operator T() noexcept {
        return *this == less      ? T::less
            : *this == equivalent ? T::equivalent
            : *this == equal      ? T::equal
                                  : T::greater;
    }
#endif
    constexpr strong_ordering(strong_ordering const&) noexcept = default;
    constexpr strong_ordering(strong_ordering&&) noexcept = default;
    constexpr strong_ordering& operator=(strong_ordering const&) noexcept = default;
    constexpr strong_ordering& operator=(strong_ordering&&) noexcept = default;

    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator==(strong_ordering l, strong_ordering r) noexcept {
        return l.value == r.value;
    }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr bool operator==(strong_ordering l, zero_t) noexcept { return l.value == 0; }

#define UTL_ORDERING_COMPARISONS(SYMBOL)                                        \
    UTL_ATTRIBUTES(NODISCARD, CONST)                                            \
    friend constexpr bool operator SYMBOL(zero_t, strong_ordering r) noexcept { \
        return 0 SYMBOL r.value;                                                \
    }                                                                           \
    friend constexpr bool operator SYMBOL(strong_ordering l, zero_t) noexcept { \
        return l.value SYMBOL 0;                                                \
    }

    UTL_ORDERING_COMPARISONS(<)
    UTL_ORDERING_COMPARISONS(>)
    UTL_ORDERING_COMPARISONS(<=)
    UTL_ORDERING_COMPARISONS(>=)

#undef UTL_ORDERING_COMPARISONS

#if UTL_CXX20
    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr strong_ordering operator<=>(strong_ordering l, zero_t) noexcept { return l; }

    UTL_ATTRIBUTES(NODISCARD, CONST)
    friend constexpr strong_ordering operator<=>(zero_t, strong_ordering r) noexcept {
        return strong_ordering(order_t(-r.value));
    }
#endif

private:
    constexpr explicit strong_ordering(order_t value) noexcept : value(value_t(value)) {}
    value_t value;
};

UTL_INLINE_CXX17 constexpr strong_ordering strong_ordering::less{details::compare::order_t::less};
UTL_INLINE_CXX17 constexpr strong_ordering strong_ordering::equivalent{details::compare::order_t::equal};
UTL_INLINE_CXX17 constexpr strong_ordering strong_ordering::equal{details::compare::order_t::equal};
UTL_INLINE_CXX17 constexpr strong_ordering strong_ordering::greater{details::compare::order_t::greater};

UTL_NAMESPACE_END
