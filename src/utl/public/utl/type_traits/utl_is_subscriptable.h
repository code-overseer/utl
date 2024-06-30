// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20
#  include "utl/concepts/utl_subscriptable.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename Arg>
struct is_subscriptable : UTL_SCOPE bool_constant<UTL_SCOPE subscriptable_with<T, Arg>> {};

#  if UTL_CXX14
template <typename T, typename Arg>
UTL_INLINE_CXX17 constexpr bool is_subscriptable_v = UTL_SCOPE subscriptable_with<T, Arg>;
#  endif // UTL_CXX14

UTL_NAMESPACE_END

#else
#  include "utl/type_traits/utl_declval.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace subscriptable {

template <typename T, typename Arg>
auto trait(int) noexcept
    -> decltype(UTL_SCOPE declval<T>()[UTL_SCOPE declval<Arg>()], UTL_SCOPE true_type{});
template <typename T, typename Arg>
auto trait(float) noexcept -> UTL_SCOPE false_type;

template <typename T, typename Arg>
using trait_t = decltype(UTL_SCOPE details::subscriptable::trait<T, Arg>(0));
} // namespace subscriptable
} // namespace details

template <typename T, typename Arg>
struct is_subscriptable : UTL_SCOPE details::subscriptable::trait_t<T, Arg> {};

#  if UTL_CXX14
template <typename T, typename Arg>
UTL_INLINE_CXX17 constexpr bool is_subscriptable_v = is_subscriptable<T, Arg>::value;
#  endif // UTL_CXX14

UTL_NAMESPACE_END

#endif

#if UTL_CXX20
#  define UTL_TRAIT_is_subscriptable(...) UTL_SCOPE subscriptable<__VA_ARGS__>
#elif UTL_CXX14
#  define UTL_TRAIT_is_subscriptable(...) UTL_SCOPE is_subscriptable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_subscriptable(...) UTL_SCOPE is_subscriptable<__VA_ARGS__>::value
#endif
