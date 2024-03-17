// Copyright 2023-2024 Bryan Wong

#include "utl/preprocessor/utl_traits_check.h"
#include "utl/type_traits/utl_reference_constructs_from_temporary.h"

#if UTL_SUPPORTS_TRAIT(reference_constructs_from_temporary)

/**
 * Test lifted from GCC's test suite
 */

#  define SA(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct A {
    A();
};
struct B {
    operator A();
};
struct C {
    explicit C();
};
struct D {
    explicit operator A();
};
struct E {
    explicit operator A&();
};
struct F {
    explicit operator A&&();
};
// Could use a class template with explicit(bool), but then this would need
// C++20.
struct G {
    operator int();
    explicit operator int&&();
};
struct G2 {
    explicit operator int();
    operator int&&();
};
struct H {
    operator int();
    explicit operator int&();
};
struct H2 {
    explicit operator int();
    operator int&();
};

struct Base {};
struct Der : Base {};

template <typename T, typename RT>
struct morph {
    mutable T val{};
    operator RT() const { return static_cast<RT>(val); }
};

template <typename T>
using id = T;

// Built-in types.
SA(!utl::reference_constructs_from_temporary<int, int>::value);
SA(!utl::reference_constructs_from_temporary<int&, void>::value);
SA(!utl::reference_constructs_from_temporary<int&, void const>::value);
SA(!utl::reference_constructs_from_temporary<int&, void volatile>::value);
SA(!utl::reference_constructs_from_temporary<int&, void const volatile>::value);
SA(!utl::reference_constructs_from_temporary<void, void>::value);
SA(!utl::reference_constructs_from_temporary<void, int>::value);
SA(!utl::reference_constructs_from_temporary<int&, int>::value);
SA(!utl::reference_constructs_from_temporary<int&, int&>::value);
SA(!utl::reference_constructs_from_temporary<int&, int&&>::value);
SA(!utl::reference_constructs_from_temporary<int&, long>::value);

// static_assert(!utl::details::dangling::constructs_from_temp<int&, long&>::value, "");
// non-const lvalue reference to type 'int' cannot bind to a value of unrelated type 'long'
SA(!utl::reference_constructs_from_temporary<int&, long&>::value);
SA(!utl::reference_constructs_from_temporary<int&, long&&>::value);
SA(utl::reference_constructs_from_temporary<int const&, int>::value);
SA(!utl::reference_constructs_from_temporary<int const&, int&>::value);
SA(!utl::reference_constructs_from_temporary<int const&, int const&>::value);
SA(!utl::reference_constructs_from_temporary<int const&, int&&>::value);
SA(utl::reference_constructs_from_temporary<int const&, long>::value);
SA(utl::reference_constructs_from_temporary<int const&, long&>::value);
SA(utl::reference_constructs_from_temporary<int const&, long&&>::value);
SA(utl::reference_constructs_from_temporary<int&&, int>::value);
SA(!utl::reference_constructs_from_temporary<int&&, int&>::value);
SA(!utl::reference_constructs_from_temporary<int&&, int&&>::value);
SA(utl::reference_constructs_from_temporary<int&&, long>::value);
SA(utl::reference_constructs_from_temporary<int&&, long&>::value);
SA(utl::reference_constructs_from_temporary<int&&, long&&>::value);
SA(!utl::reference_constructs_from_temporary<unsigned int&, double>::value);
SA(!utl::reference_constructs_from_temporary<int volatile&, int>::value);
SA(!utl::reference_constructs_from_temporary<int const volatile&, int>::value);
SA(!utl::reference_constructs_from_temporary<int volatile&, int&>::value);
SA(!utl::reference_constructs_from_temporary<int const volatile&, int&>::value);
SA(!utl::reference_constructs_from_temporary<int volatile&, int&&>::value);
SA(!utl::reference_constructs_from_temporary<int const volatile&, int&&>::value);

