// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_constructible;

#  if UTL_CXX17

using std::is_constructible_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_constructible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_constructible)
#    define UTL_BUILTIN_is_constructible(...) __is_constructible(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_constructible)

#  ifdef UTL_BUILTIN_is_constructible

UTL_NAMESPACE_BEGIN

template <typename T, typename... Args>
struct __UTL_PUBLIC_TEMPLATE is_constructible :
    bool_constant<UTL_BUILTIN_is_constructible(T, Args...)> {};

#    if UTL_CXX14
template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_constructible_v = UTL_BUILTIN_is_constructible(T, Args...);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_constructible 1

#  else // ifdef UTL_BUILTIN_is_constructible

#    include "utl/type_traits/utl_declval.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace constructible {
template <typename T, typename... Args>
__UTL_HIDE_FROM_ABI auto impl(int) noexcept -> decltype((T(declval<Args>()...), true_type));

template <typename T, typename... Args>
__UTL_HIDE_FROM_ABI auto impl(float) noexcept -> false_type;

template <typename T, typename... Args>
using impl_t UTL_NODEBUG = decltype(impl<T, Args...>(0));
} // namespace constructible
} // namespace details

template <typename T, typename... Args>
struct __UTL_PUBLIC_TEMPLATE is_constructible : details::constructible::impl_t<T, Args...> {};

#    if UTL_CXX14
template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_constructible 1

#  endif // ifdef UTL_BUILTIN_is_constructible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_constructible
#  define UTL_TRAIT_is_constructible(...) UTL_BUILTIN_is_constructible(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_constructible(...) __UTL is_constructible_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_constructible(...) __UTL is_constructible<__VA_ARGS__>::value
#endif
