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

//  template<class U, class G>
//    constexpr explicit(see below) expected(const expected<U, G>&);
//
// Let:
// - UF be const U&
// - GF be const G&
//
// Constraints:
// - is_constructible_v<T, UF> is true; and
// - is_constructible_v<E, GF> is true; and
// - is_constructible_v<T, expected<U, G>&> is false; and
// - is_constructible_v<T, expected<U, G>> is false; and
// - is_constructible_v<T, const expected<U, G>&> is false; and
// - is_constructible_v<T, const expected<U, G>> is false; and
// - is_convertible_v<expected<U, G>&, T> is false; and
// - is_convertible_v<expected<U, G>&&, T> is false; and
// - is_convertible_v<const expected<U, G>&, T> is false; and
// - is_convertible_v<const expected<U, G>&&, T> is false; and
// - is_constructible_v<unexpected<E>, expected<U, G>&> is false; and
// - is_constructible_v<unexpected<E>, expected<U, G>> is false; and
// - is_constructible_v<unexpected<E>, const expected<U, G>&> is false; and
// - is_constructible_v<unexpected<E>, const expected<U, G>> is false.
//
// Effects: If rhs.has_value(), direct-non-list-initializes val with utl::forward<UF>(*rhs).
// Otherwise, direct-non-list-initializes unex with utl::forward<GF>(rhs.error()).
//
// Postconditions: rhs.has_value() is unchanged; rhs.has_value() == this->has_value() is true.
//
// Throws: Any exception thrown by the initialization of val or unex.
//
// Remarks: The expression inside explicit is equivalent to !is_convertible_v<UF, T> ||
// !is_convertible_v<GF, E>.

#include "utl/utl_config.h"

#include "tests/expected/test_types.h"
#include "tests/test_macros.h"
#include "utl/expected/utl_expected.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_trivially_copy_constructible.h"

#include <cassert>

namespace expected {
// Test Constraints:
template <class T1, class Err1, class T2, class Err2>
using canCstrFromExpected =
    utl::is_constructible<utl::expected<T1, Err1>, utl::expected<T2, Err2> const&>;

struct CtorFromInt {
    CtorFromInt(int);
};

static_assert(canCstrFromExpected<CtorFromInt, int, int, int>::value, "");

struct NoCtorFromInt {};

// !is_constructible_v<T, UF>
static_assert(!canCstrFromExpected<NoCtorFromInt, int, int, int>::value, "");

// !is_constructible_v<E, GF>
static_assert(!canCstrFromExpected<int, NoCtorFromInt, int, int>::value, "");

template <class T, bool = utl::is_same_v<T, int>>
struct CtorFrom {
    explicit CtorFrom(int);

