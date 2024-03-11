// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::make_signed;
using std::make_unsigned;

#  ifdef UTL_CXX14

using std::make_signed_t;
using std::make_unsigned_t;

#  else  // ifdef UTL_CXX14
template <typename T>
using make_signed_t = typename make_signed<T>::type;
template <typename T>
using make_unsigned_t = typename make_unsigned<T>::type;
#  endif // ifdef UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_make_signed 1
#  define UTL_TRAIT_SUPPORTED_make_unsigned 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_make_signed
#    define UTL_DISABLE_BUILTIN_make_signed 0
#  endif // ifndef UTL_DISABLE_BUILTIN_make_signed

#  ifndef UTL_DISABLE_BUILTIN_make_unsigned
#    define UTL_DISABLE_BUILTIN_make_unsigned 0
#  endif // ifndef UTL_DISABLE_BUILTIN_make_unsigned

#  if UTL_SHOULD_USE_BUILTIN(make_signed)
#    define UTL_BUILTIN_make_signed(...) __make_signed(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(make_signed)

#  if UTL_SHOULD_USE_BUILTIN(make_unsigned)
#    define UTL_BUILTIN_make_unsigned(...) __make_unsigned(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(make_unsigned)

#  if !defined(UTL_BUILTIN_make_signed) || !defined(UTL_BUILTIN_make_unsigned)

#    include "utl/type_traits/utl_is_enum.h"
#    include "utl/type_traits/utl_is_integral.h"
#    include "utl/type_traits/utl_logical_traits.h"

#  endif

#  ifdef UTL_BUILTIN_make_signed

UTL_NAMESPACE_BEGIN

template <typename T>
struct make_signed {
    using type = UTL_BUILTIN_make_signed(T);
};

template <typename T>
using make_signed_t = UTL_BUILTIN_make_signed(T);

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_make_signed 1

#  else // ifdef UTL_BUILTIN_make_signed

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
            using type = T;                \
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
using find_signed_t = typename find_signed<T, Idx>::type;

template <typename T, size_t Idx>
struct find_signed<T, Idx, true> :
    conditional_t<sizeof(T) == sizeof(signed_table_entry_t<Idx>), signed_table_entry<Idx>,
        find_signed<T, Idx + 1>> {};
} // namespace type_traits
} // namespace details

template <typename T>
struct make_signed {
    using type = details::type_traits::find_signed_t<T>;
};
template <typename T>
struct make_signed<T const> {
    using type = typename make_signed<T>::type const;
};
template <typename T>
struct make_signed<T volatile> {
    using type = typename make_signed<T>::type volatile;
};
template <typename T>
struct make_signed<T const volatile> {
    using type = typename make_signed<T>::type const volatile;
};
template <>
struct make_signed<bool> {};
template <>
struct make_signed<unsigned char> {
    using type = signed char;
};
template <>
struct make_signed<signed char> {
    using type = signed char;
};
template <>
struct make_signed<unsigned short> {
    using type = signed short;
};
template <>
struct make_signed<signed short> {
    using type = signed short;
};
template <>
struct make_signed<unsigned int> {
    using type = signed int;
};
template <>
struct make_signed<signed int> {
    using type = signed int;
};
template <>
struct make_signed<unsigned long> {
    using type = signed long;
};
template <>
struct make_signed<signed long> {
    using type = signed long;
};

#    ifdef UTL_SUPPORTS_INT128
template <>
struct make_signed<__uint128_t> {
    using type = __int128_t;
};
template <>
struct make_signed<__int128_t> {
    using type = __int128_t;
};
#    endif

template <typename T>
using make_signed_t = typename make_signed<T>::type;

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_make_signed \
        UTL_TRAIT_SUPPORTED_is_integral&& UTL_TRAIT_SUPPORTED_is_enum

#  endif // ifdef UTL_BUILTIN_make_signed

#  ifdef UTL_BUILTIN_make_unsigned

UTL_NAMESPACE_BEGIN

template <typename T>
struct make_unsigned {
    using type = UTL_BUILTIN_make_unsigned(T);
};

template <typename T>
using make_unsigned_t = UTL_BUILTIN_make_unsigned(T);

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_make_unsigned 1

#  else // ifdef UTL_BUILTIN_make_unsigned

namespace details {
namespace type_traits {

template <size_t I>
struct unsigned_table_entry;
template <size_t I>
using unsigned_table_entry_t = typename unsigned_table_entry<I>::type;
#    define UTL_GENERATE_TABLE_ENTRY(N, T) \
        template <>                        \
        struct unsigned_table_entry<N> {   \
            using type = T;                \
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

UTL_NAMESPACE_BEGIN

template <typename T>
struct make_unsigned {
    using type = details::type_traits::find_unsigned_t<T>;
};
template <typename T>
struct make_unsigned<T const> {
    using type = typename make_unsigned<T>::type const;
};
template <typename T>
struct make_unsigned<T volatile> {
    using type = typename make_unsigned<T>::type volatile;
};
template <typename T>
struct make_unsigned<T const volatile> {
    using type = typename make_unsigned<T>::type const volatile;
};
template <>
struct make_unsigned<bool> {};
template <>
struct make_unsigned<unsigned char> {
    using type = unsigned char;
};
template <>
struct make_unsigned<signed char> {
    using type = unsigned char;
};
template <>
struct make_unsigned<unsigned short> {
    using type = unsigned short;
};
template <>
struct make_unsigned<signed short> {
    using type = unsigned short;
};
template <>
struct make_unsigned<unsigned int> {
    using type = unsigned int;
};
template <>
struct make_unsigned<signed int> {
    using type = unsigned int;
};
template <>
struct make_unsigned<unsigned long> {
    using type = unsigned long;
};
template <>
struct make_unsigned<signed long> {
    using type = unsigned long;
};

#    ifdef UTL_SUPPORTS_INT128
template <>
struct make_unsigned<__uint128_t> {
    using type = __uint128_t;
};
template <>
struct make_unsigned<__int128_t> {
    using type = __uint128_t;
};
#    endif

template <typename T>
using make_unsigned_t = typename make_unsigned<T>::type;

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_make_unsigned \
        UTL_TRAIT_SUPPORTED_is_integral&& UTL_TRAIT_SUPPORTED_is_enum

#  endif // ifdef UTL_BUILTIN_make_unsigned

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
