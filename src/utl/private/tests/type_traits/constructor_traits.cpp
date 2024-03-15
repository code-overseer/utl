// Copyright 2023-2024 Bryan Wong

#include "utl/type_traits/utl_is_x_constructible.h"

#include <initializer_list>

namespace constructor_traits_test {
/**
 * Implicit constructibles
 */
struct A {
    A() = default;
    A(A const&) = default;
    A(A&&) = default;
    template <typename... T>
    A(T&&...) {}
};

/**
 * Explicit constructibles
 */
struct B {
    explicit B() = default;
    B(B const&) = default;
    B(B&&) = default;
    template <typename... T>
    explicit B(T&&...) {}
};

struct C {
    C(std::initializer_list<int>) {}
};

struct D {
    explicit D(std::initializer_list<int>) {}
};

static_assert(utl::is_implicit_constructible<A>::value, "Should be implicit");
static_assert(utl::is_implicit_constructible<A, A>::value, "Should be implicit");
static_assert(utl::is_implicit_constructible<A, A const&>::value, "Should be implicit");
static_assert(utl::is_implicit_constructible<A, int, float, char, A>::value, "Should be implicit");

static_assert(!utl::is_implicit_constructible<B>::value, "Should be explicit");
static_assert(!utl::is_implicit_constructible<B, int, float, char, A>::value, "Should be explicit");
static_assert(utl::is_explicit_constructible<B>::value, "Should be explicit");
static_assert(utl::is_explicit_constructible<B, double, short, unsigned char, A>::value,
    "Should be explicit");

static_assert(!utl::is_implicit_constructible<C>::value, "Should not be constructible");
static_assert(!utl::is_implicit_constructible<D>::value, "Should not be constructible");
static_assert(!utl::is_explicit_constructible<C>::value, "Should not be constructible");
static_assert(!utl::is_explicit_constructible<D>::value, "Should not be constructible");

} // namespace constructor_traits_test
