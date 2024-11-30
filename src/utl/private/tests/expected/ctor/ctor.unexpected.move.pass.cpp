//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// template<class G>
//   constexpr explicit(!is_convertible<G, E>) expected(unexpected<G>::value&& e);
//
// Let GF be G
//
// Constraints: is_constructible<E, GF>::value is true.
//
// Effects: Direct-non-list-initializes unex with utl::forward<GF>(e.error()).
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

#include <cassert>
namespace expected {
// Test Constraints
static_assert(utl::is_constructible<utl::expected<int, int>, utl::unexpected<int>>::value, "");
static_assert(
    utl::is_constructible<utl::expected<int, MoveOnly>, utl::unexpected<MoveOnly>>::value, "");

// !is_constructible<E, GF>::value
struct foo {};
static_assert(!utl::is_constructible<utl::expected<int, int>, utl::unexpected<foo>>::value, "");

static_assert(utl::is_convertible<utl::unexpected<int>&&, utl::expected<int, int>>::value, "");
static_assert(
    !utl::is_convertible<utl::unexpected<int>&&, utl::expected<int, NotConvertible>>::value, "");

struct MyInt {
    int i;
    constexpr MyInt(int ii) : i(ii) {}
    friend constexpr bool operator==(MyInt const& l, MyInt const& r) { return l.i == r.i; }
    friend constexpr bool operator!=(MyInt const& l, MyInt const& r) { return l.i != r.i; }
};

template <class Err, class V = int>
UTL_CONSTEXPR_CXX14 void testInt() {
    utl::unexpected<int> u(5);
    utl::expected<V, Err> e(utl::move(u));
    assert(!e.has_value());
    assert(e.error() == 5);
}

UTL_CONSTEXPR_CXX14 void testMoveOnly() {
    utl::unexpected<MoveOnly> u(MoveOnly(5));
    utl::expected<int, MoveOnly> e(utl::move(u));
    assert(!e.has_value());
    assert(e.error() == 5);
    assert(u.error() == 0);
}

UTL_CONSTEXPR_CXX14 bool test() {
    testInt<int>();
    testInt<MyInt>();
    testInt<MoveOnly>();
    testInt<TailClobberer<1>, bool>();
    testMoveOnly();
    return true;
}

void testException() {
#if UTL_WITH_EXCEPTIONS
    struct Throwing {
        Throwing(int) { throw Except{}; }
    };

    {
        utl::unexpected<int> u(5);
        try {
            UTL_ATTRIBUTE(MAYBE_UNUSED) utl::expected<int, Throwing> e(utl::move(u));
            assert(false);
        } catch (Except) {}
    }

#endif // TEST_HAS_NO_EXCEPTIONS
}
} // namespace expected
int main(int, char**) {
    expected::test();
#if UTL_CXX17
    static_assert(expected::test(), "");
#endif
    expected::testException();
    return 0;
}
