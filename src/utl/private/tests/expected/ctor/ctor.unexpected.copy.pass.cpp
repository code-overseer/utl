// Adapted from the LLVM Project, Copyright 2023-2024 Bryan Wong
// Commit: 9fd3c4115cf2cd3da1405e1f2c38d53582b5dc81

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03

// template<class G>
//   constexpr explicit(!is_convertible<const G&, E>) expected(const unexpected<G>::value& e);
//
// Let GF be const G&
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
static_assert(
    utl::is_constructible<utl::expected<int, int>, utl::unexpected<int> const&>::value, "");

// !is_constructible<E, GF>::value
struct foo {};
static_assert(
    !utl::is_constructible<utl::expected<int, int>, utl::unexpected<foo> const&>::value, "");
static_assert(
    !utl::is_constructible<utl::expected<int, MoveOnly>, utl::unexpected<MoveOnly> const&>::value,
    "");

static_assert(utl::is_convertible<utl::unexpected<int> const&, utl::expected<int, int>>::value, "");
static_assert(
    !utl::is_convertible<utl::unexpected<int> const&, utl::expected<int, NotConvertible>>::value,
    "");

struct MyInt {
    int i;
    constexpr MyInt(int ii) : i(ii) {}
    friend constexpr bool operator==(MyInt const& l, MyInt const& r) { return l.i == r.i; }
    friend constexpr bool operator!=(MyInt const& l, MyInt const& r) { return l.i != r.i; }
};

template <class T, class V = int>
UTL_CONSTEXPR_CXX14 void testUnexpected() {
    utl::unexpected<int> const u(5);
    utl::expected<V, T> e(u);
    assert(!e.has_value());
    assert(e.error() == 5);
}

UTL_CONSTEXPR_CXX14 bool test() {
    testUnexpected<int>();
    testUnexpected<MyInt>();
    testUnexpected<TailClobberer<1>, bool>();
    return true;
}

void testException() {
#if UTL_WITH_EXCEPTIONS
    struct Throwing {
        Throwing(int) { throw Except{}; }
    };

    {
        utl::unexpected<int> const u(5);
        try {
            UTL_ATTRIBUTE(MAYBE_UNUSED) utl::expected<int, Throwing> e(u);
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
