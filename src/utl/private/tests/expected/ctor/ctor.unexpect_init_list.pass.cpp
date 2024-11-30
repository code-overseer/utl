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

// template<class U, class... Args>
//   constexpr explicit expected(unexpect_t, initializer_list<U> il, Args&&... args);
//
// Constraints: is_constructible_v<E, initializer_list<U>&, Args...> is true.
//
// Effects: Direct-non-list-initializes unex with il, utl::forward<Args>(args)....
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
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_trivially_move_constructible.h"
#include "utl/utility/utl_forward.h"

#include <cassert>
#include <vector>

namespace expected {
// Test Constraints:
static_assert(utl::is_constructible<utl::expected<int, std::vector<int>>, utl::unexpect_t,
                  std::initializer_list<int>>::value,
    "");

// !is_constructible_v<T, initializer_list<U>&, Args...>
static_assert(!utl::is_constructible<utl::expected<int, int>, utl::unexpect_t,
                  std::initializer_list<int>>::value,
    "");

static_assert(!utl::is_implicit_constructible<utl::expected<int, std::vector<int>>, utl::unexpect_t,
                  std::initializer_list<int>>::value,
    "");

template <class... Ts>
struct Data {
    std::vector<int> vec_;
    utl::tuple<Ts...> tuple_;

    template <class... Us,
        typename = utl::enable_if_t<utl::is_constructible<utl::tuple<Ts...>, Us&&...>::value>>
    constexpr Data(std::initializer_list<int> il, Us&&... us)
        : vec_(il)
        , tuple_(utl::forward<Us>(us)...) {}
};

UTL_CONSTEXPR_CXX23 bool test() {
    // no arg
    {
        utl::expected<int, Data<>> e(utl::unexpect, {1, 2, 3});
        assert(!e.has_value());
        // auto expectedList = {1, 2, 3};
        // assert(utl::ranges::equal(e.error().vec_, expectedList));
    }

    // one arg
    {
        utl::expected<int, Data<MoveOnly>> e(utl::unexpect, {4, 5, 6}, MoveOnly(5));
        assert(!e.has_value());
        // auto expectedList = {4, 5, 6};
        // assert((utl::ranges::equal(e.error().vec_, expectedList)));
        assert(utl::get<0>(e.error().tuple_) == 5);
    }

    // multi args
    {
        int i = 5;
        int j = 6;
        MoveOnly m(7);
        utl::expected<int, Data<int&, int&&, MoveOnly>> e(
            utl::unexpect, {1, 2}, i, utl::move(j), utl::move(m));
        assert(!e.has_value());
        // auto expectedList = {1, 2};
        // assert((utl::ranges::equal(e.error().vec_, expectedList)));
        assert(&utl::get<0>(e.error().tuple_) == &i);
        assert(&utl::get<1>(e.error().tuple_) == &j);
        assert(utl::get<2>(e.error().tuple_) == 7);
        assert(m.get() == 0);
    }

    // TailClobberer
    {
        utl::expected<bool, TailClobberer<1>> e(utl::unexpect, {1, 2, 3});
        assert(!e.has_value());
    }

    return true;
}

void testException() {
#if UTL_WITH_EXCEPTIONS
    struct Throwing {
        Throwing(std::initializer_list<int>, int) { throw Except{}; };
    };

    try {
        utl::expected<int, Throwing> u(utl::unexpect, {1, 2}, 5);
        assert(false);
    } catch (Except) {}
#endif // TEST_HAS_NO_EXCEPTIONS
}

} // namespace expected

int main(int, char**) {
    expected::test();
#if UTL_CXX23
    static_assert(expected::test());
#endif
    expected::testException();
    return 0;
}
