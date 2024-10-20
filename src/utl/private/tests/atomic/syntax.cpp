// Copyright 2023-2024 Bryan Wong

#include "utl/atomic/utl_atomic.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_is_enum.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_void.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_template_list.h"
#include "utl/type_traits/utl_type_identity.h"
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

template <>
struct Enum<void*> {
    enum class Type {
    };
};

template <typename Native>
struct alignas(Native) Struct {
    unsigned char data[sizeof(Native)];
};

template <typename Native>
using Enum_t = Enum<Native>::Type;

using native_types = utl::type_list<bool, char, signed char, unsigned char, short, unsigned short,
    int, unsigned int, long, unsigned long, long long, unsigned long long, void*>;

using enum_types = utl::type_list<Enum<bool>::Type, Enum<char>::Type, Enum<signed char>::Type,
    Enum<unsigned char>::Type, Enum<short>::Type, Enum<unsigned short>::Type, Enum<int>::Type,
    Enum<unsigned int>::Type, Enum<long>::Type, Enum<unsigned long>::Type, Enum<long long>::Type,
    Enum<unsigned long long>::Type>;

using class_types = utl::type_list<Struct<bool>, Struct<char>, Struct<signed char>,
    Struct<unsigned char>, Struct<short>, Struct<unsigned short>, Struct<int>, Struct<unsigned int>,
    Struct<long>, Struct<unsigned long>, Struct<long long>, Struct<unsigned long long>>;

template <typename TYPE, typename OP>
struct LoadAssertions {
    static TYPE load(TYPE* ptr) noexcept { return OP::load(ptr); }
    static TYPE load(TYPE volatile* ptr) noexcept { return OP::load(ptr); }
    static TYPE load(TYPE const* ptr) noexcept { return OP::load(ptr); }
    static TYPE load(TYPE const volatile* ptr) noexcept { return OP::load(ptr); }
};

#define INSTANTIATE(ASSERTION, MODIFIER, OP)                     \
    template struct ASSERTION<MODIFIER<bool>, OP>;               \
    template struct ASSERTION<MODIFIER<char>, OP>;               \
    template struct ASSERTION<MODIFIER<signed char>, OP>;        \
    template struct ASSERTION<MODIFIER<unsigned char>, OP>;      \
    template struct ASSERTION<MODIFIER<short>, OP>;              \
    template struct ASSERTION<MODIFIER<unsigned short>, OP>;     \
    template struct ASSERTION<MODIFIER<int>, OP>;                \
    template struct ASSERTION<MODIFIER<unsigned int>, OP>;       \
    template struct ASSERTION<MODIFIER<long>, OP>;               \
    template struct ASSERTION<MODIFIER<unsigned long>, OP>;      \
    template struct ASSERTION<MODIFIER<long long>, OP>;          \
    template struct ASSERTION<MODIFIER<unsigned long long>, OP>; \
    template struct ASSERTION<MODIFIER<void*>, OP>

INSTANTIATE(LoadAssertions, utl::type_identity_t, utl::atomic_relaxed);
INSTANTIATE(LoadAssertions, Enum_t, utl::atomic_relaxed);
INSTANTIATE(LoadAssertions, Struct, utl::atomic_relaxed);
INSTANTIATE(LoadAssertions, utl::type_identity_t, utl::atomic_consume);
INSTANTIATE(LoadAssertions, Enum_t, utl::atomic_consume);
INSTANTIATE(LoadAssertions, Struct, utl::atomic_consume);
INSTANTIATE(LoadAssertions, utl::type_identity_t, utl::atomic_acquire);
INSTANTIATE(LoadAssertions, Enum_t, utl::atomic_acquire);
INSTANTIATE(LoadAssertions, Struct, utl::atomic_acquire);
INSTANTIATE(LoadAssertions, utl::type_identity_t, utl::atomic_seq_cst);
INSTANTIATE(LoadAssertions, Enum_t, utl::atomic_seq_cst);
INSTANTIATE(LoadAssertions, Struct, utl::atomic_seq_cst);

template <typename TYPE, typename OP>
struct StoreAssertions {
    static void store(TYPE* ptr, TYPE value) noexcept { OP::store(ptr, value); }
    static void store(TYPE volatile* ptr, TYPE value) noexcept { OP::store(ptr, value); }
};

