// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_nothrow_assignable;

#  ifdef UTL_CXX17

using std::is_nothrow_assignable_v;

#  elif defined(UTL_CXX14) // ifdef UTL_CXX17

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;

#  endif // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_nothrow_assignable 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_nothrow_assignable
#    define UTL_DISABLE_BUILTIN_is_nothrow_assignable 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_nothrow_assignable

#  if UTL_SHOULD_USE_BUILTIN(is_nothrow_assignable)
#    define UTL_BUILTIN_is_nothrow_assignable(...) __is_nothrow_assignable(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_nothrow_assignable)

#  ifdef UTL_BUILTIN_is_nothrow_assignable

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct is_nothrow_assignable : bool_constant<UTL_BUILTIN_is_nothrow_assignable(T, U)> {};

#    ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_nothrow_assignable_v = UTL_BUILTIN_is_nothrow_assignable(T, U);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_nothrow_assignable 1

#  else // ifdef UTL_BUILTIN_is_nothrow_assignable

#    include "utl/type_traits/utl_declval.h"
#    include "utl/type_traits/utl_is_assignable.h"
#    include "utl/type_traits/utl_is_reference.h"
UTL_NAMESPACE_BEGIN

namespace details {
namespace assignable {
template <bool assignable, bool reference>
struct nothrow_branch_t {};

template <typename T>
void implicit_conv(T) noexcept;

template <typename T, typename U>
auto nothrow_impl(nothrow_branch_t<true, false>) noexcept
    -> bool_constant<noexcept(T(declval<Args>()...))>;

template <typename T, typename Arg>
auto nothrow_impl(nothrow_branch_t<true, true>) noexcept
    -> bool_constant<noexcept(implicit_conv<T>(declval<Arg>()))>;

template <typename T, typename U>
auto nothrow_impl(nothrow_branch_t<false, false>) noexcept -> false_type;

template <typename T, typename U>
auto nothrow_impl(nothrow_branch_t<false, true>) noexcept -> false_type;

template <typename T, typename U>
using nothrow_impl_t = decltype(nothrow_impl<T, U>(
    nothrow_branch_t<UTL_TRAIT_VALUE(is_assignable, T, Args), UTL_TRAIT_VALUE(is_reference, T)>{}));

} // namespace assignable
} // namespace details

template <typename T, typename U>
struct is_nothrow_assignable : details::assignable::nothrow_impl_t<T, U> {};

#    ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;
#    endif // UTL_CXX14

#    define UTL_TRAIT_SUPPORTED_is_nothrow_assignable 1

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_nothrow_assignable

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
