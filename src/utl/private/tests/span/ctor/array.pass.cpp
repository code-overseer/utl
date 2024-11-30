// Adapted from the LLVM Project, Copyright 2023-2024 Bryan Wong
// Commit: 99696b35bc8a0054e0b0c1a26e8dd5049fa8c41b

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <span>

// template<size_t N>
//     constexpr span(element_type (&arr)[N]) noexcept;
//
// Remarks: These constructors shall not participate in overload resolution unless:
//   - extent == dynamic_extent || N == extent is true, and
//   - remove_pointer_t<decltype(data(arr))>(*)[] is convertible to ElementType(*)[].
//

#include "tests/test_macros.h"
#include "utl/span/utl_span.h"
#include "utl/string/utl_basic_short_string.h"
#include "utl/type_traits/utl_is_constructible.h"

#include <cassert>

namespace span {
void checkCV() {
    int arr[] = {1, 2, 3};
    int const carr[] = {4, 5, 6};
    int volatile varr[] = {7, 8, 9};
    int const volatile cvarr[] = {1, 3, 5};

    //  Types the same (dynamic sized)
    {
        utl::span<int> s1{arr};           // a span<               int> pointing at int.
        utl::span<int const> s2{carr};    // a span<const          int> pointing at const int.
        utl::span<int volatile> s3{varr}; // a span<      volatile int> pointing at volatile int.
        utl::span<int const volatile> s4{
            cvarr}; // a span<const volatile int> pointing at const volatile int.
        assert(s1.size() + s2.size() + s3.size() + s4.size() == 12);
    }

    //  Types the same (static sized)
    {
        utl::span<int, 3> s1{arr};           // a span<               int> pointing at int.
        utl::span<int const, 3> s2{carr};    // a span<const          int> pointing at const int.
        utl::span<int volatile, 3> s3{varr}; // a span<      volatile int> pointing at volatile int.
        utl::span<int const volatile, 3> s4{
            cvarr}; // a span<const volatile int> pointing at const volatile int.
        assert(s1.size() + s2.size() + s3.size() + s4.size() == 12);
    }

    //  types different (dynamic sized)
    {
        utl::span<int const> s1{arr};           // a span<const          int> pointing at int.
        utl::span<int volatile> s2{arr};        // a span<      volatile int> pointing at int.
        utl::span<int volatile> s3{arr};        // a span<      volatile int> pointing at const int.
        utl::span<int const volatile> s4{arr};  // a span<const volatile int> pointing at int.
        utl::span<int const volatile> s5{carr}; // a span<const volatile int> pointing at const int.
        utl::span<int const volatile> s6{
            varr}; // a span<const volatile int> pointing at volatile int.
        assert(s1.size() + s2.size() + s3.size() + s4.size() + s5.size() + s6.size() == 18);
    }

    //  types different (static sized)
    {
        utl::span<int const, 3> s1{arr};    // a span<const          int> pointing at int.
        utl::span<int volatile, 3> s2{arr}; // a span<      volatile int> pointing at int.
        utl::span<int volatile, 3> s3{arr}; // a span<      volatile int> pointing at const int.
        utl::span<int const volatile, 3> s4{arr}; // a span<const volatile int> pointing at int.
        utl::span<int const volatile, 3> s5{
            carr}; // a span<const volatile int> pointing at const int.
        utl::span<int const volatile, 3> s6{
            varr}; // a span<const volatile int> pointing at volatile int.
        assert(s1.size() + s2.size() + s3.size() + s4.size() + s5.size() + s6.size() == 18);
    }
}

template <class T>
constexpr bool testSpan() {
    T val[2] = {};

    ASSERT_NOEXCEPT(utl::span<T>{val});
    ASSERT_NOEXCEPT(utl::span<T, 2>{val});
    ASSERT_NOEXCEPT(utl::span<T const>{val});
    ASSERT_NOEXCEPT(utl::span<T const, 2>{val});

    utl::span<T> s1 = val;
    utl::span<T, 2> s2 = val;
    utl::span<T const> s3 = val;
    utl::span<T const, 2> s4 = val;
    assert(s1.data() == val && s1.size() == 2);
    assert(s2.data() == val && s2.size() == 2);
    assert(s3.data() == val && s3.size() == 2);
    assert(s4.data() == val && s4.size() == 2);

    utl::span<int const> s5 = {
        {1, 2}
    };
    utl::span<int const, 2> s6({1, 2});
    assert(s5.size() == 2); // and it dangles
    assert(s6.size() == 2); // and it dangles

    return true;
}

struct A {};

void array_test_driver() {
    testSpan<int>();
    testSpan<double>();
    testSpan<A>();
    testSpan<utl::string>();

    static_assert(testSpan<int>(), "");
    static_assert(testSpan<double>(), "");
    static_assert(testSpan<A>(), "");

    checkCV();

    // Size wrong
    { static_assert(!utl::is_constructible<utl::span<int, 2>, int(&)[3]>::value, ""); }

    // Type wrong
    {
        static_assert(!utl::is_constructible<utl::span<float>, int(&)[3]>::value, "");
        static_assert(!utl::is_constructible<utl::span<float, 3>, int(&)[3]>::value, "");
    }

    // CV wrong (dynamically sized)
    {
        static_assert(!utl::is_constructible<utl::span<int>, int const(&)[3]>::value, "");
        static_assert(!utl::is_constructible<utl::span<int>, int volatile(&)[3]>::value, "");
        static_assert(!utl::is_constructible<utl::span<int>, int const volatile(&)[3]>::value, "");

        static_assert(!utl::is_constructible<utl::span<int const>, int volatile(&)[3]>::value, "");
        static_assert(
            !utl::is_constructible<utl::span<int const>, int const volatile(&)[3]>::value, "");

        static_assert(!utl::is_constructible<utl::span<int volatile>, int const(&)[3]>::value, "");
        static_assert(
            !utl::is_constructible<utl::span<int volatile>, int const volatile(&)[3]>::value, "");
    }

    // CV wrong (statically sized)
    {
        static_assert(!utl::is_constructible<utl::span<int, 3>, int const(&)[3]>::value, "");
        static_assert(!utl::is_constructible<utl::span<int, 3>, int volatile(&)[3]>::value, "");
        static_assert(
            !utl::is_constructible<utl::span<int, 3>, int const volatile(&)[3]>::value, "");

        static_assert(
            !utl::is_constructible<utl::span<int const, 3>, int volatile(&)[3]>::value, "");
        static_assert(
            !utl::is_constructible<utl::span<int const, 3>, int const volatile(&)[3]>::value, "");

        static_assert(
            !utl::is_constructible<utl::span<int volatile, 3>, int const(&)[3]>::value, "");
        static_assert(
            !utl::is_constructible<utl::span<int volatile, 3>, int const volatile(&)[3]>::value,
            "");
    }
}
} // namespace span

#include <cstdio>
int main() {
    span::array_test_driver();
}
