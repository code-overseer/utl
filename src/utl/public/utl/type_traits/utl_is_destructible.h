// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_destructible;

#  if UTL_CXX17
using std::is_destructible_v;
#  elif UTL_CXX14 // UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_destructible_v = is_destructible<T>::value;
#  endif          // UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_destructible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_destructible)
#    define UTL_BUILTIN_is_destructible(...) __is_destructible(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_destructible)

#  ifdef UTL_BUILTIN_is_destructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_destructible : bool_constant<UTL_BUILTIN_is_destructible(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_destructible_v = UTL_BUILTIN_is_destructible(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_destructible 1

#  else // ifdef UTL_BUILTIN_is_destructible

UTL_NAMESPACE_BEGIN
namespace details {
namespace destructible {
template <typename>
__UTL_HIDE_FROM_ABI auto callable_impl(float) noexcept -> __UTL false_type;
template <typename T>
__UTL_HIDE_FROM_ABI auto callable_impl(int) noexcept
    -> __UTL always_true_type<decltype(declval<T&>().~T())>;
template <typename T>
using callable = decltype(callable_impl<T>(0));
} // namespace destructible
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_destructible :
    bool_constant<UTL_TRAIT_is_reference(T) ||
        (UTL_TRAIT_is_object(T) &&
            details::destructible::callable<remove_all_extents_t<T>>::value)> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_destructible<T const> : is_destructible<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_destructible<T volatile> : is_destructible<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_destructible<T const volatile> : is_destructible<T> {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_destructible<void> : false_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_destructible<T[]> : false_type {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_destructible_v = is_destructible<T>::value;
#    endif // UTL_CXX14

#    define UTL_TRAIT_SUPPORTED_is_destructible 1

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_destructible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_destructible
#  define UTL_TRAIT_is_destructible(...) UTL_BUILTIN_is_destructible(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_destructible(...) __UTL is_destructible_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_destructible(...) __UTL is_destructible<__VA_ARGS__>::value
#endif