    explicit CtorFrom(T);
    template <typename U>
    explicit CtorFrom(U&&) = delete;
};

template <class T>
struct CtorFrom<T, true> {
    explicit CtorFrom(T);
    template <typename U>
    explicit CtorFrom(U&&) = delete;
};

// is_constructible_v<T, expected<U, G>&>
static_assert(!canCstrFromExpected<CtorFrom<utl::expected<int, int>&>, int, int, int>::value, "");

// is_constructible_v<T, expected<U, G>>
static_assert(!canCstrFromExpected<CtorFrom<utl::expected<int, int>&&>, int, int, int>::value, "");

// is_constructible_v<T, expected<U, G>&>
// note that this is true because it is covered by the other overload
//   template<class U = T> constexpr explicit(see below) expected(U&& v);
// The fact that it is not ambiguous proves that the overload under testing is removed
static_assert(
    canCstrFromExpected<CtorFrom<utl::expected<int, int> const&>, int, int, int>::value, "");

// is_constructible_v<T, expected<U, G>>
static_assert(
    !canCstrFromExpected<CtorFrom<utl::expected<int, int> const&&>, int, int, int>::value, "");

template <class T, bool = utl::is_same_v<T, int>>
struct ConvertFrom {
    ConvertFrom(int);
    ConvertFrom(T);
    template <typename U>
    ConvertFrom(U&&) = delete;
};

template <class T>
struct ConvertFrom<T, true> {
    ConvertFrom(T);
    template <typename U>
    ConvertFrom(U&&) = delete;
};

// is_convertible_v<expected<U, G>&, T>
static_assert(
    !canCstrFromExpected<ConvertFrom<utl::expected<int, int>&>, int, int, int>::value, "");

// is_convertible_v<expected<U, G>&&, T>
static_assert(
    !canCstrFromExpected<ConvertFrom<utl::expected<int, int>&&>, int, int, int>::value, "");

// is_convertible_v<const expected<U, G>&, T>
// note that this is true because it is covered by the other overload
//   template<class U = T> constexpr explicit(see below) expected(U&& v);
// The fact that it is not ambiguous proves that the overload under testing is removed
static_assert(
    canCstrFromExpected<ConvertFrom<utl::expected<int, int> const&>, int, int, int>::value, "");

// is_convertible_v<const expected<U, G>&&, T>
static_assert(
    !canCstrFromExpected<ConvertFrom<utl::expected<int, int> const&&>, int, int, int>::value, "");

// Note for below 4 tests, because their E is constructible from cvref of utl::expected<int, int>,
// unexpected<E> will be constructible from cvref of utl::expected<int, int>
// is_constructible_v<unexpected<E>, expected<U, G>&>
static_assert(!canCstrFromExpected<int, CtorFrom<utl::expected<int, int>&>, int, int>::value, "");

// is_constructible_v<unexpected<E>, expected<U, G>>
static_assert(!canCstrFromExpected<int, CtorFrom<utl::expected<int, int>&&>, int, int>::value, "");

// is_constructible_v<unexpected<E>, const expected<U, G>&> is false
static_assert(
    !canCstrFromExpected<int, CtorFrom<utl::expected<int, int> const&>, int, int>::value, "");

// is_constructible_v<unexpected<E>, const expected<U, G>>
static_assert(
    !canCstrFromExpected<int, CtorFrom<utl::expected<int, int> const&&>, int, int>::value, "");

// test explicit
static_assert(
    utl::is_convertible<utl::expected<int, int> const&, utl::expected<short, long>>::value, "");

// !is_convertible_v<UF, T>
static_assert(
    utl::is_constructible<utl::expected<CtorFrom<int>, int>, utl::expected<int, int> const&>::value,
    "");
static_assert(
    !utl::is_convertible<utl::expected<int, int> const&, utl::expected<CtorFrom<int>, int>>::value,
    "");

// !is_convertible_v<GF, E>.
static_assert(
    utl::is_constructible<utl::expected<int, CtorFrom<int>>, utl::expected<int, int> const&>::value,
    "");
static_assert(
    !utl::is_convertible<utl::expected<int, int> const&, utl::expected<int, CtorFrom<int>>>::value,
    "");

struct Data {
    int i;
    constexpr Data(int ii) : i(ii) {}
};

constexpr bool test() {
    // convert the value
    {
        utl::expected<int, int> const e1(5);
        utl::expected<Data, int> e2 = e1;
        assert(e2.has_value());
        assert(e2.value().i == 5);
        assert(e1.has_value());
        assert(e1.value() == 5);
    }

    // convert the error
    {
        utl::expected<int, int> const e1(utl::unexpect, 5);
        utl::expected<int, Data> e2 = e1;
        assert(!e2.has_value());
        assert(e2.error().i == 5);
        assert(!e1.has_value());
        assert(e1.error() == 5);
    }

    // convert TailClobberer
    {
        utl::expected<TailClobbererNonTrivialMove<0>, char> const e1;
        utl::expected<TailClobberer<0>, char> e2 = e1;
        assert(e2.has_value());
        assert(e1.has_value());
    }

    return true;
}

void testException() {
#if UTL_WITH_EXCEPTIONS
    struct ThrowingInt {
        ThrowingInt(int) { throw Except{}; }
    };

    // throw on converting value
    {
        utl::expected<int, int> const e1;
        try {
            UTL_ATTRIBUTE(MAYBE_UNUSED) utl::expected<ThrowingInt, int> e2 = e1;
            assert(false);
        } catch (Except) {}
    }

    // throw on converting error
    {
        utl::expected<int, int> const e1(utl::unexpect);
        try {
            UTL_ATTRIBUTE(MAYBE_UNUSED) utl::expected<int, ThrowingInt> e2 = e1;
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
