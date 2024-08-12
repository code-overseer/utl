// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20
#  include "utl/concepts/utl_subscriptable.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename Arg>
struct __UTL_PUBLIC_TEMPLATE is_subscriptable :
    __UTL bool_constant<__UTL subscriptable_with<T, Arg>> {};

#  if UTL_CXX14
template <typename T, typename Arg>
UTL_INLINE_CXX17 constexpr bool is_subscriptable_v = __UTL subscriptable_with<T, Arg>;
#  endif // UTL_CXX14

UTL_NAMESPACE_END

#else
#  include "utl/type_traits/utl_declval.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace subscriptable {

template <typename T, typename Arg>
__UTL_HIDE_FROM_ABI auto trait(int) noexcept
    -> decltype(__UTL declval<T>()[__UTL declval<Arg>()], __UTL true_type{});
template <typename T, typename Arg>
__UTL_HIDE_FROM_ABI auto trait(float) noexcept -> __UTL false_type;

template <typename T, typename Arg>
using trait_t UTL_NODEBUG = decltype(__UTL details::subscriptable::trait<T, Arg>(0));
} // namespace subscriptable
} // namespace details

template <typename T, typename Arg>
struct __UTL_PUBLIC_TEMPLATE is_subscriptable : __UTL details::subscriptable::trait_t<T, Arg> {};

#  if UTL_CXX14
template <typename T, typename Arg>
UTL_INLINE_CXX17 constexpr bool is_subscriptable_v = is_subscriptable<T, Arg>::value;
#  endif // UTL_CXX14

UTL_NAMESPACE_END

#endif

#if UTL_CXX20
#  define UTL_TRAIT_is_subscriptable(...) __UTL subscriptable<__VA_ARGS__>
#elif UTL_CXX14
#  define UTL_TRAIT_is_subscriptable(...) __UTL is_subscriptable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_subscriptable(...) __UTL is_subscriptable<__VA_ARGS__>::value
#endif
