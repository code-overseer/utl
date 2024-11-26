// Adapted from the LLVM Project, Copyright 2023-2024 Bryan Wong
// Commit: 9fd3c4115cf2cd3da1405e1f2c38d53582b5dc81

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// template<class... Args>
//   constexpr explicit expected(in_place_t, Args&&... args);
//
// Constraints: is_constructible_v<T, Args...> is true.
//
// Effects: Direct-non-list-initializes val with utl::forward<Args>(args)....
//
// Postconditions: has_value() is true.
//
// Throws: Any exception thrown by the initialization of val.

#include "utl/utl_config.h"

#include "tests/expected/test_types.h"
#include "tests/test_macros.h"
#include "utl/expected/utl_expected.h"
#include "utl/tuple/utl_tuple.h"
#include "utl/type_traits/utl_is_default_constructible.h"
#include "utl/type_traits/utl_is_implicit_constructible.h"

#include <cassert>

namespace expected {
namespace ctor {
namespace inplace {

// Test Constraints:
static_assert(utl::is_constructible_v<utl::expected<int, int>, utl::in_place_t>, "");
static_assert(utl::is_constructible_v<utl::expected<int, int>, utl::in_place_t, int>, "");

// !is_constructible_v<T, Args...>
struct foo {};
static_assert(!utl::is_constructible_v<utl::expected<foo, int>, utl::in_place_t, int>, "");

static_assert(utl::is_implicit_constructible<int, int>::value, "");
static_assert(!utl::is_implicit_constructible<utl::expected<int, int>, utl::in_place_t>::value, "");
static_assert(
    !utl::is_implicit_constructible<utl::expected<int, int>, utl::in_place_t, int>::value, "");

template <class T, class E = int>
UTL_CONSTEXPR_CXX14 void testInt() {
    utl::expected<T, E> e(utl::in_place, 5);
    assert(e.has_value());
    assert(e.value() == 5);
}

template <class T, class E = int>
UTL_CONSTEXPR_CXX14 void testLValue() {
    T t(5);
    utl::expected<T, E> e(utl::in_place, t);
    assert(e.has_value());
    assert(e.value() == 5);
}

template <class T, class E = int>
UTL_CONSTEXPR_CXX14 void testRValue() {
    utl::expected<T, E> e(utl::in_place, T(5));
    assert(e.has_value());
    assert(e.value() == 5);
}

UTL_CONSTEXPR_CXX14 bool test() {
    testInt<int>();
    testInt<CopyOnly>();
    testInt<MoveOnly>();
    testInt<TailClobberer<0>, bool>();
    testLValue<int>();
    testLValue<CopyOnly>();
    testLValue<TailClobberer<0>, bool>();
    testRValue<int>();
    testRValue<MoveOnly>();
    testRValue<TailClobberer<0>, bool>();

    // no arg
    {
        utl::expected<int, int> e(utl::in_place);
        assert(e.has_value());
        assert(e.value() == 0);
    }

    // one arg
    {
        utl::expected<int, int> e(utl::in_place, 5);
        assert(e.has_value());
        assert(e.value() == 5);
    }

    // multi args
    {
        utl::expected<utl::tuple<int, short, MoveOnly>, int> e(
            utl::in_place, 1, short{2}, MoveOnly(3));
        assert(e.has_value());
        assert((e.value() == utl::tuple<int, short, MoveOnly>(1, short{2}, MoveOnly(3))));
    }

    return true;
}

void testException() {
#ifdef UTL_WITH_EXCEPTIONS
    struct Throwing {
        Throwing(int) { throw Except{}; };
    };

    try {
        utl::expected<Throwing, int> u(utl::in_place, 5);
        assert(false);
    } catch (Except) {}

#endif // UTL_WITH_EXCEPTIONS
}

void execute() {
    test();
#if UTL_CXX14
    static_assert(test(), "");
#endif
    testException();
}
} // namespace inplace
} // namespace ctor
} // namespace expected