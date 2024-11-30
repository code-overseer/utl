// Adapted from the LLVM Project, Copyright 2023-2024 Bryan Wong

#include "utl/ranges/utl_swap.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_trivially_copy_constructible.h"
#include "utl/type_traits/utl_is_trivially_move_constructible.h"
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

struct NotConvertible {
    explicit NotConvertible(int);
};

struct CopyOnly {
    int i;
    constexpr CopyOnly(int ii) : i(ii) {}
    CopyOnly(CopyOnly const&) = default;
    CopyOnly(CopyOnly&&) = delete;
    friend constexpr bool operator==(CopyOnly const& mi, int ii) { return mi.i == ii; }
};

class MoveOnly {
    int data_;

public:
    constexpr MoveOnly(int data = 1) : data_(data) {}

    MoveOnly(MoveOnly const&) = delete;
    MoveOnly& operator=(MoveOnly const&) = delete;

    UTL_CONSTEXPR_CXX14 MoveOnly(MoveOnly&& x) noexcept : data_(x.data_) { x.data_ = 0; }
    UTL_CONSTEXPR_CXX14 MoveOnly& operator=(MoveOnly&& x) {
        data_ = x.data_;
        x.data_ = 0;
        return *this;
    }

    friend constexpr bool operator==(MoveOnly const& x, MoveOnly const& y) {
        return x.data_ == y.data_;
    }
    friend constexpr bool operator!=(MoveOnly const& x, MoveOnly const& y) {
        return x.data_ != y.data_;
    }
    friend constexpr bool operator<(MoveOnly const& x, MoveOnly const& y) {
        return x.data_ < y.data_;
    }
    friend constexpr bool operator<=(MoveOnly const& x, MoveOnly const& y) {
        return x.data_ <= y.data_;
    }
    friend constexpr bool operator>(MoveOnly const& x, MoveOnly const& y) {
        return x.data_ > y.data_;
    }
    friend constexpr bool operator>=(MoveOnly const& x, MoveOnly const& y) {
        return x.data_ >= y.data_;
    }

    constexpr int get() const { return data_; }
};

template <int Constant, bool Noexcept = true, bool ThrowOnMove = false>
struct TailClobbererNonTrivialMove : TailClobberer<Constant> {
    using TailClobberer<Constant>::TailClobberer;

    [[noreturn]] void ThrowExcept() { throw Except{}; }

    constexpr TailClobbererNonTrivialMove(TailClobbererNonTrivialMove&&) noexcept(Noexcept)
        : TailClobberer<Constant>() {
#if UTL_WITH_EXCEPTIONS
#  if UTL_CXX17
        if constexpr (!Noexcept && ThrowOnMove)
#  else
        if (!Noexcept && ThrowOnMove)
#  endif
            ThrowExcept();
#endif
    }
};
static_assert(!utl::is_trivially_copy_constructible_v<TailClobbererNonTrivialMove<0>>, "");
static_assert(utl::is_move_constructible_v<TailClobbererNonTrivialMove<0>>, "");
static_assert(!utl::is_trivially_move_constructible_v<TailClobbererNonTrivialMove<0>>, "");
static_assert(utl::is_nothrow_move_constructible_v<TailClobbererNonTrivialMove<0, true>>, "");
static_assert(!utl::is_nothrow_move_constructible_v<TailClobbererNonTrivialMove<0, false>>, "");
} // namespace expected
