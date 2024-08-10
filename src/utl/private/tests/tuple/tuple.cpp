// Copyright 2023-2024 Bryan Wong

#include "utl/tuple/utl_tuple.h"
#include "utl/type_traits/utl_std_traits.h"
UTL_DISABLE_WARNING_PUSH()
#if UTL_COMPILER_GNU_BASED
UTL_DISABLE_WARNING("-Wunused-const-variable")
#endif

namespace tuple_test {
template <int>
struct copy_only;
struct alloc;
} // namespace tuple_test

namespace utl {
template <int N>
struct uses_allocator<::tuple_test::copy_only<N>, ::tuple_test::alloc> : true_type {};
} // namespace utl

namespace tuple_test {

static constexpr utl::tuple<int, float> t(73, 0.2f);

#define STRIP(Value) utl::remove_cvref_t<decltype(Value)>

static_assert(utl::get<0>(t) == 73, "");
static_assert(utl::get<int>(t) == 73, "");

static_assert(utl::tuple_size<STRIP(t)>::value == 2, "tuple_size should return the expected size");
static_assert(utl::is_same<utl::tuple_element_t<0, STRIP(t)>, int>::value,
    "tuple_element should return the indexed element");
static_assert(utl::is_same<utl::tuple_element_t<1, STRIP(t)>, float>::value,
    "tuple_element should return the indexed element");
static_assert(!utl::tuple_traits::is_gettable<2, decltype(t)>::value, "Should not be gettable");
static_assert(!utl::tuple_traits::is_gettable<2, decltype(t)>::value, "Should not be gettable");

template <typename T, typename Tuple, typename = void>
struct not_gettable_by_type : utl::true_type {};
template <typename T, typename Tuple>
struct not_gettable_by_type<T, Tuple,
    utl::void_t<decltype(utl::get<T>(utl::declval<utl::remove_cvref_t<Tuple>>()))>> :
    utl::false_type {};
static_assert(not_gettable_by_type<short, decltype(t)>::value,
    "Should not be gettable by type if type does not exist");

static constexpr utl::tuple<long long, double> u(t);
static constexpr utl::tuple<int, short, char, long long> v(73, 2, 22, 2652547);
static constexpr utl::tuple<int, short, char, long long, short> w(73, 2, 22, 2652547, 7231);
static constexpr utl::tuple<int, float> t1(2, 0.354);
static constexpr utl::tuple<long long, double> t2(2, 0.354);

static_assert(
    utl::is_same<decltype(tuple_cat(t1, t2)), utl::tuple<int, float, long long, double>>::value,
    "tuple_cat concatenates all tuple types in order and forward constructs all elements");

static constexpr utl::tuple<int, float, long long, double> t3(tuple_cat(t1, t2));

static_assert(utl::get<char>(v) == 22, "Should be gettable by type if target exists");
static_assert(utl::get<char>(w) == 22, "Should be gettable by type if target type is unique");
static_assert(not_gettable_by_type<short, decltype(w)>::value,
    "Should not be gettable by type if target is not unique");

template <int>
struct f {};
static_assert(utl::is_same<decltype(utl::forward_as_tuple(f<0>{}, f<1>{}, f<2>{})),
                  utl::tuple<f<0>&&, f<1>&&, f<2>&&>>::value,
    "utl::forward_as_tuple must be lvalue if lvalue, rvalue otherwise");

static_assert(utl::is_same<decltype(utl::forward_as_tuple(utl::declval<f<0>&>(), f<1>{}, f<2>{})),
                  utl::tuple<f<0>&, f<1>&&, f<2>&&>>::value,
    "utl::forward_as_tuple must be lvalue if lvalue, rvalue otherwise");

static_assert(utl::is_same<decltype(utl::forward_as_tuple(utl::declval<f<0> const&>(),
                               utl::declval<f<0>&>(), utl::declval<f<0> const volatile&>())),
                  utl::tuple<f<0> const&, f<0>&, f<0> const volatile&>>::value,
    "utl::forward_as_tuple must conserve cv qualifiers");

static_assert(utl::is_same<decltype(utl::forward_as_tuple(utl::declval<f<0> const>(),
                               utl::declval<f<0>>(), utl::declval<f<0> const volatile>())),
                  utl::tuple<f<0> const&&, f<0>&&, f<0> const volatile&&>>::value,
    "utl::forward_as_tuple must conserve cv qualifiers");

struct explicit_obj {
    explicit explicit_obj() noexcept = default;
};
struct implicit_obj {
    implicit_obj() noexcept = default;
};

static_assert(utl::is_implicit_constructible<utl::tuple<implicit_obj, int, float>>::value,
    "tuples containing non explicit constructible elements should not be explicitly constructible");
static_assert(!utl::is_explicit_constructible<utl::tuple<implicit_obj, int, float>>::value,
    "tuples containing non explicit constructible elements should not be explicitly constructible");

static_assert(!utl::is_implicit_constructible<utl::tuple<explicit_obj, int, float>>::value,
    "tuples containing non implicit constructible elements should not be implicitly constructible");
static_assert(utl::is_explicit_constructible<utl::tuple<explicit_obj, int, float>>::value,
    "tuples containing non implicit constructible elements should not be implicitly constructible");

template <int>
struct move_only {
public:
    constexpr move_only() noexcept = default;
    constexpr move_only(move_only&&) noexcept = default;
    UTL_CONSTEXPR_CXX14 move_only& operator=(move_only&&) noexcept = default;
    constexpr move_only const& operator=(move_only&&) const noexcept { return *this; }

private:
    constexpr move_only(move_only const&) noexcept = default;
    UTL_CONSTEXPR_CXX14 move_only& operator=(move_only const&) noexcept = default;
};

static_assert(
    !utl::is_copy_constructible<move_only<0>>::value, "move_only should be copy constructible");
static_assert(!utl::is_copy_assignable<move_only<0>>::value, "move_only should be copy assignable");
static_assert(
    utl::is_move_constructible<move_only<0>>::value, "move_only should be move constructible");
static_assert(utl::is_move_assignable<move_only<0>>::value, "move_only should be move assignable");

struct alloc {
    using is_always_equal = utl::true_type;
};
template <int>
struct copy_only {
public:
    constexpr copy_only() noexcept = default;
    constexpr copy_only(copy_only const&) noexcept = default;
    UTL_CONSTEXPR_CXX14 copy_only& operator=(copy_only const&) noexcept = default;
    explicit constexpr copy_only(copy_only const&, alloc const&);