// Classes.
SA(!utl::reference_constructs_from_temporary<A, A>::value);
// A& r(A{}); doesn't construct.
SA(!utl::reference_constructs_from_temporary<A&, A>::value);
SA(!utl::reference_constructs_from_temporary<A&, A&>::value);
SA(!utl::reference_constructs_from_temporary<A&, A&&>::value);
// Here we get const struct A & r = (const struct A &) &D.2414;
SA(utl::reference_constructs_from_temporary<A const&, A>::value);
SA(!utl::reference_constructs_from_temporary<A const&, A&>::value);
SA(!utl::reference_constructs_from_temporary<A const&, A const&>::value);
SA(!utl::reference_constructs_from_temporary<A const&, A&&>::value);
// Here we get struct A & r = (struct A &) &D.2439;
SA(utl::reference_constructs_from_temporary<A&&, A>::value);
SA(!utl::reference_constructs_from_temporary<A&&, A&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, A const&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, A&&>::value);

SA(!utl::reference_constructs_from_temporary<A, B>::value);
SA(!utl::reference_constructs_from_temporary<A&, B>::value);
SA(!utl::reference_constructs_from_temporary<A&, B&>::value);
SA(!utl::reference_constructs_from_temporary<A&, B const&>::value);
SA(!utl::reference_constructs_from_temporary<A&, B&&>::value);
SA(utl::reference_constructs_from_temporary<A const&, B>::value);
SA(utl::reference_constructs_from_temporary<A const&, B&>::value);
// Doesn't construct, so it's false.
SA(!utl::reference_constructs_from_temporary<A const&, B const&>::value);
SA(utl::reference_constructs_from_temporary<A const&, B&&>::value);
SA(utl::reference_constructs_from_temporary<A&&, B>::value);
SA(utl::reference_constructs_from_temporary<A&&, B&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, B const&>::value);
SA(utl::reference_constructs_from_temporary<A&&, B&&>::value);

SA(!utl::reference_constructs_from_temporary<A const&, C>::value);
SA(!utl::reference_constructs_from_temporary<A const&, C&>::value);

// Doesn't construct, so it's false.
SA(!utl::reference_constructs_from_temporary<int&, morph<int, int>>::value);
// Here we get
//   const int & r2 = D.2580 = morph<int, int>::operator int
//     (&TARGET_EXPR <D.2578, {.val=0}>); (const int &) &D.2580;
SA(utl::reference_constructs_from_temporary<int const&, morph<int, int>>::value);
SA(!utl::reference_constructs_from_temporary<int&, morph<int, int&>>::value);
SA(!utl::reference_constructs_from_temporary<int&, morph<int, int const&>>::value);
SA(!utl::reference_constructs_from_temporary<int&, morph<int, int&&>>::value);
SA(utl::reference_constructs_from_temporary<int const&, morph<long, long&>>::value);

// These are like const C& c(cref); so the explicit ctor C isn't a problem
// even in copy-init context.  const C& r = {}; would be a different story.
SA(!utl::reference_constructs_from_temporary<C, C>::value);
SA(!utl::reference_constructs_from_temporary<C&, C>::value);
SA(!utl::reference_constructs_from_temporary<C&, C&>::value);
SA(!utl::reference_constructs_from_temporary<C&, C&&>::value);
SA(utl::reference_constructs_from_temporary<C const&, C>::value);
SA(!utl::reference_constructs_from_temporary<C const&, C&>::value);
SA(!utl::reference_constructs_from_temporary<C const&, C const&>::value);
SA(!utl::reference_constructs_from_temporary<C const&, C&&>::value);
SA(utl::reference_constructs_from_temporary<C&&, C>::value);
SA(!utl::reference_constructs_from_temporary<C&&, C&>::value);
SA(!utl::reference_constructs_from_temporary<C&&, C const&>::value);
SA(!utl::reference_constructs_from_temporary<C&&, C&&>::value);

