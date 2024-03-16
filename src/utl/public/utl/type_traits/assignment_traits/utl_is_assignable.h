// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_assignable;

#  ifdef UTL_CXX17

using std::is_assignable_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_assignable_v = is_assignable<T, U>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_assignable 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_assignable
#    define UTL_DISABLE_BUILTIN_is_assignable 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_assignable

#  if UTL_SHOULD_USE_BUILTIN(is_assignable)
#    define UTL_BUILTIN_is_assignable(...) __is_assignable(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_assignable)

#  ifdef UTL_BUILTIN_is_assignable

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct is_assignable : bool_constant<UTL_BUILTIN_is_assignable(T, U)> {};

#    ifdef UTL_CXX14
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
auto impl(int) noexcept -> decltype((declval<T>() = declval<U>()), true_type);

template <typename T, typename U>
auto impl(float) noexcept -> false_type;

template <typename T, typename U>
using impl_t = decltype(impl<T, U>(0));
} // namespace assignable
} // namespace details

template <typename T, typename U>
struct is_assignable : details::assignable::impl_t<T, U> {};

#    ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_assignable_v = is_assignable<T, U>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_assignable 1

#  endif // ifdef UTL_BUILTIN_is_assignable

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
