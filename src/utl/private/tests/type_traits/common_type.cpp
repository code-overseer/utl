// Copyright 2023-2024 Bryan Wong

// Portions of this file are derived from LLVM, which is under the Apache License v2.0 with LLVM
// Exceptions copyrighted by the LLVM project.

#include "utl/tuple/utl_tuple.h"
#include "utl/type_traits/utl_common_type.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/utility/utl_pair.h"

struct E {};

template <class T>
struct X {
    explicit X(T const&) {}
};

template <class T>
struct S {
    explicit S(T const&) {}
};

template <class T>
struct bad_reference_wrapper {
    bad_reference_wrapper(T&);
    bad_reference_wrapper(T&&) = delete;
    operator T&() const;
};

namespace std {
template <typename T>
struct common_type<T, ::S<T>> {
    typedef S<T> type;
};

template <class T>
struct common_type<::S<T>, T> {
    typedef S<T> type;
};

//  P0548
template <class T>
struct common_type<::S<T>, ::S<T>> {
    typedef S<T> type;
};

template <>
struct common_type<::S<long>, long> {};
template <>
struct common_type<long, ::S<long>> {};
template <>
struct common_type<::X<double>, ::X<double>> {};
} // namespace std

template <class>
struct VoidT {
    typedef void type;
};

template <class Tp>
struct always_bool_imp {
    using type = bool;
};
template <class Tp>
using always_bool = typename always_bool_imp<Tp>::type;

template <class... Args>
constexpr auto no_common_type_imp(int) -> always_bool<typename utl::common_type<Args...>::type> {
    return false;
}

template <class... Args>
constexpr bool no_common_type_imp(long) {
    return true;
}

template <class... Args>
using no_common_type = utl::integral_constant<bool, no_common_type_imp<Args...>(0)>;

template <class T1, class T2>
struct TernaryOp {
    static_assert((utl::is_same<typename utl::decay<T1>::type, T1>::value), "must be same");
    static_assert((utl::is_same<typename utl::decay<T2>::type, T2>::value), "must be same");
    typedef
        typename utl::decay<decltype(false ? utl::declval<T1>() : utl::declval<T2>())>::type type;
};

template <class T, class U, class Expect>
struct test_bullet_three_one_imp {
    typedef typename utl::decay<T>::type DT;
    typedef typename utl::decay<U>::type DU;
    static_assert((!utl::is_same<T, DT>::value || !utl::is_same<U, DU>::value), "");
    static_assert((utl::is_same<typename utl::common_type<T, U>::type, Expect>::value), "");
    static_assert((utl::is_same<typename utl::common_type<U, T>::type, Expect>::value), "");
    static_assert((utl::is_same<typename utl::common_type<T, U>::type,
                      typename utl::common_type<DT, DU>::type>::value),
        "");
};

struct A {};
struct B {};
struct C : B {};
template <>
struct utl::common_type<A, utl::tuple<B>> {
    using type = tuple<B>;
};

static_assert((utl::is_same<utl::common_type<int>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<char>::type, char>::value), "");
static_assert((utl::is_same<utl::common_type_t<int>, int>::value), "");
static_assert((utl::is_same<utl::common_type_t<char>, char>::value), "");

static_assert((utl::is_same<utl::common_type<int>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int const>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int volatile>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int const volatile>::type, int>::value), "");

static_assert((utl::is_same<utl::common_type<int, int>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int, int const>::type, int>::value), "");

static_assert((utl::is_same<utl::common_type<long, int const>::type, long>::value), "");
static_assert((utl::is_same<utl::common_type<long const, int>::type, long>::value), "");
static_assert((utl::is_same<utl::common_type<long, int volatile>::type, long>::value), "");
static_assert((utl::is_same<utl::common_type<long volatile, int>::type, long>::value), "");
static_assert((utl::is_same<utl::common_type<long const, int const>::type, long>::value), "");

static_assert((utl::is_same<utl::common_type<double, char>::type, double>::value), "");
static_assert((utl::is_same<utl::common_type<short, char>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type_t<double, char>, double>::value), "");
static_assert((utl::is_same<utl::common_type_t<short, char>, int>::value), "");

static_assert((utl::is_same<utl::common_type<double, char, long long>::type, double>::value), "");
static_assert(
    (utl::is_same<utl::common_type<unsigned, char, long long>::type, long long>::value), "");
static_assert((utl::is_same<utl::common_type_t<double, char, long long>, double>::value), "");
static_assert((utl::is_same<utl::common_type_t<unsigned, char, long long>, long long>::value), "");

static_assert((utl::is_same<utl::common_type<void>::type, void>::value), "");
static_assert((utl::is_same<utl::common_type<void const>::type, void>::value), "");
static_assert((utl::is_same<utl::common_type<void volatile>::type, void>::value), "");
static_assert((utl::is_same<utl::common_type<void const volatile>::type, void>::value), "");