// These are all false ultimately because of CWG 2267, which we implement.
SA(!utl::reference_constructs_from_temporary<A, D>::value);
SA(!utl::reference_constructs_from_temporary<A&, D>::value);
SA(!utl::reference_constructs_from_temporary<A&, D&>::value);
SA(!utl::reference_constructs_from_temporary<A&, D const&>::value);
SA(!utl::reference_constructs_from_temporary<A&, D&&>::value);
SA(!utl::reference_constructs_from_temporary<A const&, D>::value);
SA(!utl::reference_constructs_from_temporary<A const&, D&>::value);
SA(!utl::reference_constructs_from_temporary<A const&, D const&>::value);
SA(!utl::reference_constructs_from_temporary<A const&, D&&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, D>::value);
SA(!utl::reference_constructs_from_temporary<A&&, D&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, D const&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, D&&>::value);

SA(!utl::reference_constructs_from_temporary<A, E>::value);
/* A& a1(E{}); compiles, but A& a2 = E{}; doesn't.
   With the former, we get A& a = E::operator A& (&TARGET_EXPR <D.2715, {}>)
   so we're not binding the reference to a temporary, although there is
   a temporary involved.  So the result is false in both copy- and direct-
   init, albeit for different reasons!  */
SA(!utl::reference_constructs_from_temporary<A&, E>::value);
// A& a = E::operator A& ((struct E *) r)); copy-init doesn't compile.
SA(!utl::reference_constructs_from_temporary<A&, E&>::value);
SA(!utl::reference_constructs_from_temporary<A&, E const&>::value);
SA(!utl::reference_constructs_from_temporary<A&, E&&>::value);
// direct-init:
// const A& a = (const struct A &) E::operator A& (&TARGET_EXPR <D.2720, {}>)
SA(!utl::reference_constructs_from_temporary<A const&, E>::value);
SA(!utl::reference_constructs_from_temporary<A const&, E&>::value);
SA(!utl::reference_constructs_from_temporary<A const&, E const&>::value);
SA(!utl::reference_constructs_from_temporary<A const&, E&&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, E>::value);
SA(!utl::reference_constructs_from_temporary<A&&, E&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, E const&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, E&&>::value);

SA(!utl::reference_constructs_from_temporary<A, F>::value);
// A& a1(F{}); and A& a2 = F{}; both invalid.
SA(!utl::reference_constructs_from_temporary<A&, F>::value);
SA(!utl::reference_constructs_from_temporary<A&, F&>::value);
SA(!utl::reference_constructs_from_temporary<A&, F const&>::value);
SA(!utl::reference_constructs_from_temporary<A&, F&&>::value);
SA(!utl::reference_constructs_from_temporary<A const&, F>::value);
SA(!utl::reference_constructs_from_temporary<A const&, F&>::value);
SA(!utl::reference_constructs_from_temporary<A const&, F const&>::value);
SA(!utl::reference_constructs_from_temporary<A const&, F&&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, F>::value);
SA(!utl::reference_constructs_from_temporary<A&&, F&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, F const&>::value);
SA(!utl::reference_constructs_from_temporary<A&&, F&&>::value);

/* This is where _converts_ and _constructs_ will differ:
   in direct-init we use G::operator int&& (no temporary),
   but in copy-init we use G::operator int, where a temporary is created
   to be bound to int&&.  */
SA(!utl::reference_constructs_from_temporary<int&&, G>::value);
// Similar to the previous one.
SA(!utl::reference_constructs_from_temporary<int const&, H>::value);
/* And here I've switched the explicit-ness.  In both copy- and direct-init
   we call operator int&, so no temporary.  */
SA(!utl::reference_constructs_from_temporary<int&&, G2>::value);
SA(!utl::reference_constructs_from_temporary<int const&, H2>::value);

SA(utl::reference_constructs_from_temporary<Base const&, Der>::value);

// This fails because std::is_constructible_v<int&&, id<int[3]>> is false.
SA(!utl::reference_constructs_from_temporary<int&&, id<int[3]>>::value);

// Arrays.
SA(!utl::reference_constructs_from_temporary<int, int[]>::value);
SA(!utl::reference_constructs_from_temporary<int[], int[]>::value);
SA(!utl::reference_constructs_from_temporary<int&, int[]>::value);
SA(!utl::reference_constructs_from_temporary<int&&, int[]>::value);
SA(!utl::reference_constructs_from_temporary<int const&, int[]>::value);

#endif
