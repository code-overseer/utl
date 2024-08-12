// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::make_unsigned;

#  if UTL_CXX14

using std::make_unsigned_t;

#  else  // if UTL_CXX14
template <typename T>
using make_unsigned_t = typename make_unsigned<T>::type;
#  endif // if UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_make_unsigned 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(make_unsigned)
#    define UTL_BUILTIN_make_unsigned(...) __make_unsigned(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(make_unsigned)

#  ifdef UTL_BUILTIN_make_unsigned

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE make_unsigned {
    using type UTL_NODEBUG = UTL_BUILTIN_make_unsigned(T);
};

template <typename T>
using make_unsigned_t = UTL_BUILTIN_make_unsigned(T);

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_make_unsigned 1

#  else // ifdef UTL_BUILTIN_make_unsigned

#    include "utl/type_traits/utl_is_enum.h"
#    include "utl/type_traits/utl_is_integral.h"
#    include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace type_traits {

template <size_t I>
struct unsigned_table_entry;
template <size_t I>
using unsigned_table_entry_t = typename unsigned_table_entry<I>::type;
#    define UTL_GENERATE_TABLE_ENTRY(N, T) \
        template <>                        \
        struct unsigned_table_entry<N> {   \
            using type UTL_NODEBUG = T;    \
        }

UTL_GENERATE_TABLE_ENTRY(0, unsigned char);
UTL_GENERATE_TABLE_ENTRY(1, unsigned short);
UTL_GENERATE_TABLE_ENTRY(2, unsigned int);
UTL_GENERATE_TABLE_ENTRY(3, unsigned long);
UTL_GENERATE_TABLE_ENTRY(4, unsigned long long);
#    ifdef UTL_SUPPORTS_INT128
UTL_GENERATE_TABLE_ENTRY(5, __uint128_t);
#    endif

#    undef UTL_GENERATE_TABLE_ENTRY

template <typename T, size_t Idx = 0, bool = (is_integral<T>::value || is_enum<T>::value)>
struct find_unsigned;
template <typename T, size_t Idx = 0>
using find_unsigned_t = typename find_unsigned<T, Idx>::type;

template <typename T, size_t Idx>
struct find_unsigned<T, Idx, true> :
    conditional_t<sizeof(T) == sizeof(unsigned_table_entry_t<Idx>), unsigned_table_entry<Idx>,
        find_unsigned<T, Idx + 1>> {};
} // namespace type_traits
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE make_unsigned {
    using type UTL_NODEBUG = details::type_traits::find_unsigned_t<T>;
};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<T const> {
    using type UTL_NODEBUG = typename make_unsigned<T>::type const;
};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<T volatile> {
    using type UTL_NODEBUG = typename make_unsigned<T>::type volatile;
};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<T const volatile> {
    using type UTL_NODEBUG = typename make_unsigned<T>::type const volatile;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<bool> {};
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<unsigned char> {
    using type UTL_NODEBUG = unsigned char;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<signed char> {
    using type UTL_NODEBUG = unsigned char;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<unsigned short> {
    using type UTL_NODEBUG = unsigned short;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<signed short> {
    using type UTL_NODEBUG = unsigned short;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<unsigned int> {
    using type UTL_NODEBUG = unsigned int;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<signed int> {
    using type UTL_NODEBUG = unsigned int;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<unsigned long> {
    using type UTL_NODEBUG = unsigned long;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<signed long> {
    using type UTL_NODEBUG = unsigned long;
};

#    ifdef UTL_SUPPORTS_INT128
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<__uint128_t> {
    using type UTL_NODEBUG = __uint128_t;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_unsigned<__int128_t> {
    using type UTL_NODEBUG = __uint128_t;
};
#    endif

template <typename T>
using make_unsigned_t = typename make_unsigned<T>::type;

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_make_unsigned \
        UTL_TRAIT_SUPPORTED_is_integral&& UTL_TRAIT_SUPPORTED_is_enum

#  endif // ifdef UTL_BUILTIN_make_unsigned

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