static_assert((utl::is_same<utl::common_type<void, void const>::type, void>::value), "");
static_assert((utl::is_same<utl::common_type<void const, void>::type, void>::value), "");
static_assert((utl::is_same<utl::common_type<void, void volatile>::type, void>::value), "");
static_assert((utl::is_same<utl::common_type<void volatile, void>::type, void>::value), "");
static_assert((utl::is_same<utl::common_type<void const, void const>::type, void>::value), "");

static_assert((utl::is_same<utl::common_type<int, S<int>>::type, S<int>>::value), "");
static_assert((utl::is_same<utl::common_type<int, S<int>, S<int>>::type, S<int>>::value), "");
static_assert((utl::is_same<utl::common_type<int, int, S<int>>::type, S<int>>::value), "");

// (4.1)
// -- If sizeof...(T) is zero, there shall be no member type.
static_assert(no_common_type<>::value, "");

// (4.2)
// -- If sizeof...(T) is one, let T0 denote the sole type constituting the pack
//    T. The member typedef-name type shall denote the same type, if any, as
//    common_type_t<T0, T0>; otherwise there shall be no member type.
static_assert((utl::is_same<utl::common_type<void>::type, void>::value), "");
static_assert((utl::is_same<utl::common_type<int>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int const>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int volatile[]>::type, int volatile*>::value), "");
static_assert((utl::is_same<utl::common_type<void (&)()>::type, void (*)()>::value), "");

static_assert((no_common_type<X<double>>::value), "");
// (4.3)
// -- If sizeof...(T) is two, let the first and second types constituting T be
//    denoted by T1 and T2, respectively, and let D1 and D2 denote the same types
//    as decay_t<T1> and decay_t<T2>, respectively.
// (4.3.1)
//    -- If is_same<T1, D1> is false or is_same<T2, D2> is false, let C
//       denote the same type, if any, as common_type_t<D1, D2>.
// Test that the user provided specialization of common_type is used after
// decaying T1.
struct three_one_0 {
    typedef S<int> const T1;
    typedef int T2;
    static_assert(sizeof(test_bullet_three_one_imp<T1, T2, S<int>>), "");
};
// Test a user provided specialization that does not provide a typedef.
struct three_one_1 {
    typedef ::S<long> const T1;
    typedef long T2;
    static_assert((no_common_type<T1, T2>::value), "");
    static_assert((no_common_type<T2, T1>::value), "");
};
// Test that the ternary operator is not applied when the types are the
// same.
struct three_one_2 {
    typedef void const T1;
    typedef void Expect;
    static_assert((utl::is_same<utl::common_type<T1, T1>::type, Expect>::value), "");
    static_assert(
        (utl::is_same<utl::common_type<T1, T1>::type, utl::common_type<T1>::type>::value), "");
};
struct three_one_3 {
    typedef int const T1[];
    typedef int const* Expect;
    static_assert((utl::is_same<utl::common_type<T1, T1>::type, Expect>::value), "");
    static_assert(
        (utl::is_same<utl::common_type<T1, T1>::type, utl::common_type<T1>::type>::value), "");
};
// (4.3)
// -- If sizeof...(T) is two, let the first and second types constituting T be
//    denoted by T1 and T2, respectively, and let D1 and D2 denote the same types
//    as decay_t<T1> and decay_t<T2>, respectively.
// (4.3.1)
//    -- If [...]
// (4.3.2)
//    -- [Note: [...]
// (4.3.3)
//    -- Otherwise, if
//       decay_t<decltype(false ? declval<D1>() : declval<D2>())>
//       denotes a type, let C denote that type.
struct three_three_0 {
    typedef int const* T1;
    typedef int* T2;
    typedef TernaryOp<T1, T2>::type Expect;
    static_assert((utl::is_same<utl::common_type<T1, T2>::type, Expect>::value), "");
    static_assert((utl::is_same<utl::common_type<T2, T1>::type, Expect>::value), "");
};
// Test that there is no ::type member when the ternary op is ill-formed
struct three_three_1 {
    typedef int T1;
    typedef void T2;
    static_assert((no_common_type<T1, T2>::value), "");
    static_assert((no_common_type<T2, T1>::value), "");
};
struct three_three_2 {
    typedef int T1;
    typedef X<int> T2;
    static_assert((no_common_type<T1, T2>::value), "");
    static_assert((no_common_type<T2, T1>::value), "");
};
// Test that the ternary operator is not applied when the types are the
// same.
struct three_three_3 {
    typedef void T1;
    typedef void Expect;
    static_assert((utl::is_same<utl::common_type<T1, T1>::type, Expect>::value), "");
    static_assert(
        (utl::is_same<utl::common_type<T1, T1>::type, utl::common_type<T1>::type>::value), "");
};