    constexpr copy_only(copy_only&&) noexcept = delete;
    UTL_CONSTEXPR_CXX14 copy_only& operator=(copy_only&&) noexcept = delete;
    explicit constexpr copy_only(copy_only&&, alloc const&) = delete;
};

static_assert(
    utl::is_copy_constructible<copy_only<0>>::value, "copy_only should be copy constructible");
static_assert(utl::is_copy_assignable<copy_only<0>>::value, "copy_only should be copy assignable");
static_assert(
    !utl::is_move_constructible<copy_only<0>>::value, "copy_only should be move constructible");
static_assert(!utl::is_move_assignable<copy_only<0>>::value, "copy_only should be move assignable");

static_assert(!utl::is_copy_constructible<utl::tuple<move_only<0>, copy_only<0>>>::value,
    "if non-copy_only element exists, tuple should not be copy_only");
static_assert(!utl::is_copy_assignable<utl::tuple<move_only<0>, copy_only<0>>>::value,
    "if non-copy_only element exists, tuple should not be copy_only");
static_assert(!utl::is_copy_constructible<utl::tuple<move_only<0>, int>>::value,
    "if non-copy_only element exists, tuple should not be copy_only");
static_assert(!utl::is_copy_assignable<utl::tuple<move_only<0>, int>>::value,
    "if non-copy_only element exists, tuple should not be copy_only");
static_assert(utl::is_copy_constructible<utl::tuple<copy_only<0>, copy_only<1>>>::value,
    "if all elements are copy_only, tuple should be copy_only");
static_assert(utl::is_copy_assignable<utl::tuple<copy_only<0>, copy_only<1>>>::value,
    "if all elements are copy_only, tuple should be copy_only");

static_assert(utl::is_move_constructible<utl::tuple<int, int>>::value,
    "fundamental tuple is move constructible");

static_assert(!utl::is_constructible<utl::tuple<move_only<0>, move_only<1>>,
                  utl::tuple<move_only<0>&&, move_only<1>&&> const&>::value,
    "Move only tuple should not be constructible with an lvalue tuple of rvalue references of the "
    "same element types");
static_assert(utl::is_constructible<utl::tuple<move_only<0>, move_only<1>>,
                  utl::tuple<move_only<0>&&, move_only<1>&&>>::value,
    "Move only tuple should be constructible with an rvalue tuple of rvalue references of the same "
    "element types");

static_assert(utl::is_constructible<utl::tuple<copy_only<0>, copy_only<1>>,
                  utl::tuple<copy_only<0>&&, copy_only<1>&&> const&>::value,
    "Copy only tuple should be constructible with an lvalue tuple of rvalue references of the same "
    "element types");

#ifdef UTL_ENFORCE_NONMOVABILIITY
static_assert(!utl::is_assignable<utl::tuple<copy_only<0>, copy_only<1>>&,
                  utl::tuple<copy_only<0>&&, copy_only<1>&&>>::value,
    "Copy only tuple should not be assignable with an rvalue tuple of rvalue references of the "
    "same element types");
static_assert(!utl::is_constructible<utl::tuple<copy_only<0>, copy_only<1>>, utl::allocator_arg_t,
                  alloc, utl::tuple<copy_only<0>&&, copy_only<1>&&>>::value,
    "Copy only tuple should not be constructible with an rvalue tuple of rvalue references of the "
    "same element types");
static_assert(!utl::is_move_constructible<utl::tuple<copy_only<0>, copy_only<1>>>::value,
    "if non-move_only element exists, tuple should not be move_only");
static_assert(!utl::is_constructible<utl::tuple<copy_only<0>, copy_only<1>>,
                  utl::tuple<copy_only<0>&&, copy_only<1>&&>>::value,
    "Copy only tuple should not be constructible with an rvalue tuple of rvalue references of the "
    "same element types");
static_assert(!utl::is_move_constructible<utl::tuple<move_only<0>, copy_only<0>>>::value,
    "if non-move_only element exists, tuple should not be move_only");
static_assert(!utl::is_move_assignable<utl::tuple<move_only<0>, copy_only<0>>>::value,
    "if non-move_only element exists, tuple should not be move_only");
static_assert(!utl::is_move_assignable<utl::tuple<copy_only<0>, copy_only<1>>>::value,
    "if non-move_only element exists, tuple should not be move_only");
#else
static_assert(utl::is_constructible<utl::tuple<copy_only<0>, copy_only<1>>, utl::allocator_arg_t,
                  alloc, utl::tuple<copy_only<0>&&, copy_only<1>&&>>::value,
    "Copy only tuple should be constructible with an rvalue tuple of rvalue references of the same "
    "element types");

static_assert(utl::is_move_constructible<utl::tuple<copy_only<0>, copy_only<1>>>::value,
    "if non-move_only element exists, tuple should still be movable");
static_assert(utl::is_constructible<utl::tuple<copy_only<0>, copy_only<1>>,
                  utl::tuple<copy_only<0>&&, copy_only<1>&&>>::value,
    "Copy only tuple should be constructible with an rvalue tuple of rvalue references of the same "
    "element types");
static_assert(utl::is_move_constructible<utl::tuple<move_only<0>, copy_only<0>>>::value,
    "if non-move_only element exists, tuple should still be movable");
static_assert(utl::is_move_assignable<utl::tuple<move_only<0>, copy_only<0>>>::value,
    "if non-move_only element exists, tuple should still be movable");
static_assert(utl::is_move_assignable<utl::tuple<copy_only<0>, copy_only<1>>>::value,
    "if non-move_only element exists, tuple should still be movable");
#endif

static_assert(utl::is_move_constructible<utl::tuple<move_only<0>, move_only<1>>>::value,
    "if all elements are move_only, tuple should be move_only");
static_assert(utl::is_move_assignable<utl::tuple<move_only<0>, move_only<1>>>::value,
    "if all elements are move_only, tuple should be move_only");

template <int J>
struct defaultable {
    constexpr defaultable() noexcept = default;
    template <int i UTL_REQUIRES_CXX11(i != J)>
    constexpr defaultable(defaultable<i>) noexcept {}
};

static_assert(utl::is_default_constructible<utl::tuple<defaultable<0>, int, float>>::value,
    "If all element are default constructible, tuple is default constructible");

static_assert(utl::is_constructible<utl::tuple<int, defaultable<0>, float>, int, defaultable<0>,
                  float>::value,
    "If all element are default constructible, tuple is default constructible");

static_assert(utl::is_nothrow_default_constructible<
                  utl::tuple<defaultable<0>, defaultable<1>, defaultable<3>>>::value,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(
    utl::is_nothrow_constructible<utl::tuple<long long, double>, utl::tuple<int, float>>::value,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(utl::is_constructible<utl::tuple<long long, double>, utl::tuple<int, float>&>::value,
    "If all element are constructible, tuple is constructible from reference");
static_assert(
    utl::is_constructible<utl::tuple<long long, double>, utl::tuple<int, float> const&>::value,
    "If all element are constructible, tuple is constructible from const reference");

static_assert(utl::is_nothrow_constructible<utl::tuple<long long, double>, int, float>::value,
    "If all element are nothrow constructible, tuple is nothrow constructible");

#if defined(UTL_CXX20) || !defined(UTL_TARGET_APPLE)
static_assert(
    utl::is_nothrow_constructible<utl::tuple<defaultable<0>, defaultable<1>, defaultable<3>>,
        defaultable<2>, defaultable<3>, defaultable<1>>::value,
    "If all element are nothrow constructible, tuple is nothrow constructible");
#endif

static_assert(utl::is_constructible<defaultable<0>, defaultable<2>>::value,
    "If all element are nothrow constructible, tuple is nothrow constructible");

// constexpr utl::tuple<defaultable<0>, defaultable<1>, defaultable<3>> asdas(defaultable<0>{},
// defaultable<1>{}, defaultable<3>{}); constexpr utl::tuple<defaultable<0>, defaultable<1>,
// defaultable<3>> jkadsmjsamd(defaultable<2>{}, defaultable<3>{}, defaultable<1>{});

static_assert(utl::is_nothrow_copy_constructible<
                  utl::tuple<defaultable<0>, defaultable<1>, defaultable<3>>>::value,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(utl::is_nothrow_move_constructible<
                  utl::tuple<defaultable<0>, defaultable<1>, defaultable<3>>>::value,
    "If all element are nothrow constructible, tuple is nothrow constructible");

static_assert(utl::is_nothrow_copy_assignable<
                  utl::tuple<defaultable<0>, defaultable<1>, defaultable<3>>>::value,
    "If all element are nothrow assignable, tuple is nothrow assignable");

static_assert(utl::is_nothrow_move_assignable<
                  utl::tuple<defaultable<0>, defaultable<1>, defaultable<3>>>::value,
    "If all element are nothrow assignable, tuple is nothrow assignable");

template <int>
struct non_defaultable {
    non_defaultable() = delete;
    non_defaultable(int) {}
};

static_assert(!utl::is_default_constructible<utl::tuple<non_defaultable<0>, int, float>>::value,
    "If non-default constructible element exists, tuple is not default constructible");

template <int>
struct trivial_struct {
    int x, y;
};
static_assert(utl::is_trivial<trivial_struct<0>>::value, "Should be trivial");

using trivial_tuple = utl::tuple<long long, double, trivial_struct<0>, trivial_struct<1>>;
#ifdef UTL_CXX20

static_assert(utl::is_trivially_copy_constructible<trivial_tuple>::value,
    "If all element are trivial, tuple is trivial in >C++20");
static_assert(utl::is_trivially_default_constructible<trivial_tuple>::value,
    "If all element are trivial, tuple is trivial in >C++20");
static_assert(utl::is_trivially_move_constructible<trivial_tuple>::value,
    "If all element are trivial, tuple is trivial in >C++20");
static_assert(utl::is_trivially_copy_assignable<trivial_tuple>::value,
    "If all element are trivial, tuple is trivial in >C++20");
static_assert(utl::is_trivially_move_assignable<trivial_tuple>::value,
    "If all element are trivial, tuple is trivial in >C++20");
static_assert(utl::is_trivially_destructible<trivial_tuple>::value,
    "If all element are trivial, tuple is trivial in >C++20");
static_assert(utl::is_trivially_copyable<trivial_tuple>::value,
    "If all element are trivial, tuple is trivial in >C++20");
static_assert(utl::is_trivial<trivial_tuple>::value,
    "If all element are trivial, tuple is trivial in >C++20");
#endif

static_assert(utl::is_standard_layout<trivial_tuple>::value,
    "If all element are standard layout, tuple is standard layout");
static_assert(utl::tuple_element_offset<2, trivial_tuple>::value == 16, "");
static_assert(utl::tuple_element_offset<2, utl::tuple<long long, short, char>>::value == 10, "");

static_assert(utl::reference_constructs_from_temporary_v<long long&&, int>, "");
static_assert(
    !utl::is_constructible<utl::tuple<long long&&, double&&>, utl::tuple<int, float>>::value,
    "cannot dangle");

} // namespace tuple_test

#include <tuple>

namespace tuple_test {
static_assert(utl::is_constructible<utl::tuple<long long, double>, std::tuple<int, float>>::value,
    "If all element are constructible, tuple is constructible from const reference");
static_assert(utl::is_constructible<utl::tuple<long long, double>, std::pair<int, float>>::value,
    "If all element are constructible, tuple is constructible from const reference");
static_assert(utl::is_assignable<utl::tuple<long long, double>, std::tuple<int, float>>::value,
    "If all element are constructible, tuple is constructible from const reference");
static_assert(utl::is_assignable<utl::tuple<long long, double>, std::pair<int, float>>::value,
    "If all element are constructible, tuple is constructible from const reference");

static_assert(
    !utl::is_constructible<utl::tuple<long long&&, double&&>, std::tuple<int, float>>::value,
    "If all element are constructible, tuple is constructible from const reference");
static_assert(!utl::is_constructible<utl::tuple<long long&&, double>, std::pair<int, float>>::value,
    "If all element are constructible, tuple is constructible from const reference");
} // namespace tuple_test
UTL_DISABLE_WARNING_POP()
