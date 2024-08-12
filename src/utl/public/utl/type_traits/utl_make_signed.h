// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::make_signed;

#  if UTL_CXX14

using std::make_signed_t;

#  else  // if UTL_CXX14
template <typename T>
using make_signed_t = typename make_signed<T>::type;
#  endif // if UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_make_signed 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  if UTL_SHOULD_USE_BUILTIN(make_signed)
#    define UTL_BUILTIN_make_signed(...) __make_signed(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(make_signed)

#  ifdef UTL_BUILTIN_make_signed

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE make_signed {
    using type UTL_NODEBUG = UTL_BUILTIN_make_signed(T);
};

template <typename T>
using make_signed_t = UTL_BUILTIN_make_signed(T);

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_make_signed 1

#  else // ifdef UTL_BUILTIN_make_signed

#    include "utl/type_traits/utl_is_enum.h"
#    include "utl/type_traits/utl_is_integral.h"
#    include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace type_traits {

template <size_t I>
struct signed_table_entry;
template <size_t I>
using signed_table_entry_t = typename signed_table_entry<I>::type;
#    define UTL_GENERATE_TABLE_ENTRY(N, T) \
        template <>                        \
        struct signed_table_entry<N> {     \
            using type UTL_NODEBUG = T;    \
        }

UTL_GENERATE_TABLE_ENTRY(0, signed char);
UTL_GENERATE_TABLE_ENTRY(1, signed short);
UTL_GENERATE_TABLE_ENTRY(2, signed int);
UTL_GENERATE_TABLE_ENTRY(3, signed long);
UTL_GENERATE_TABLE_ENTRY(4, signed long long);
#    ifdef UTL_SUPPORTS_INT128
UTL_GENERATE_TABLE_ENTRY(5, __int128_t);
#    endif

#    undef UTL_GENERATE_TABLE_ENTRY

template <typename T, size_t Idx = 0, bool = (is_integral<T>::value || is_enum<T>::value)>
struct find_signed;
template <typename T, size_t Idx = 0>
using find_signed_t UTL_NODEBUG = typename find_signed<T, Idx>::type;

template <typename T, size_t Idx>
struct find_signed<T, Idx, true> :
    conditional_t<sizeof(T) == sizeof(signed_table_entry_t<Idx>), signed_table_entry<Idx>,
        find_signed<T, Idx + 1>> {};
} // namespace type_traits
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE make_signed {
    using type UTL_NODEBUG = details::type_traits::find_signed_t<T>;
};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE make_signed<T const> {
    using type UTL_NODEBUG = typename make_signed<T>::type const;
};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE make_signed<T volatile> {
    using type UTL_NODEBUG = typename make_signed<T>::type volatile;
};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE make_signed<T const volatile> {
    using type UTL_NODEBUG = typename make_signed<T>::type const volatile;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<bool> {};
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<unsigned char> {
    using type UTL_NODEBUG = signed char;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<signed char> {
    using type UTL_NODEBUG = signed char;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<unsigned short> {
    using type UTL_NODEBUG = signed short;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<signed short> {
    using type UTL_NODEBUG = signed short;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<unsigned int> {
    using type UTL_NODEBUG = signed int;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<signed int> {
    using type UTL_NODEBUG = signed int;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<unsigned long> {
    using type UTL_NODEBUG = signed long;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<signed long> {
    using type UTL_NODEBUG = signed long;
};

#    ifdef UTL_SUPPORTS_INT128
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<__uint128_t> {
    using type UTL_NODEBUG = __int128_t;
};
template <>
struct __UTL_PUBLIC_TEMPLATE make_signed<__int128_t> {
    using type UTL_NODEBUG = __int128_t;
};
#    endif

template <typename T>
using make_signed_t = typename make_signed<T>::type;

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_make_signed \
        UTL_TRAIT_SUPPORTED_is_integral&& UTL_TRAIT_SUPPORTED_is_enum

#  endif // ifdef UTL_BUILTIN_make_signed

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
