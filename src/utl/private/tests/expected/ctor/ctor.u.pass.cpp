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

// template<class U = T>
//   constexpr explicit(!is_convertible_v<U, T>) expected(U&& v);
//
// Constraints:
// - is_same_v<remove_cvref_t<U>, in_place_t> is false; and
// - is_same_v<expected, remove_cvref_t<U>> is false; and
// - remove_cvref_t<U> is not a specialization of unexpected; and
// - is_constructible_v<T, U> is true.
//
// Effects: Direct-non-list-initializes val with utl::forward<U>(v).
//
// Postconditions: has_value() is true.
//
// Throws: Any exception thrown by the initialization of val.

#include "utl/utl_config.h"

#include "tests/expected/test_types.h"
#include "tests/test_macros.h"
#include "utl/expected/utl_expected.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"

#include <cassert>

namespace expected {
namespace ctor {
namespace u {

// Test Constraints:
static_assert(utl::is_constructible_v<utl::expected<int, int>, int>, "");

// is_same_v<remove_cvref_t<U>, in_place_t>
struct FromJustInplace {
    FromJustInplace(utl::in_place_t);
};
static_assert(!utl::is_constructible_v<utl::expected<FromJustInplace, int>, utl::in_place_t>, "");
static_assert(
    !utl::is_constructible_v<utl::expected<FromJustInplace, int>, utl::in_place_t const&>, "");

// is_same_v<expected, remove_cvref_t<U>>
// Note that result is true because it is covered by the constructors that take expected
static_assert(utl::is_constructible_v<utl::expected<int, int>, utl::expected<int, int>&>, "");

// remove_cvref_t<U> is a specialization of unexpected
// Note that result is true because it is covered by the constructors that take unexpected
static_assert(utl::is_constructible_v<utl::expected<int, int>, utl::unexpected<int>&>, "");

// !is_constructible_v<T, U>
struct foo {};
static_assert(!utl::is_constructible_v<utl::expected<int, int>, foo>, "");

// test explicit(!is_convertible_v<U, T>)

static_assert(utl::is_convertible_v<int, utl::expected<int, int>>, "");
static_assert(!utl::is_convertible_v<int, utl::expected<NotConvertible, int>>, "");

struct BaseError {};
struct DerivedError : BaseError {};

template <class T, class E = int>
constexpr void testInt() {
    utl::expected<T, E> e(5);
    assert(e.has_value());
    assert(e.value() == 5);
}

template <class T, class E = int>
constexpr void testLValue() {
    T t(5);
    utl::expected<T, E> e(t);
    assert(e.has_value());
    assert(e.value() == 5);
}

template <class T, class E = int>
constexpr void testRValue() {
    utl::expected<T, E> e(T(5));
    assert(e.has_value());
    assert(e.value() == 5);
}

constexpr bool test() {
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

    // Test default template argument.
    // Without it, the template parameter cannot be deduced from an initializer list
    {
        struct Bar {
            int i;
            int j;
            constexpr Bar(int ii, int jj) : i(ii), j(jj) {}
        };

        utl::expected<Bar, int> e({5, 6});
        assert(e.value().i == 5);
        assert(e.value().j == 6);
    }

    // https://cplusplus.github.io/LWG/issue3836

    // Test &
    {
        utl::expected<bool, DerivedError> e1(false);
        utl::expected<bool, BaseError> e2(e1);
        assert(e2.has_value());
        assert(!e2.value()); // yes, e2 holds "false" since LWG3836
    }

    // Test &&
    {
        utl::expected<bool, DerivedError> e1(false);
        utl::expected<bool, BaseError> e2(utl::move(e1));
        assert(e2.has_value());
        assert(!e2.value()); // yes, e2 holds "false" since LWG3836
    }

    // Test const&
    {
        utl::expected<bool, DerivedError> const e1(false);
        utl::expected<bool, BaseError> e2(e1);
        assert(e2.has_value());
        assert(!e2.value()); // yes, e2 holds "false" since LWG3836
    }

    // Test const&&
    {
        utl::expected<bool, DerivedError> const e1(false);
        utl::expected<bool, BaseError> e2(utl::move(e1));
        assert(e2.has_value());
        assert(!e2.value()); // yes, e2 holds "false" since LWG3836
    }
    return true;
}

void testException() {
#ifdef UTL_WITH_EXCEPTIONS
    struct Throwing {
        Throwing(int) { throw Except{}; };
    };

    try {
        utl::expected<Throwing, int> u(5);
        assert(false);
    } catch (Except) {}
#endif // UTL_WITH_EXCEPTIONS
}

int main(int, char**) {
    test();
#if UTL_CXX17
    static_assert(test(), "");
#endif
    testException();
    return 0;
}
} // namespace u
} // namespace ctor
} // namespace expected