// (4.3)
// -- If sizeof...(T) is two, let the first and second types constituting T be
//    denoted by T1 and T2, respectively, and let D1 and D2 denote the same types
//    as decay_t<T1> and decay_t<T2>, respectively.
// (4.3.1)
//    -- If [...]
// (4.3.2)
//    -- [Note: [...]
// (4.3.3)
//    -- Otherwise
// (4.3.4)
//    -- Otherwise, if COND-RES(CREF(D1), CREF(D2)) denotes a type, let C
//       denote the type decay_t<COND-RES(CREF(D1), CREF(D2))>.
static_assert(utl::is_same<utl::common_type_t<int, bad_reference_wrapper<int>>, int>::value, "");
static_assert(
    utl::is_same<utl::common_type_t<bad_reference_wrapper<double>, double>, double>::value, "");
static_assert(
    utl::is_same<utl::common_type_t<bad_reference_wrapper<double> const, double>, double>::value,
    "");
static_assert(
    utl::is_same<utl::common_type_t<bad_reference_wrapper<double> volatile, double>, double>::value,
    "");

static_assert(utl::is_same<utl::common_type_t<bad_reference_wrapper<double> const volatile, double>,
                  double>::value,
    "");

static_assert(
    utl::is_same<utl::common_type_t<bad_reference_wrapper<double>, double const>, double>::value,
    "");
static_assert(
    utl::is_same<utl::common_type_t<bad_reference_wrapper<double>, double volatile>, double>::value,
    "");
static_assert(utl::is_same<utl::common_type_t<bad_reference_wrapper<double>, double const volatile>,
                  double>::value,
    "");

static_assert(
    utl::is_same<utl::common_type_t<bad_reference_wrapper<double>&, double>, double>::value, "");
static_assert(
    utl::is_same<utl::common_type_t<bad_reference_wrapper<double>, double&>, double>::value, "");

// (4.4)
// -- If sizeof...(T) is greater than two, let T1, T2, and R, respectively,
// denote the first, second, and (pack of) remaining types constituting T.
// Let C denote the same type, if any, as common_type_t<T1, T2>. If there is
// such a type C, the member typedef-name type shall denote the
// same type, if any, as common_type_t<C, R...>. Otherwise, there shall be
// no member type.
static_assert((no_common_type<int, E>::value), "");
static_assert((no_common_type<int, int, E>::value), "");
static_assert((no_common_type<int, int, E, int>::value), "");
static_assert((no_common_type<int, int, int, E>::value), "");

// P0548
static_assert((utl::is_same<utl::common_type<S<int>>::type, S<int>>::value), "");
static_assert((utl::is_same<utl::common_type<S<int>, S<int>>::type, S<int>>::value), "");

static_assert((utl::is_same<utl::common_type<int>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int const>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int volatile>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int const volatile>::type, int>::value), "");

static_assert((utl::is_same<utl::common_type<int, int>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int const, int>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int, int const>::type, int>::value), "");
static_assert((utl::is_same<utl::common_type<int const, int const>::type, int>::value), "");

static_assert(
    utl::is_same<utl::common_type<int, int, int, int, int, int, int, int>::type, int>::value, "");

static_assert(utl::is_same<utl::common_type_t<utl::tuple<int>>, utl::tuple<int>>::value, "");
static_assert(
    utl::is_same<utl::common_type_t<utl::tuple<int>, utl::tuple<long>>, utl::tuple<long>>::value,
    "");
static_assert(utl::is_same<utl::common_type_t<utl::tuple<int const>, utl::tuple<int const>>,
                  utl::tuple<int>>::value,
    "");
static_assert(utl::is_same<utl::common_type_t<utl::tuple<int const&>>, utl::tuple<int>>::value, "");
static_assert(
    utl::is_same<
        utl::common_type_t<utl::tuple<int const volatile&>, utl::tuple<long const volatile&>>,
        utl::tuple<long>>::value,
    "");

static_assert(
    utl::is_same<utl::common_type_t<A, utl::tuple<B>, utl::tuple<C>>, utl::tuple<B>>::value, "");

static_assert(
    utl::is_same<utl::common_type_t<utl::pair<int, int>>, utl::pair<int, int>>::value, "");
static_assert(utl::is_same<utl::common_type_t<utl::pair<int, long>, utl::pair<long, int>>,
                  utl::pair<long, long>>::value,
    "");
static_assert(
    utl::is_same<
        utl::common_type_t<utl::pair<int const, long const>, utl::pair<int const, long const>>,
        utl::pair<int, long>>::value,
    "");

static_assert(utl::is_same<utl::common_type_t<utl::pair<int const&, long const&>>,
                  utl::pair<int, long>>::value,
    "");
static_assert(utl::is_same<utl::common_type_t<utl::pair<int const volatile&, long const volatile&>,
                               utl::pair<long const volatile&, int const volatile&>>,
                  utl::pair<long, long>>::value,
    "");

static_assert(
    utl::is_same<utl::common_type_t<A, utl::tuple<B>, utl::tuple<C>>, utl::tuple<B>>::value, "");