INSTANTIATE(StoreAssertions, utl::type_identity_t, utl::atomic_relaxed);
INSTANTIATE(StoreAssertions, Enum_t, utl::atomic_relaxed);
INSTANTIATE(StoreAssertions, Struct, utl::atomic_relaxed);
INSTANTIATE(StoreAssertions, utl::type_identity_t, utl::atomic_release);
INSTANTIATE(StoreAssertions, Enum_t, utl::atomic_release);
INSTANTIATE(StoreAssertions, Struct, utl::atomic_release);
INSTANTIATE(StoreAssertions, utl::type_identity_t, utl::atomic_seq_cst);
INSTANTIATE(StoreAssertions, Enum_t, utl::atomic_seq_cst);
INSTANTIATE(StoreAssertions, Struct, utl::atomic_seq_cst);

template <typename TYPE, typename OP>
struct ExchangeAssertions {
    static TYPE exchange(TYPE* ptr, TYPE value) noexcept { return OP::exchange(ptr, value); }
    static TYPE exchange(TYPE volatile* ptr, TYPE value) noexcept {
        return OP::exchange(ptr, value);
    }
};

INSTANTIATE(ExchangeAssertions, utl::type_identity_t, utl::atomic_relaxed);
INSTANTIATE(ExchangeAssertions, Enum_t, utl::atomic_relaxed);
INSTANTIATE(ExchangeAssertions, Struct, utl::atomic_relaxed);
INSTANTIATE(ExchangeAssertions, utl::type_identity_t, utl::atomic_consume);
INSTANTIATE(ExchangeAssertions, Enum_t, utl::atomic_consume);
INSTANTIATE(ExchangeAssertions, Struct, utl::atomic_consume);
INSTANTIATE(ExchangeAssertions, utl::type_identity_t, utl::atomic_acquire);
INSTANTIATE(ExchangeAssertions, Enum_t, utl::atomic_acquire);
INSTANTIATE(ExchangeAssertions, Struct, utl::atomic_acquire);
INSTANTIATE(ExchangeAssertions, utl::type_identity_t, utl::atomic_release);
INSTANTIATE(ExchangeAssertions, Enum_t, utl::atomic_release);
INSTANTIATE(ExchangeAssertions, Struct, utl::atomic_release);
INSTANTIATE(ExchangeAssertions, utl::type_identity_t, utl::atomic_acq_rel);
INSTANTIATE(ExchangeAssertions, Enum_t, utl::atomic_acq_rel);
INSTANTIATE(ExchangeAssertions, Struct, utl::atomic_acq_rel);
INSTANTIATE(ExchangeAssertions, utl::type_identity_t, utl::atomic_seq_cst);
INSTANTIATE(ExchangeAssertions, Enum_t, utl::atomic_seq_cst);
INSTANTIATE(ExchangeAssertions, Struct, utl::atomic_seq_cst);

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
    static TYPE fetch_and(TYPE* ptr, BitwiseArg_t<TYPE> arg) noexcept {
        return OP::fetch_and(ptr, arg);
    }
    static TYPE fetch_and(TYPE volatile* ptr, BitwiseArg_t<TYPE> arg) noexcept {
        return OP::fetch_and(ptr, arg);
    }

    static TYPE fetch_or(TYPE* ptr, BitwiseArg_t<TYPE> arg) noexcept {
        return OP::fetch_or(ptr, arg);
    }
    static TYPE fetch_or(TYPE volatile* ptr, BitwiseArg_t<TYPE> arg) noexcept {
        return OP::fetch_or(ptr, arg);
    }
    static TYPE fetch_xor(TYPE* ptr, BitwiseArg_t<TYPE> arg) noexcept {
        return OP::fetch_xor(ptr, arg);
    }
    static TYPE fetch_xor(TYPE volatile* ptr, BitwiseArg_t<TYPE> arg) noexcept {
        return OP::fetch_xor(ptr, arg);
    }
};

template <typename OP>
struct FetchBitwiseAssertions<void*, OP> {};

template <typename TYPE, typename OP>
struct FetchArithmeticAssertions {
    static TYPE fetch_add(TYPE* ptr, ArithmeticArg_t<TYPE> arg) noexcept {
        return OP::fetch_add(ptr, arg);
    }
    static TYPE fetch_add(TYPE volatile* ptr, ArithmeticArg_t<TYPE> arg) noexcept {
        return OP::fetch_add(ptr, arg);
    }

    static TYPE fetch_sub(TYPE* ptr, ArithmeticArg_t<TYPE> arg) noexcept {
        return OP::fetch_sub(ptr, arg);
    }
    static TYPE fetch_sub(TYPE volatile* ptr, ArithmeticArg_t<TYPE> arg) noexcept {
        return OP::fetch_sub(ptr, arg);
    }
};

