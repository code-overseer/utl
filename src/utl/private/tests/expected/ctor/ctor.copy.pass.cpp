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

// constexpr expected(const expected& rhs);
//
// Effects: If rhs.has_value() is true, direct-non-list-initializes val with *rhs.
// Otherwise, direct-non-list-initializes unex with rhs.error().
//
// Postconditions: rhs.has_value() == this->has_value().
//
// Throws: Any exception thrown by the initialization of val or unex.
//
// Remarks: This constructor is defined as deleted unless
// - is_copy_constructible<T>::value is true and
// - is_copy_constructible<E>::value is true.
//
// This constructor is trivial if
// - is_trivially_copy_constructible<T>::value is true and
// - is_trivially_copy_constructible<E>::value is true.

#include "utl/utl_config.h"

#include "tests/expected/test_types.h"
#include "tests/test_macros.h"
#include "utl/expected/utl_expected.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_trivially_copy_constructible.h"

#include <cassert>

namespace expected {
struct NonCopyable {
    NonCopyable(NonCopyable const&) = delete;
};

struct CopyableNonTrivial {
    int i;
    constexpr CopyableNonTrivial(int ii) : i(ii) {}
    constexpr CopyableNonTrivial(CopyableNonTrivial const& o) : i(o.i) {}
    friend constexpr bool operator==(CopyableNonTrivial const& l, CopyableNonTrivial const& r) {
        return l.i == r.i;
    }
    friend constexpr bool operator!=(CopyableNonTrivial const& l, CopyableNonTrivial const& r) {
        return l.i != r.i;
    }
};

// Test: This constructor is defined as deleted unless
// - is_copy_constructible<T>::value is true and
// - is_copy_constructible<E>::value is true.
static_assert(utl::is_copy_constructible<utl::expected<int, int>>::value, "");
static_assert(utl::is_copy_constructible<utl::expected<CopyableNonTrivial, int>>::value, "");
static_assert(utl::is_copy_constructible<utl::expected<int, CopyableNonTrivial>>::value, "");
static_assert(
    utl::is_copy_constructible<utl::expected<CopyableNonTrivial, CopyableNonTrivial>>::value, "");
static_assert(!utl::is_copy_constructible<utl::expected<NonCopyable, int>>::value, "");
static_assert(!utl::is_copy_constructible<utl::expected<int, NonCopyable>>::value, "");
static_assert(!utl::is_copy_constructible<utl::expected<NonCopyable, NonCopyable>>::value, "");

// Test: This constructor is trivial if
// - is_trivially_copy_constructible<T>::value is true and
// - is_trivially_copy_constructible<E>::value is true.
static_assert(utl::is_trivially_copy_constructible<utl::expected<int, int>>::value, "");
static_assert(
    !utl::is_trivially_copy_constructible<utl::expected<CopyableNonTrivial, int>>::value, "");
static_assert(
    !utl::is_trivially_copy_constructible<utl::expected<int, CopyableNonTrivial>>::value, "");
static_assert(!utl::is_trivially_copy_constructible<
                  utl::expected<CopyableNonTrivial, CopyableNonTrivial>>::value,
    "");

struct Any {
    constexpr Any() = default;
    constexpr Any(Any const&) = default;
    constexpr Any& operator=(Any const&) = default;

    template <class T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_same(Any, utl::decay_t<T>) &&
        UTL_TRAIT_is_copy_constructible(utl::decay_t<T>))>
    UTL_CONSTRAINT_CXX20(!UTL_TRAIT_is_same(Any, utl::decay_t<T>) &&
        UTL_TRAIT_is_copy_constructible(utl::decay_t<T>))
    constexpr Any(T&&) {}
};

constexpr bool test() {
    // copy the value non-trivial
    {
        utl::expected<CopyableNonTrivial, int> const e1(5);
        auto e2 = e1;
        assert(e2.has_value());
        assert(e2.value().i == 5);
    }

    // copy the error non-trivial
    {
        utl::expected<int, CopyableNonTrivial> const e1(utl::unexpect, 5);
        auto e2 = e1;
        assert(!e2.has_value());
        assert(e2.error().i == 5);
    }

    // copy the value trivial
    {
        utl::expected<int, int> const e1(5);
        auto e2 = e1;
        assert(e2.has_value());
        assert(e2.value() == 5);
    }

    // copy the error trivial
    {
        utl::expected<int, int> const e1(utl::unexpect, 5);
        auto e2 = e1;
        assert(!e2.has_value());
        assert(e2.error() == 5);
    }

    // copy TailClobberer as value
    {
        utl::expected<TailClobberer<0>, bool> const e1;
        auto e2 = e1;
        assert(e2.has_value());
    }

    // copy TailClobberer as error
    {
        utl::expected<bool, TailClobberer<1>> const e1(utl::unexpect);
        auto e2 = e1;
        assert(!e2.has_value());
    }

    {
        // TODO(LLVM 20): Remove once we drop support for Clang 17
#if UTL_COMPILER_CLANG_AT_LEAST(20, 0, 0)
        // https://github.com/llvm/llvm-project/issues/92676
        utl::expected<Any, int> e1;
        auto e2 = e1;
        assert(e2.has_value());
#endif
    }

    return true;
}

void testException() {
#if UTL_WITH_EXCEPTIONS
    struct Throwing {
        Throwing() = default;
        Throwing(Throwing const&) { throw Except{}; }
    };

    // throw on copying value
    {
        utl::expected<Throwing, int> const e1;
        try {
            UTL_ATTRIBUTE(MAYBE_UNUSED) auto e2 = e1;
            assert(false);
        } catch (Except) {}
    }

    // throw on copying error
    {
        utl::expected<int, Throwing> const e1(utl::unexpect);
        try {
            UTL_ATTRIBUTE(MAYBE_UNUSED) auto e2 = e1;
            assert(false);
        } catch (Except) {}
    }

#endif // TEST_HAS_NO_EXCEPTIONS
}
} // namespace expected

int main(int, char**) {
    expected::test();
#if UTL_CXX17
    // Certain ctors are only constexpr in C++17
    static_assert(expected::test(), "");
#endif
    expected::testException();
    return 0;
}
