// Adapted from the LLVM Project, Copyright 2023-2024 Bryan Wong
// Commit: 99696b35bc8a0054e0b0c1a26e8dd5049fa8c41b

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// <span>

//  constexpr span(const span& other) noexcept = default;

#include "tests/test_macros.h"
#include "utl/span/utl_span.h"
#include "utl/string/utl_basic_short_string.h"
#include "utl/type_traits/utl_declval.h"

#include <cassert>

namespace span {

template <class T>
constexpr void test() {
    ASSERT_NOEXCEPT(utl::span<T>(utl::declval<utl::span<T> const&>()));
    ASSERT_NOEXCEPT(utl::span<T>{utl::declval<utl::span<T> const&>()});

    // dynamic_extent
    {
        utl::span<T> x;
        utl::span<T> copy(x);
        assert(copy.data() == x.data());
        assert(copy.size() == x.size());
    }
    {
        T array[3] = {};
        utl::span<T> x(array, 3);
        utl::span<T> copy(x);
        assert(copy.data() == array);
        assert(copy.size() == 3);
    }
    {
        T array[3] = {};
        utl::span<T> x(array, 2);
        utl::span<T> copy(x);
        assert(copy.data() == array);
        assert(copy.size() == 2);
    }

    // static extent
    {
        utl::span<T, 0> x;
        utl::span<T, 0> copy(x);
        assert(copy.data() == x.data());
        assert(copy.size() == x.size());
    }
    {
        T array[3] = {};
        utl::span<T, 3> x(array);
        utl::span<T, 3> copy(x);
        assert(copy.data() == array);
        assert(copy.size() == 3);
    }
    {
        T array[2] = {};
        utl::span<T, 2> x(array);
        utl::span<T, 2> copy(x);
        assert(copy.data() == array);
        assert(copy.size() == 2);
    }
}

struct Foo {};

constexpr bool test_all() {
    test<int>();
    test<int const>();
    test<int volatile>();
    test<int const volatile>();

    test<long>();
    test<long const>();
    test<long volatile>();
    test<long const volatile>();

    test<double>();
    test<double const>();
    test<double volatile>();
    test<double const volatile>();

    // Note: Can't test non-fundamental types with volatile because we require `T*` to be
    // indirectly_readable,
    //       which isn't the case when T is volatile.
    test<Foo>();
    test<Foo const>();

    test<utl::string>();
    test<utl::string const>();

    // Regression test for https://github.com/llvm/llvm-project/issues/104496
    {
        struct Incomplete;
        utl::span<Incomplete> x;
        utl::span<Incomplete> copy(x);
        assert(copy.data() == x.data());
        assert(copy.size() == x.size());
    }

    return true;
}

void copy_test_driver() {
    test_all();
#if UTL_CXX20
    static_assert(test_all());
#endif
}
} // namespace span

int main() {
    span::copy_test_driver();
}