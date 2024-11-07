// Copyright 2023-2024 Bryan Wong

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <span>

//   template<class It, class EndOrSize>
//     span(It, EndOrSize) -> span<remove_reference_t<iter_reference_t<_It>>>;
//
//   template<class T, size_t N>
//     span(T (&)[N]) -> span<T, N>;
//
//   template<class T, size_t N>
//     span(array<T, N>&) -> span<T, N>;
//
//   template<class T, size_t N>
//     span(const array<T, N>&) -> span<const T, N>;
//
//   template<class R>
//     span(R&&) -> span<remove_reference_t<ranges::range_reference_t<R>>>;

#include "utl/utl_config.h"

#if UTL_CXX17

#  include "tests/test_macros.h"
#  include "utl/iterator/utl_begin.h"
#  include "utl/iterator/utl_data.h"
#  include "utl/iterator/utl_end.h"
#  include "utl/iterator/utl_size.h"
#  include "utl/span/utl_span.h"
#  include "utl/string/utl_basic_short_string.h"
#  include "utl/type_traits/utl_declval.h"

#  include <array>
#  include <cassert>
namespace span {
namespace deduct {

void test_iterator_sentinel() {
    int arr[] = {1, 2, 3};
    {
        utl::span s{utl::begin(arr), utl::end(arr)};
        ASSERT_SAME_TYPE(decltype(s), utl::span<int>);
        assert(s.size() == utl::size(arr));
        assert(s.data() == utl::data(arr));
    }
    {
        utl::span s{utl::begin(arr), 3};
        ASSERT_SAME_TYPE(decltype(s), utl::span<int>);
        assert(s.size() == utl::size(arr));
        assert(s.data() == utl::data(arr));
    }

    {
        utl::span s{utl::begin(arr), utl::integral_constant<size_t, 3>{}};
        ASSERT_SAME_TYPE(decltype(s), utl::span<int, 3>);
        assert(s.size() == utl::size(arr));
        assert(s.data() == utl::data(arr));
    }
}

void test_c_array() {
    {
        int arr[] = {1, 2, 3};
        utl::span s{arr};
        ASSERT_SAME_TYPE(decltype(s), utl::span<int, 3>);
        assert(s.size() == utl::size(arr));
        assert(s.data() == utl::data(arr));
    }

    {
        int const arr[] = {1, 2, 3};
        utl::span s{arr};
        ASSERT_SAME_TYPE(decltype(s), utl::span<int const, 3>);
        assert(s.size() == utl::size(arr));
        assert(s.data() == utl::data(arr));
    }
}

void test_std_array() {
    {
        std::array<double, 4> arr = {1.0, 2.0, 3.0, 4.0};
        utl::span s{arr};
        ASSERT_SAME_TYPE(decltype(s), utl::span<double, 4>);
        assert(s.size() == arr.size());
        assert(s.data() == arr.data());
    }

    {
        std::array<long, 5> const arr = {4, 5, 6, 7, 8};
        utl::span s{arr};
        ASSERT_SAME_TYPE(decltype(s), utl::span<long const, 5>);
        assert(s.size() == arr.size());
        assert(s.data() == arr.data());
    }
}

#  if 0
// Unimplemented
void test_range_std_container() {
    {
        utl::string str{"ABCDE"};
        utl::span s{str};
        ASSERT_SAME_TYPE(decltype(s), utl::span<char>);
        assert(s.size() == str.size());
        assert(s.data() == str.data());
    }

    {
        utl::string const str{"QWERTYUIOP"};
        utl::span s{str};
        ASSERT_SAME_TYPE(decltype(s), utl::span<char const>);
        assert(s.size() == str.size());
        assert(s.data() == str.data());
    }
}
#  else
void test_range_std_container() {}
#  endif

void deduct_run_test() {
    test_iterator_sentinel();
    test_c_array();
    test_std_array();
    test_range_std_container();
}

} // namespace deduct
} // namespace span

#endif