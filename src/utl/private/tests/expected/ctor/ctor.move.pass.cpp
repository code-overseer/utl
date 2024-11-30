// Adapted from the LLVM Project, Copyright 2023-2024 Bryan Wong

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03

// constexpr expected(expected&& rhs) noexcept(see below);
//
// Constraints:
// - is_move_constructible<T>::value is true and
// - is_move_constructible<E>::value is true.
//
// Effects: If rhs.has_value() is true, direct-non-list-initializes val with utl::move(*rhs).
// Otherwise, direct-non-list-initializes unex with utl::move(rhs.error()).
//
// Postconditions: rhs.has_value() is unchanged; rhs.has_value() == this->has_value() is true.
//
// Throws: Any exception thrown by the initialization of val or unex.
//
// Remarks: The exception specification is equivalent to is_nothrow_move_constructible<T>::value &&
// is_nothrow_move_constructible<E>::value.
//
// This constructor is trivial if
// - is_trivially_move_constructible<T>::value is true and
// - is_trivially_move_constructible<E>::value is true.

#include "utl/utl_config.h"

#include "tests/expected/test_types.h"
#include "tests/test_macros.h"
#include "utl/expected/utl_expected.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_trivially_move_constructible.h"

#include <cassert>

namespace expected {

struct NonMovable {
    NonMovable(NonMovable&&) = delete;
};

struct MovableNonTrivial {
    int i;
    constexpr MovableNonTrivial(int ii) : i(ii) {}
    constexpr MovableNonTrivial(MovableNonTrivial&& o) : i(o.i) { o.i = 0; }
    friend constexpr bool operator==(MovableNonTrivial const& l, MovableNonTrivial const& r) {
        return l.i == r.i;
    }
    friend constexpr bool operator!=(MovableNonTrivial const& l, MovableNonTrivial const& r) {
        return l.i != r.i;
    }
};

struct MoveMayThrow {
    MoveMayThrow(MoveMayThrow&&) {}
};

// Test Constraints:
// - is_move_constructible<T>::value is true and
// - is_move_constructible<E>::value is true.
static_assert(utl::is_move_constructible<utl::expected<int, int>>::value, "");
static_assert(utl::is_move_constructible<utl::expected<MovableNonTrivial, int>>::value, "");
static_assert(utl::is_move_constructible<utl::expected<int, MovableNonTrivial>>::value, "");
static_assert(
    utl::is_move_constructible<utl::expected<MovableNonTrivial, MovableNonTrivial>>::value, "");
static_assert(!utl::is_move_constructible<utl::expected<NonMovable, int>>::value, "");
static_assert(!utl::is_move_constructible<utl::expected<int, NonMovable>>::value, "");
static_assert(!utl::is_move_constructible<utl::expected<NonMovable, NonMovable>>::value, "");

// Test: This constructor is trivial if
// - is_trivially_move_constructible<T>::value is true and
// - is_trivially_move_constructible<E>::value is true.
static_assert(utl::is_trivially_move_constructible<utl::expected<int, int>>::value, "");
static_assert(
    !utl::is_trivially_move_constructible<utl::expected<MovableNonTrivial, int>>::value, "");
static_assert(
    !utl::is_trivially_move_constructible<utl::expected<int, MovableNonTrivial>>::value, "");
static_assert(!utl::is_trivially_move_constructible<
                  utl::expected<MovableNonTrivial, MovableNonTrivial>>::value,
    "");

// Test: The exception specification is equivalent to
// is_nothrow_move_constructible<T> && is_nothrow_move_constructible_v<E>::value.

static_assert(
    utl::is_nothrow_move_constructible<utl::details::expected::underlying_storage<int, int>>::value,
    "");
static_assert(utl::is_nothrow_move_constructible<utl::expected<int, int>>::value, "");
static_assert(!utl::is_nothrow_move_constructible<utl::expected<MoveMayThrow, int>>::value, "");
static_assert(!utl::is_nothrow_move_constructible<utl::expected<int, MoveMayThrow>>::value, "");
static_assert(
    !utl::is_nothrow_move_constructible<utl::expected<MoveMayThrow, MoveMayThrow>>::value, "");

constexpr bool test() {
    // move the value non-trivial
    {
        utl::expected<MovableNonTrivial, int> e1(5);
        auto e2 = utl::move(e1);
        assert(e2.has_value());
        assert(e2.value().i == 5);
        assert(e1.has_value());
        assert(e1.value().i == 0);
    }

    // move the error non-trivial
    {
        utl::expected<int, MovableNonTrivial> e1(utl::unexpect, 5);
        auto e2 = utl::move(e1);
        assert(!e2.has_value());
        assert(e2.error().i == 5);
        assert(!e1.has_value());
        assert(e1.error().i == 0);
    }

    // move the value trivial
    {
        utl::expected<int, int> e1(5);
        auto e2 = utl::move(e1);
        assert(e2.has_value());
        assert(e2.value() == 5);
        assert(e1.has_value());
    }

    // move the error trivial
    {
        utl::expected<int, int> e1(utl::unexpect, 5);
        auto e2 = utl::move(e1);
        assert(!e2.has_value());
        assert(e2.error() == 5);
        assert(!e1.has_value());
    }

    // move TailClobbererNonTrivialMove as value
    {
        utl::expected<TailClobbererNonTrivialMove<0>, bool> e1;
        auto e2 = utl::move(e1);
        assert(e2.has_value());
        assert(e1.has_value());
    }

    // move TailClobbererNonTrivialMove as error
    {
        utl::expected<bool, TailClobbererNonTrivialMove<1>> e1(utl::unexpect);
        auto e2 = utl::move(e1);
        assert(!e2.has_value());
        assert(!e1.has_value());
    }

    return true;
}

void testException() {
#ifndef TEST_HAS_NO_EXCEPTIONS
    struct Throwing {
        Throwing() = default;
        Throwing(Throwing&&) { throw Except{}; }
    };

    // throw on moving value
    {
        utl::expected<Throwing, int> e1;
        try {
            UTL_ATTRIBUTE(MAYBE_UNUSED) auto e2 = utl::move(e1);
            assert(false);
        } catch (Except) {}
    }

    // throw on moving error
    {
        utl::expected<int, Throwing> e1(utl::unexpect);
        try {
            UTL_ATTRIBUTE(MAYBE_UNUSED) auto e2 = utl::move(e1);
            assert(false);
        } catch (Except) {}
    }

#endif // TEST_HAS_NO_EXCEPTIONS
}
} // namespace expected

int main(int, char**) {
    expected::test();
#if UTL_CXX17
    // Certain ctors are only constexpr in C++17
    static_assert(expected::test(), "");
#endif
    expected::testException();
    return 0;
}
