// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_CXX20

#  include "utl/concepts/utl_subscriptable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace subscriptable {

template <typename T, typename Arg>
concept nothrow_test = __UTL subscriptable_with<T, Arg> && requires(T t, Arg arg) {
    { t[arg] } noexcept;
};

template <typename T, typename Arg>
using nothrow_trait_t UTL_NODEBUG =
    __UTL bool_constant<__UTL details::subscriptable::nothrow_test<T, Arg>>;

} // namespace subscriptable
} // namespace details

template <typename T, typename Arg>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_subscriptable :
    __UTL details::subscriptable::nothrow_trait_t<T, Arg> {};

#  if UTL_CXX14
template <typename T, typename Arg>
UTL_INLINE_CXX17 constexpr bool is_nothrow_subscriptable_v =
    __UTL details::subscriptable::nothrow_test<T, Arg>;
#  endif // UTL_CXX14

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_subscriptable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace subscriptable {
template <typename T, typename Arg>
__UTL_HIDE_FROM_ABI auto nothrow_test(int) noexcept
    -> bool_constant<noexcept(__UTL declval<T>()[__UTL declval<Arg>()])>;
template <typename T, typename Arg>
__UTL_HIDE_FROM_ABI auto nothrow_test(float) noexcept -> __UTL false_type;

template <typename T, typename Arg>
using nothrow_trait_t UTL_NODEBUG = decltype(__UTL details::subscriptable::nothrow_test<T, Arg>(0));
} // namespace subscriptable
} // namespace details

template <typename T, typename Arg>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_subscriptable :
    __UTL details::subscriptable::nothrow_trait_t<T, Arg> {};

#  if UTL_CXX14
template <typename T, typename Arg>
UTL_INLINE_CXX17 constexpr bool is_nothrow_subscriptable_v = is_nothrow_subscriptable<T, Arg>::value;
#  endif // UTL_CXX14

UTL_NAMESPACE_END
#endif

#if UTL_CXX20
#  define UTL_TRAIT_is_nothrow_subscriptable(...) \
      __UTL details::subscriptable::nothrow_test<__VA_ARGS__>
#elif UTL_CXX14
#  define UTL_TRAIT_is_nothrow_subscriptable(...) __UTL is_nothrow_subscriptable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_nothrow_subscriptable(...) __UTL is_nothrow_subscriptable<__VA_ARGS__>::value
#endif
