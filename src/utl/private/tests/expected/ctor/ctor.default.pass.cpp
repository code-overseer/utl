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

//
// constexpr expected();

// Constraints: is_default_constructible<T>::value is true.
//
// Effects: Value-initializes val.
// Postconditions: has_value() is true.
//
// Throws: Any exception thrown by the initialization of val.

#include "utl/utl_config.h"

#include "tests/expected/test_types.h"
#include "tests/test_macros.h"
#include "utl/expected/utl_expected.h"
#include "utl/type_traits/utl_is_default_constructible.h"

#include <cassert>
#include <initializer_list>

namespace expected {

// Test constraints
static_assert(utl::is_default_constructible<utl::expected<int, int>>::value, "");
static_assert(!utl::is_default_constructible<utl::expected<NoDefaultCtor, int>>::value, "");

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
#if UTL_WITH_EXCEPTIONS
    struct Throwing {
        Throwing() { throw Except{}; };
    };

    try {
        utl::expected<Throwing, int> u;
        assert(false);
    } catch (Except) {}
#endif // TEST_HAS_NO_EXCEPTIONS
}

} // namespace expected

int main() {
    expected::test();
#if UTL_CXX14
    static_assert(expected::test(), "");
#endif
    expected::testException();
}
