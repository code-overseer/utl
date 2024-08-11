// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_nothrow_convertible;

#  if UTL_CXX17

using std::is_nothrow_convertible_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_nothrow_convertible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_nothrow_convertible)
#    define UTL_BUILTIN_is_nothrow_convertible(...) __is_nothrow_convertible(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_nothrow_convertible)

#  ifdef UTL_BUILTIN_is_nothrow_convertible

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct UTL_PUBLIC_TEMPLATE is_nothrow_convertible :
    bool_constant<UTL_BUILTIN_is_nothrow_convertible(From, To)> {};

#    if UTL_CXX14
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_nothrow_convertible_v = UTL_BUILTIN_is_nothrow_convertible(From, To);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_convertible 1

#  else // if UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#    include "utl/type_traits/utl_declval.h"
#    include "utl/type_traits/utl_is_void.h"
#    include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace convertible {
template <typename T>
UTL_HIDE_FROM_ABI void implicit_conv(T) noexcept;
template <typename From, typename To>
UTL_HIDE_FROM_ABI auto nothrow_test(int) noexcept
    -> bool_constant<noexcept(implicit_conv<To>(declval<From>()))>;
template <typename, typename>
UTL_HIDE_FROM_ABI auto nothrow_test(float) noexcept -> false_type;

template <typename From, typename To>
using is_nothrow UTL_NODEBUG = decltype(nothrow_test<From, To>(0));

} /* namespace convertible */
} /* namespace details */

template <typename From, typename To>
struct UTL_PUBLIC_TEMPLATE is_nothrow_convertible :
    disjunction<bool_constant<UTL_TRAIT_is_void(From) && UTL_TRAIT_is_void(To)>,
        details::convertible::is_nothrow<From, To>> {};

#    if UTL_CXX14
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_convertible 1

#  endif // ifdef UTL_BUILTIN_is_nothrow_convertible

#endif // if UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#ifdef UTL_BUILTIN_is_nothrow_convertible
#  define UTL_TRAIT_is_nothrow_convertible(...) UTL_BUILTIN_is_nothrow_convertible(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_nothrow_convertible(...) UTL_SCOPE is_nothrow_convertible_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_nothrow_convertible(...) UTL_SCOPE is_nothrow_convertible<__VA_ARGS__>::value
#endif
