// Copyright 2023-2024 Bryan Wong

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

//
// constexpr expected();

// Constraints: is_default_constructible_v<T> is true.
//
// Effects: Value-initializes val.
// Postconditions: has_value() is true.
//
// Throws: Any exception thrown by the initialization of val.

#include "utl/utl_config.h"

#include "tests/test_macros.h"
#include "utl/expected/utl_expected.h"
#include "utl/type_traits/utl_is_default_constructible.h"

#include <cassert>
#include <initializer_list>

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

namespace expected {
namespace ctor {

// Test constraints
static_assert(utl::is_default_constructible_v<utl::expected<int, int>>, "");
static_assert(!utl::is_default_constructible_v<utl::expected<NoDefaultCtor, int>>, "");
static_assert(utl::is_constructible_v<utl::details::expected::data_union<int, NoDefaultCtor>,
                  utl::in_place_t>,
    "");
struct MyInt {
    int i;
    friend constexpr bool operator==(MyInt const& left, MyInt const& right) noexcept {
        return left.i == right.i;
    }
    friend constexpr bool operator!=(MyInt const& left, MyInt const& right) noexcept {
        return left.i != right.i;
    }
};

template <class T, class E>
UTL_CONSTEXPR_CXX14 void testDefaultCtor() {
    utl::expected<T, E> e;
    assert(e.has_value());
    assert(e.value() == T());
}

template <class T>
UTL_CONSTEXPR_CXX14 void testTypes() {
    testDefaultCtor<T, bool>();
    testDefaultCtor<T, int>();
    testDefaultCtor<T, NoDefaultCtor>();
}

UTL_CONSTEXPR_CXX14 bool test() {
    testTypes<int>();
    testTypes<MyInt>();
    testTypes<TailClobberer<0>>();
    return true;
}

void testException() {
#ifdef UTL_WITH_EXCEPTIONS
    struct Throwing {
        Throwing() { throw Except{}; };
    };

    try {
        utl::expected<Throwing, int> u;
        assert(false);
    } catch (Except) {}
#endif // TEST_HAS_NO_EXCEPTIONS
}

void default_ctor(int, char**) {
    test();
#if UTL_CXX14
    static_assert(test(), "");
#endif
    testException();
}
} // namespace ctor
} // namespace expected
