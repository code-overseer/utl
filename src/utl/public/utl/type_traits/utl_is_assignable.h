// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_assignable;

#  if UTL_CXX17

using std::is_assignable_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_assignable_v = is_assignable<T, U>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_assignable 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_assignable)
#    define UTL_BUILTIN_is_assignable(...) __is_assignable(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_assignable)

#  ifdef UTL_BUILTIN_is_assignable

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct UTL_PUBLIC_TEMPLATE is_assignable : bool_constant<UTL_BUILTIN_is_assignable(T, U)> {};

#    if UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_assignable_v = UTL_BUILTIN_is_assignable(T, U);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_assignable 1

#  else // ifdef UTL_BUILTIN_is_assignable

#    include "utl/type_traits/utl_declval.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace assignable {
template <typename T, typename U>
UTL_HIDE_FROM_ABI auto impl(int) noexcept -> decltype((declval<T>() = declval<U>()), true_type);

template <typename T, typename U>
UTL_HIDE_FROM_ABI auto impl(float) noexcept -> false_type;

template <typename T, typename U>
using impl_t UTL_NODEBUG = decltype(impl<T, U>(0));
} // namespace assignable
} // namespace details

template <typename T, typename U>
struct UTL_PUBLIC_TEMPLATE is_assignable : details::assignable::impl_t<T, U> {};

#    if UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_assignable_v = is_assignable<T, U>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_assignable 1

#  endif // ifdef UTL_BUILTIN_is_assignable

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_assignable
#  define UTL_TRAIT_is_assignable(...) UTL_BUILTIN_is_assignable(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_assignable(...) UTL_SCOPE is_assignable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_assignable(...) UTL_SCOPE is_assignable<__VA_ARGS__>::value
#endif