INSTANTIATE(FetchBitwiseAssertions, utl::type_identity_t, utl::atomic_relaxed);
INSTANTIATE(FetchBitwiseAssertions, utl::type_identity_t, utl::atomic_consume);
INSTANTIATE(FetchBitwiseAssertions, utl::type_identity_t, utl::atomic_acquire);
INSTANTIATE(FetchBitwiseAssertions, utl::type_identity_t, utl::atomic_release);
INSTANTIATE(FetchBitwiseAssertions, utl::type_identity_t, utl::atomic_acq_rel);
INSTANTIATE(FetchBitwiseAssertions, utl::type_identity_t, utl::atomic_seq_cst);

INSTANTIATE(FetchArithmeticAssertions, utl::type_identity_t, utl::atomic_relaxed);
INSTANTIATE(FetchArithmeticAssertions, utl::type_identity_t, utl::atomic_consume);
INSTANTIATE(FetchArithmeticAssertions, utl::type_identity_t, utl::atomic_acquire);
INSTANTIATE(FetchArithmeticAssertions, utl::type_identity_t, utl::atomic_release);
INSTANTIATE(FetchArithmeticAssertions, utl::type_identity_t, utl::atomic_acq_rel);
INSTANTIATE(FetchArithmeticAssertions, utl::type_identity_t, utl::atomic_seq_cst);

template <typename TYPE, typename OP, typename Failure>
struct CompareExchangeAssertions {
    static bool cas_stron(TYPE* ptr, TYPE* exp, TYPE val, Failure f) noexcept {
        return OP::compare_exchange_strong(ptr, exp, val, f);
    }
    static bool cas_weak(TYPE* ptr, TYPE* exp, TYPE val, Failure f) noexcept {
        return OP::compare_exchange_weak(ptr, exp, val, f);
    }
    static bool cas_stron(TYPE volatile* ptr, TYPE* exp, TYPE val, Failure f) noexcept {
        return OP::compare_exchange_strong(ptr, exp, val, f);
    }
    static bool cas_weak(TYPE volatile* ptr, TYPE* exp, TYPE val, Failure f) noexcept {
        return OP::compare_exchange_weak(ptr, exp, val, f);
    }
};

#undef INSTANTIATE
#define INSTANTIATE(ASSERTION, MODIFIER, OP, FAIL)                     \
    template struct ASSERTION<MODIFIER<bool>, OP, FAIL>;               \
    template struct ASSERTION<MODIFIER<char>, OP, FAIL>;               \
    template struct ASSERTION<MODIFIER<signed char>, OP, FAIL>;        \
    template struct ASSERTION<MODIFIER<unsigned char>, OP, FAIL>;      \
    template struct ASSERTION<MODIFIER<short>, OP, FAIL>;              \
    template struct ASSERTION<MODIFIER<unsigned short>, OP, FAIL>;     \
    template struct ASSERTION<MODIFIER<int>, OP, FAIL>;                \
    template struct ASSERTION<MODIFIER<unsigned int>, OP, FAIL>;       \
    template struct ASSERTION<MODIFIER<long>, OP, FAIL>;               \
    template struct ASSERTION<MODIFIER<unsigned long>, OP, FAIL>;      \
    template struct ASSERTION<MODIFIER<long long>, OP, FAIL>;          \
    template struct ASSERTION<MODIFIER<unsigned long long>, OP, FAIL>; \
    template struct ASSERTION<MODIFIER<void*>, OP, FAIL>

#define INSTANTIATE_CAS_ASSERT(MODIFIER)                                  \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_relaxed, \
        decltype(utl::memory_order_relaxed));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_consume, \
        decltype(utl::memory_order_relaxed));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_consume, \
        decltype(utl::memory_order_consume));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_acquire, \
        decltype(utl::memory_order_relaxed));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_acquire, \
        decltype(utl::memory_order_consume));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_acquire, \
        decltype(utl::memory_order_acquire));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_release, \
        decltype(utl::memory_order_relaxed));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_release, \
        decltype(utl::memory_order_consume));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_release, \
        decltype(utl::memory_order_acquire));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_acq_rel, \
        decltype(utl::memory_order_relaxed));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_acq_rel, \
        decltype(utl::memory_order_consume));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_acq_rel, \
        decltype(utl::memory_order_acquire));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_seq_cst, \
        decltype(utl::memory_order_relaxed));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_seq_cst, \
        decltype(utl::memory_order_consume));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_seq_cst, \
        decltype(utl::memory_order_acquire));                             \
    INSTANTIATE(CompareExchangeAssertions, MODIFIER, utl::atomic_seq_cst, \
        decltype(utl::memory_order_seq_cst))

INSTANTIATE_CAS_ASSERT(utl::type_identity_t);
INSTANTIATE_CAS_ASSERT(Enum_t);
INSTANTIATE_CAS_ASSERT(Struct);

} // namespace atomic_tests
