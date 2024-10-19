// Copyright 2023-2024 Bryan Wong

#include "utl/atomic/utl_atomic.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_is_enum.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_void.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_template_list.h"
#include "utl/type_traits/utl_underlying_type.h"

namespace atomic_tests {
using load_operations = utl::type_list<utl::atomic_operations<utl::memory_order::relaxed>,
    utl::atomic_operations<utl::memory_order::acquire>,
    utl::atomic_operations<utl::memory_order::consume>,
    utl::atomic_operations<utl::memory_order::seq_cst>>;
using store_operations = utl::type_list<utl::atomic_operations<utl::memory_order::relaxed>,
    utl::atomic_operations<utl::memory_order::release>,
    utl::atomic_operations<utl::memory_order::seq_cst>>;

using atomic_operations = utl::type_list<utl::atomic_operations<utl::memory_order::relaxed>,
    utl::atomic_operations<utl::memory_order::acquire>,
    utl::atomic_operations<utl::memory_order::consume>,
    utl::atomic_operations<utl::memory_order::release>,
    utl::atomic_operations<utl::memory_order::acq_rel>,
    utl::atomic_operations<utl::memory_order::seq_cst>>;

template <typename Native>
struct Enum {
    enum class Type : Native {
    };
};

struct alignas(4) Struct {
    char data[4];
};

Enum<int>::Type func(Enum<int>::Type* ptr) {
    return utl::atomic_acquire::load(ptr);
}

Struct func(Struct* ptr) {
    return utl::atomic_acquire::load(ptr);
}

using native_types = utl::type_list<bool, char, signed char, unsigned char, short, unsigned short,
    int, unsigned int, long, unsigned long, long long, unsigned long long, void*>;

using enum_types = utl::type_list<Enum<bool>::Type, Enum<char>::Type, Enum<signed char>::Type,
    Enum<unsigned char>::Type, Enum<short>::Type, Enum<unsigned short>::Type, Enum<int>::Type,
    Enum<unsigned int>::Type, Enum<long>::Type, Enum<unsigned long>::Type, Enum<long long>::Type,
    Enum<unsigned long long>::Type>;

template <typename TYPE, typename OP>
struct LoadAssertions {
    static_assert(utl::is_same<TYPE, decltype(OP::load((TYPE*)(0)))>::value, "");
    static_assert(utl::is_same<TYPE, decltype(OP::load((TYPE volatile*)(0)))>::value, "");
    static_assert(utl::is_same<TYPE, decltype(OP::load((TYPE const*)(0)))>::value, "");
    static_assert(utl::is_same<TYPE, decltype(OP::load((TYPE const volatile*)(0)))>::value, "");
};

template <typename TYPE, typename OP>
struct StoreAssertions {
    static_assert(utl::is_void<decltype(OP::store((TYPE*)(0), utl::declval<TYPE>()))>::value, "");
    static_assert(
        utl::is_void<decltype(OP::store((TYPE volatile*)(0), utl::declval<TYPE>()))>::value, "");
};

template <typename TYPE, typename OP>
struct ExchangeAssertions {
    static_assert(
        utl::is_same<TYPE, decltype(OP::exchange((TYPE*)(0), utl::declval<TYPE>()))>::value, "");
    static_assert(utl::is_same<TYPE,
                      decltype(OP::exchange((TYPE volatile*)(0), utl::declval<TYPE>()))>::value,
        "");
};

template <typename... Ts>
struct Complete {
    static_assert(utl::conjunction<utl::is_complete<Ts>...>::value, "");
};

template <template <typename, typename> class Assertion, typename T, typename L>
struct EachType;

template <template <typename, typename> class Assertion, typename T, typename... Ops>
struct EachType<Assertion, T, utl::type_list<Ops...>> {
    static_assert(utl::conjunction<utl::is_complete<Assertion<T, Ops>>...>::value, "");
};

template <template <typename, typename> class Assertion, typename... Types, typename... Operations>
Complete<EachType<Assertion, Types, utl::type_list<Operations...>>...> instantiate(
    utl::type_list<Types...>, utl::type_list<Operations...>) noexcept;

static_assert(utl::is_complete<decltype(instantiate<LoadAssertions>(
        native_types{}, load_operations{}))>::value);
static_assert(utl::is_complete<decltype(instantiate<StoreAssertions>(
        native_types{}, store_operations{}))>::value);
static_assert(utl::is_complete<decltype(instantiate<ExchangeAssertions>(
        native_types{}, atomic_operations{}))>::value);
static_assert(utl::is_complete<decltype(instantiate<LoadAssertions>(
        enum_types{}, load_operations{}))>::value);
static_assert(utl::is_complete<decltype(instantiate<StoreAssertions>(
        enum_types{}, store_operations{}))>::value);
static_assert(utl::is_complete<decltype(instantiate<ExchangeAssertions>(
        enum_types{}, atomic_operations{}))>::value);

template <typename From, typename = void>
struct BitwiseArg {
    using type = From;
};
template <typename From>
struct BitwiseArg<From, utl::enable_if_t<utl::is_pointer<From>::value>> {
    using type = uintptr_t;
};
template <typename From>
struct BitwiseArg<From, utl::enable_if_t<utl::is_enum<From>::value>> {
    using type = utl::underlying_type_t<From>;
};
template <typename T>
using BitwiseArg_t = typename BitwiseArg<T>::type;
template <typename From, typename = void>
struct ArithmeticArg {
    using type = From;
};
template <typename From>
struct ArithmeticArg<From, utl::enable_if_t<utl::is_pointer<From>::value>> {
    using type = ptrdiff_t;
};
template <typename From>
struct ArithmeticArg<From, utl::enable_if_t<utl::is_enum<From>::value>> {
    using type = utl::underlying_type_t<From>;
};
template <typename T>
using ArithmeticArg_t = typename ArithmeticArg<T>::type;

template <typename TYPE, typename OP>
struct FetchBitwiseAssertions {
    static_assert(
        utl::is_same<TYPE,
            decltype(OP::fetch_and((TYPE*)(0), utl::declval<BitwiseArg_t<TYPE>>()))>::value,
        "");
    static_assert(utl::is_same<TYPE,
                      decltype(OP::fetch_and(
                          (TYPE volatile*)(0), utl::declval<BitwiseArg_t<TYPE>>()))>::value,
        "");
    static_assert(
        utl::is_same<TYPE,
            decltype(OP::fetch_or((TYPE*)(0), utl::declval<BitwiseArg_t<TYPE>>()))>::value,
        "");
    static_assert(
        utl::is_same<TYPE,
            decltype(OP::fetch_or((TYPE volatile*)(0), utl::declval<BitwiseArg_t<TYPE>>()))>::value,
        "");
    static_assert(
        utl::is_same<TYPE,
            decltype(OP::fetch_xor((TYPE*)(0), utl::declval<BitwiseArg_t<TYPE>>()))>::value,
        "");
    static_assert(utl::is_same<TYPE,
                      decltype(OP::fetch_xor(
                          (TYPE volatile*)(0), utl::declval<BitwiseArg_t<TYPE>>()))>::value,
        "");
};

template <typename TYPE, typename OP>
struct FetchArithmeticAssertions {
    static_assert(
        utl::is_same<TYPE,
            decltype(OP::fetch_add((TYPE*)(0), utl::declval<ArithmeticArg_t<TYPE>>()))>::value,
        "");
    static_assert(utl::is_same<TYPE,
                      decltype(OP::fetch_add(
                          (TYPE volatile*)(0), utl::declval<ArithmeticArg_t<TYPE>>()))>::value,
        "");
    static_assert(
        utl::is_same<TYPE,
            decltype(OP::fetch_sub((TYPE*)(0), utl::declval<ArithmeticArg_t<TYPE>>()))>::value,
        "");
    static_assert(utl::is_same<TYPE,
                      decltype(OP::fetch_sub(
                          (TYPE volatile*)(0), utl::declval<ArithmeticArg_t<TYPE>>()))>::value,
        "");
};

static_assert(utl::is_complete<decltype(instantiate<FetchBitwiseAssertions>(
        native_types{}, atomic_operations{}))>::value);
static_assert(utl::is_complete<decltype(instantiate<FetchArithmeticAssertions>(
        native_types{}, atomic_operations{}))>::value);
static_assert(utl::is_complete<decltype(instantiate<FetchBitwiseAssertions>(
        enum_types{}, atomic_operations{}))>::value);
static_assert(utl::is_complete<decltype(instantiate<FetchArithmeticAssertions>(
        enum_types{}, atomic_operations{}))>::value);

template <typename Failure>
struct CompareExchangeAssertions {
    template <typename TYPE, typename OP>
    struct Assert {
        static_assert(utl::is_boolean<decltype(OP::compare_exchange_strong(
                          (TYPE*)(0), (TYPE*)(0), utl::declval<TYPE>(), Failure{}))>::value,
            "");
        static_assert(utl::is_boolean<decltype(OP::compare_exchange_strong((TYPE volatile*)(0),
                          (TYPE*)(0), utl::declval<TYPE>(), Failure{}))>::value,
            "");
    };
};

static_assert(utl::is_complete<
    decltype(instantiate<CompareExchangeAssertions<decltype(utl::memory_order_relaxed)>::Assert>(
        native_types{}, atomic_operations{}))>::value);

static_assert(utl::is_complete<decltype(instantiate<
        CompareExchangeAssertions<decltype(utl::memory_order_acquire)>::Assert>(native_types{},
        utl::type_list<utl::atomic_acquire, utl::atomic_release, utl::atomic_acq_rel,
            utl::atomic_seq_cst>{}))>::value);

static_assert(utl::is_complete<decltype(instantiate<
        CompareExchangeAssertions<decltype(utl::memory_order_consume)>::Assert>(native_types{},
        utl::type_list<utl::atomic_acquire, utl::atomic_consume, utl::atomic_release,
            utl::atomic_acq_rel, utl::atomic_seq_cst>{}))>::value);

static_assert(utl::is_complete<
    decltype(instantiate<CompareExchangeAssertions<decltype(utl::memory_order_seq_cst)>::Assert>(
        native_types{}, utl::type_list<utl::atomic_seq_cst>{}))>::value);

static_assert(utl::is_complete<
    decltype(instantiate<CompareExchangeAssertions<decltype(utl::memory_order_relaxed)>::Assert>(
        enum_types{}, atomic_operations{}))>::value);

static_assert(utl::is_complete<decltype(instantiate<
        CompareExchangeAssertions<decltype(utl::memory_order_acquire)>::Assert>(enum_types{},
        utl::type_list<utl::atomic_acquire, utl::atomic_release, utl::atomic_acq_rel,
            utl::atomic_seq_cst>{}))>::value);

static_assert(utl::is_complete<decltype(instantiate<
        CompareExchangeAssertions<decltype(utl::memory_order_consume)>::Assert>(enum_types{},
        utl::type_list<utl::atomic_acquire, utl::atomic_consume, utl::atomic_release,
            utl::atomic_acq_rel, utl::atomic_seq_cst>{}))>::value);

static_assert(utl::is_complete<
    decltype(instantiate<CompareExchangeAssertions<decltype(utl::memory_order_seq_cst)>::Assert>(
        enum_types{}, utl::type_list<utl::atomic_seq_cst>{}))>::value);

} // namespace atomic_tests
