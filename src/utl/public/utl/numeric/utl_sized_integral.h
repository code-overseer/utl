// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20
#  include "utl/concepts/utl_integral.h"
#  include "utl/concepts/utl_signed_integral.h"
#  include "utl/concepts/utl_unsigned_integral.h"

UTL_NAMESPACE_BEGIN

template <typename T, size_t N>
concept sized_integral = integral<T> && sizeof(T) == N;
template <typename T, size_t N>
concept sized_unsigned_integral = unsigned_integral<T> && sized_integral<T, N>;
template <typename T, size_t N>
concept sized_signed_integral = signed_integral<T> && sized_integral<T, N>;

template <size_t N, typename T>
struct __UTL_PUBLIC_TEMPLATE is_sized_integral : bool_constant<sized_integral<T, N>> {};
template <size_t N, typename T>
struct __UTL_PUBLIC_TEMPLATE is_sized_unsigned_integral :
    bool_constant<sized_unsigned_integral<T, N>> {};
template <size_t N, typename T>
struct __UTL_PUBLIC_TEMPLATE is_sized_signed_integral :
    bool_constant<sized_signed_integral<T, N>> {};
template <size_t N, typename T>
inline constexpr bool is_sized_integral_v = sized_integral<T, N>;
template <size_t N, typename T>
inline constexpr bool is_sized_unsigned_integral_v = sized_unsigned_integral<T, N>;
template <size_t N, typename T>
inline constexpr bool is_sized_signed_integral_v = sized_signed_integral<T, N>;

#  define UTL_TRAIT_is_sized_integral(...) sized_integral<__VA_ARGS__>
#  define UTL_TRAIT_is_sized_unsigned_integral(...) sized_unsigned_integral<__VA_ARGS__>
#  define UTL_TRAIT_is_sized_signed_integral(...) sized_signed_integral<__VA_ARGS__>

UTL_NAMESPACE_END
#else

#  include "utl/type_traits/utl_is_integral.h"
#  include "utl/type_traits/utl_is_signed.h"
#  include "utl/type_traits/utl_is_unsigned.h"
UTL_NAMESPACE_BEGIN

template <size_t N, typename T>
struct __UTL_PUBLIC_TEMPLATE is_sized_integral :
    bool_constant<UTL_TRAIT_is_integral(T) && sizeof(T) == N> {};
template <size_t N, typename T>
struct __UTL_PUBLIC_TEMPLATE is_sized_unsigned_integral :
    bool_constant<UTL_TRAIT_is_integral(T) && UTL_TRAIT_is_unsigned(T) && sizeof(T) == N> {};
template <size_t N, typename T>
struct __UTL_PUBLIC_TEMPLATE is_sized_signed_integral :
    bool_constant<UTL_TRAIT_is_integral(T) && UTL_TRAIT_is_signed(T) && sizeof(T) == N> {};

#  if UTL_CXX14
template <size_t N, typename T>
UTL_INLINE_CXX17 constexpr bool is_sized_integral_v = UTL_TRAIT_is_integral(T) && sizeof(T) == N;
template <size_t N, typename T>
UTL_INLINE_CXX17 constexpr bool is_sized_unsigned_integral_v =
    UTL_TRAIT_is_integral(T) && UTL_TRAIT_is_unsigned(T) && sizeof(T) == N;
template <size_t N, typename T>
UTL_INLINE_CXX17 constexpr bool is_sized_signed_integral_v =
    UTL_TRAIT_is_integral(T) && UTL_TRAIT_is_signed(T) && sizeof(T) == N;
#  endif

UTL_NAMESPACE_END

#  if UTL_CXX14
#    define UTL_TRAIT_is_sized_integral(...) is_sized_integral_v<__VA_ARGS__>
#    define UTL_TRAIT_is_sized_unsigned_integral(...) is_sized_unsigned_integral_v<__VA_ARGS__>
#    define UTL_TRAIT_is_sized_signed_integral(...) is_sized_signed_integral_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_sized_integral(...) is_sized_integral<__VA_ARGS__>::value
#    define UTL_TRAIT_is_sized_unsigned_integral(...) is_sized_unsigned_integral<__VA_ARGS__>::value
#    define UTL_TRAIT_is_sized_signed_integral(...) is_sized_signed_integral<__VA_ARGS__>::value
#  endif
#endif
