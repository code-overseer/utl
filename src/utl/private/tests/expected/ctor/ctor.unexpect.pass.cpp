// Adapted from the LLVM Project, Copyright 2023-2024 Bryan Wong
// Commit: 9fd3c4115cf2cd3da1405e1f2c38d53582b5dc81

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// template<class... Args>
//   constexpr explicit expected(unexpect_t, Args&&... args);
//
// Constraints: is_constructible_v<E, Args...> is true.
//
// Effects: Direct-non-list-initializes unex with utl::forward<Args>(args)....
//
// Postconditions: has_value() is false.
//
// Throws: Any exception thrown by the initialization of unex.

#include "utl/utl_config.h"

#include "tests/expected/test_types.h"
#include "tests/test_macros.h"
#include "utl/expected/utl_expected.h"
#include "utl/tuple/utl_tuple.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_implicit_constructible.h"

#include <cassert>

namespace expected {
// Test Constraints:
static_assert(utl::is_constructible_v<utl::expected<int, int>, utl::unexpect_t>, "");
static_assert(utl::is_constructible_v<utl::expected<int, int>, utl::unexpect_t, int>, "");

// !is_constructible_v<T, Args...>
struct foo {};
static_assert(!utl::is_constructible_v<utl::expected<int, foo>, utl::unexpect_t, int>, "");

static_assert(!utl::is_implicit_constructible<utl::expected<int, int>, utl::unexpect_t>::value, "");
static_assert(
    !utl::is_implicit_constructible<utl::expected<int, int>, utl::unexpect_t, int>::value, "");

template <class T, class V = int>
constexpr void testInt() {
    utl::expected<V, T> const e(utl::unexpect, 5);
    assert(!e.has_value());
    assert(e.error() == 5);
}

template <class T, class V = int>
constexpr void testLValue() {
    T t(5);
    utl::expected<V, T> e(utl::unexpect, t);
    assert(!e.has_value());
    assert(e.error() == 5);
}

template <class T, class V = int>
constexpr void testRValue() {
    utl::expected<V, T> e(utl::unexpect, T(5));
    assert(!e.has_value());
    assert(e.error() == 5);
}

constexpr bool test() {
    testInt<int>();
    testInt<CopyOnly>();
    testInt<MoveOnly>();
    testInt<TailClobberer<1>, bool>();
    testLValue<int>();
    testLValue<CopyOnly>();
    testLValue<TailClobberer<1>, bool>();
    testRValue<int>();
    testRValue<MoveOnly>();
    testRValue<TailClobberer<1>, bool>();

    // no arg
    {
        utl::expected<int, int> e(utl::unexpect);
        assert(!e.has_value());
        assert(e.error() == 0);
    }

    // one arg
    {
        utl::expected<int, int> e(utl::unexpect, 5);
        assert(!e.has_value());
        assert(e.error() == 5);
    }

    // multi args
    {
        utl::expected<int, utl::tuple<int, short, MoveOnly>> e(
            utl::unexpect, 1, short{2}, MoveOnly(3));
        assert(!e.has_value());
        assert((e.error() == utl::tuple<int, short, MoveOnly>(1, short{2}, MoveOnly(3))));
    }

    return true;
}

void testException() {
#ifdef UTL_WITH_EXCEPTIONS
    struct Throwing {
        Throwing(int) { throw Except{}; };
    };

    try {
        utl::expected<int, Throwing> u(utl::unexpect, 5);
        assert(false);
    } catch (Except) {}
#endif // UTL_WITH_EXCEPTIONS
}

} // namespace expected

int main() {
    expected::test();
#if UTL_CXX14
    static_assert(expected::test(), "");
#endif
    expected::testException();
}
