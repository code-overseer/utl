// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_nothrow_constructible;

#  if UTL_CXX17

using std::is_nothrow_constructible_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_nothrow_constructible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_nothrow_constructible)
#    define UTL_BUILTIN_is_nothrow_constructible(...) __is_nothrow_constructible(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_nothrow_constructible)

#  ifdef UTL_BUILTIN_is_nothrow_constructible

UTL_NAMESPACE_BEGIN

template <typename T, typename... Args>
struct is_nothrow_constructible :
    bool_constant<UTL_BUILTIN_is_nothrow_constructible(T, Args...)> {};

#    if UTL_CXX14
template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_nothrow_constructible_v = UTL_BUILTIN_is_nothrow_constructible(T, Args...);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_constructible 1

#  else // ifdef UTL_BUILTIN_is_nothrow_constructible

#    include "utl/type_traits/utl_declval.h"
#    include "utl/type_traits/utl_is_constructible.h"
#    include "utl/type_traits/utl_is_reference.h"
UTL_NAMESPACE_BEGIN

namespace details {
namespace constructible {
template <bool constructible, bool reference>
struct nothrow_branch_t {};

template <typename T>
void implicit_conv(T) noexcept;

template <typename T, typename... Args>
auto nothrow_impl(nothrow_branch_t<true, false>) noexcept
    -> bool_constant<noexcept(T(declval<Args>()...))>;

template <typename T, typename Arg>
auto nothrow_impl(nothrow_branch_t<true, true>) noexcept
    -> bool_constant<noexcept(implicit_conv<T>(declval<Arg>()))>;

template <typename T, typename... Args>
auto nothrow_impl(nothrow_branch_t<false, false>) noexcept -> false_type;

template <typename T, typename... Args>
auto nothrow_impl(nothrow_branch_t<false, true>) noexcept -> false_type;

template <typename T, typename... Args>
using nothrow_impl_t = decltype(nothrow_impl<T, Args...>(
    nothrow_branch_t<__UTL is_constructible<T, Args...>::value, __UTL is_reference<T>::value>{}));

} // namespace constructible
} // namespace details

template <typename T, typename... Args>
struct is_nothrow_constructible : details::constructible::nothrow_impl_t<T, Args...> {};

#    if UTL_CXX14
template <typename T, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;
#    endif // UTL_CXX14

#    define UTL_TRAIT_SUPPORTED_is_nothrow_constructible 1

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_nothrow_constructible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_nothrow_constructible
#  define UTL_TRAIT_is_nothrow_constructible(...) UTL_BUILTIN_is_nothrow_constructible(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_nothrow_constructible(...) __UTL is_nothrow_constructible_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_nothrow_constructible(...) __UTL is_nothrow_constructible<__VA_ARGS__>::value
#endif
