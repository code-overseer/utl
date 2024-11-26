/**
 * Copyright 2023-2024 Bryan Wong
 * Credit: This file contains code derived from the GCC test suite.
 */

#include "utl/ranges/utl_swap.h"
#include "utl/string/utl_basic_short_string.h"
#include "utl/utility/utl_swap.h"

#include <string>

UTL_DISABLE_WARNING_PUSH()
#if UTL_COMPILER_GNU_BASED
UTL_DISABLE_WARNING("-Wunused-function")
#endif

namespace {
struct non_default_ctor {
    non_default_ctor(int) {}
    non_default_ctor(non_default_ctor const&) {}
    non_default_ctor& operator=(non_default_ctor const&) = default;
    non_default_ctor& operator++() { return *this; }
};
inline bool operator==(non_default_ctor const&, non_default_ctor const&) {
    return false;
}

inline bool operator<(non_default_ctor const&, non_default_ctor const&) {
    return false;
}

inline non_default_ctor operator+(non_default_ctor const&, non_default_ctor const&) {
    return non_default_ctor(1);
}

inline non_default_ctor operator-(non_default_ctor const&, non_default_ctor const&) {
    return non_default_ctor(1);
}

inline non_default_ctor operator*(non_default_ctor const&, non_default_ctor const&) {
    return non_default_ctor(1);
}

struct pod {
    int x;
};
} // namespace

UTL_NAMESPACE_BEGIN
template void swap(non_default_ctor&, non_default_ctor&);
template void swap(pod&, pod&);
UTL_NAMESPACE_END

static_assert(noexcept(utl::swap(utl::declval<int&>(), utl::declval<int&>())), "Error");
static_assert(noexcept(utl::swap(utl::declval<int (&)[5]>(), utl::declval<int (&)[5]>())), "Error");
static_assert(noexcept(utl::ranges::swap(utl::declval<int&>(), utl::declval<int&>())), "Error");
static_assert(
    noexcept(utl::ranges::swap(utl::declval<int (&)[5]>(), utl::declval<int (&)[5]>())), "Error");

void array_test() {
    int x[2][3];
    int y[2][3];
    utl::ranges::swap(x, y);
    utl::swap(x, y);

    using utl::swap;
    swap(x, y);
}

#if UTL_CXX14
namespace {
constexpr bool constexpr_test_utility() {
    auto ok = true;

    double asc = 3.1415;
    double bsc = 2.7182;
    utl::swap(asc, bsc);
    ok = ok && asc == 2.7182 && bsc == 3.1415;

    float arr[5]{0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
    float brr[5]{5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    utl::swap(arr, brr);
    ok = ok && arr[2] == 7.0f && brr[2] == 2.0f;

    return ok;
}

constexpr bool constexpr_test_ranges() {
    auto ok = true;

    double asc = 3.1415;
    double bsc = 2.7182;
    utl::ranges::swap(asc, bsc);
    ok = ok && asc == 2.7182 && bsc == 3.1415;

    float arr[5]{0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
    float brr[5]{5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    utl::ranges::swap(arr, brr);
    ok = ok && arr[2] == 7.0f && brr[2] == 2.0f;

    return ok;
}
} // namespace

static_assert(constexpr_test_utility(), "");
static_assert(constexpr_test_ranges(), "");
#endif

#include <string>

namespace {
template <typename T>
class utility_W {
public:
    T data;
};

template <typename T>
class ranges_W {
public:
    T data;
};

template <typename T>
class adl_W {
public:
    T data;
    friend void swap(adl_W& l, adl_W& r) { utl::ranges::swap(l.data, r.data); }
};

template <class T>
void swap(utility_W<T>& x, utility_W<T>& y) {
    using utl::swap;
    swap(x.data, y.data);
}

template <class T>
void swap(ranges_W<T>& x, ranges_W<T>& y) {
    utl::ranges::swap(x.data, y.data);
}

// DR 809. swap should be overloaded for array types.
void defect_809() {
    utility_W<utl::string[8]> w1, w2; // Two objects of a Swappable type.
    using utl::swap;
    swap(w1, w2);
}

void defect_809_ranges() {
    ranges_W<utl::string[8]> w1, w2;
    utl::ranges::swap(w1, w2);
}

void std_defect_809() {
    utility_W<std::string[8]> w1, w2; // Two objects of a Swappable type.
    utl::swap(w1, w2);
}

void std_defect_809_ranges() {
    ranges_W<std::string[8]> w1, w2;
    utl::ranges::swap(w1, w2);
}

void adl_test() {
    adl_W<utl::string[8]> w1, w2;
    utl::ranges::swap(w1, w2);
}

UTL_DISABLE_WARNING_POP()
} // namespace
