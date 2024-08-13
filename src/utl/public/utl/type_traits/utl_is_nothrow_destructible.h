// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_nothrow_destructible;

#  if UTL_CXX17

using std::is_nothrow_destructible_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_nothrow_destructible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_nothrow_destructible)
#    define UTL_BUILTIN_is_nothrow_destructible(...) __is_nothrow_destructible(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_nothrow_constructible)

#  ifdef UTL_BUILTIN_is_nothrow_destructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_destructible :
    bool_constant<UTL_BUILTIN_is_nothrow_destructible(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_destructible_v = UTL_BUILTIN_is_nothrow_destructible(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_destructible 1

#  else // ifdef UTL_BUILTIN_is_nothrow_destructible

#    include "utl/type_traits/utl_is_destructible.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace destructible {

template <typename T>
__UTL_HIDE_FROM_ABI auto nothrow_impl(true_type) noexcept
    -> bool_constant<noexcept(declval<T&>().~T())>;

template <typename T>
__UTL_HIDE_FROM_ABI auto nothrow_impl(false_type) noexcept -> false_type;

template <typename T>
using nothrow_impl_t UTL_NODEBUG = decltype(nothrow_impl<T>(is_destructible<T>{}));

} // namespace destructible
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_destructible : details::destructible::nothrow_impl_t<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_destructible_v = details::destructible::nothrow_impl_t<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_destructible UTL_TRAIT_SUPPORTED_is_destructible

#  endif // ifdef UTL_BUILTIN_is_nothrow_destructible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#if UTL_CXX14
#  define UTL_TRAIT_is_nothrow_destructible(...) __UTL is_nothrow_destructible_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_nothrow_destructible(...) __UTL is_nothrow_destructible<__VA_ARGS__>::value
#endif
