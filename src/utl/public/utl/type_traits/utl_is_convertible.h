// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_convertible;

#  ifdef UTL_CXX17

using std::is_convertible_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_convertible_v = is_convertible<T, Args...>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_convertible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_convertible
#    define UTL_DISABLE_BUILTIN_is_convertible 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_convertible

#  if UTL_SHOULD_USE_BUILTIN(is_convertible)
#    define UTL_BUILTIN_is_convertible(...) __is_convertible(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_convertible)

#  ifdef UTL_BUILTIN_is_convertible

UTL_NAMESPACE_BEGIN

template <typename T, typename... Args>
struct is_convertible : bool_constant<UTL_BUILTIN_is_convertible(T, Args...)> {};

#    ifdef UTL_CXX14
template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_convertible_v = UTL_BUILTIN_is_convertible(T, Args...);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_convertible 1

#  else // ifdef UTL_BUILTIN_is_convertible

#    include "utl/type_traits/utl_declval.h"
#    include "utl/type_traits/utl_is_void.h"
#    include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace convertible {
template <typename T>
void implicit_conv(T) noexcept;

template <typename From, typename To, typename = decltype(implicit_conv<To>(declval<From>()))>
auto conv_test(int) noexcept -> true_type;
template <typename, typename>
auto conv_test(float) noexcept -> false_type;

template <typename From, typename To>
using impl_t = decltype(conv_test<From, To>(0));
} // namespace convertible
} // namespace details

template <typename From, typename To>
struct is_convertible :
    disjunction<conjunction<is_void<From>, is_void<To>>, details::convertible::impl_t<From, To>> {};

#    ifdef UTL_CXX14
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_convertible_v = is_convertible<From, To>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_convertible 1

#  endif // ifdef UTL_BUILTIN_is_convertible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
