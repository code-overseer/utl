// Adapted from the LLVM Project, Copyright 2023-2024 Bryan Wong

#include "utl/ranges/utl_swap.h"
#include "utl/utility/utl_constant_p.h"

#include <cstring>
#include <initializer_list>

namespace expected {
struct Except {};
struct NoDefaultCtor {
    NoDefaultCtor() = delete;
};

template <int Constant>
struct TailClobberer {
    constexpr TailClobberer() noexcept {
        if (!UTL_CONSTANT_P(static_cast<void*>(this))) {
            memset(static_cast<void*>(this), Constant, sizeof(*this));
        }
        // Always set `b` itself to `false` so that the comparison works.
        b = false;
    }
    constexpr TailClobberer(TailClobberer const&) : TailClobberer() {}
    constexpr TailClobberer(TailClobberer&&) = default;
    // Converts from `int`/`std::initializer_list<int>, used in some tests.
    constexpr TailClobberer(int) : TailClobberer() {}
    constexpr TailClobberer(std::initializer_list<int>) noexcept : TailClobberer() {}

    friend constexpr bool operator==(
        TailClobberer const& left, TailClobberer const& right) noexcept {
        return left.b == right.b;
    }

    friend constexpr bool operator!=(
        TailClobberer const& left, TailClobberer const& right) noexcept {
        return left.b != right.b;
    }

    friend constexpr void swap(TailClobberer& l, TailClobberer& r) { __UTL ranges::swap(l, r); }

private:
    alignas(2) bool b;
};
} // namespace expected
