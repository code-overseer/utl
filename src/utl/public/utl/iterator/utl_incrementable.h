// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_weakly_incrementable.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

#  include "utl/concepts/utl_regular.h"
#  include "utl/concepts/utl_signed_integral.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace incrementable {

using __UTL details::weakly_incrementable::pre_incrementable;

template <typename T>
concept post_incrementable = requires(T t) {
    { t++ } -> __UTL same_as<T>;
};

template <typename T>
concept nothrow_post_incrementable = post_incrementable<T> && requires(T t) {
    { t++ } noexcept;
};

template <typename T>
concept nothrow_pre_incrementable = pre_incrementable<T> && requires(T t) {
    { ++t } noexcept;
};

} // namespace incrementable
} // namespace details

template <typename T>
concept incrementable = __UTL regular<T> && __UTL weakly_incrementable<T> &&
    details::incrementable::post_incrementable<T>;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_incrementable : bool_constant<incrementable<T>> {};

template <typename T>
inline constexpr bool is_incrementable_v = incrementable<T>;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_post_incrementable :
    bool_constant<details::incrementable::nothrow_post_incrementable<T>> {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_pre_incrementable :
    bool_constant<details::incrementable::nothrow_pre_incrementable<T>> {};

template <typename T>
inline constexpr bool is_nothrow_post_incrementable_v =
    details::incrementable::nothrow_post_incrementable<T>;

template <typename T>
inline constexpr bool is_nothrow_pre_incrementable_v =
    details::incrementable::nothrow_pre_incrementable<T>;

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_is_copyable.h"
#  include "utl/type_traits/utl_is_default_constructible.h"
#  include "utl/type_traits/utl_is_equality_comparable.h"
#  include "utl/type_traits/utl_void_t.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace incrementable {

using __UTL details::weakly_incrementable::pre_incrementable;

template <typename T>
__UTL_HIDE_FROM_ABI auto post_incrementable_impl(int) noexcept
    -> __UTL is_same<decltype(__UTL declval<T&>()++), T>;
template <typename T>
__UTL_HIDE_FROM_ABI auto post_incrementable_impl(float) noexcept -> __UTL false_type;

template <typename T>
using post_incrementable UTL_NODEBUG =
    decltype(__UTL details::incrementable::post_incrementable_impl<T>(0));

template <typename T>
__UTL_HIDE_FROM_ABI auto nothrow_post_incrementable_impl(int) noexcept
    -> __UTL bool_constant<noexcept(__UTL declval<T&>()++)>;
template <typename T>
__UTL_HIDE_FROM_ABI auto nothrow_post_incrementable_impl(float) noexcept -> __UTL false_type;

template <typename T>
using nothrow_post_incrementable UTL_NODEBUG =
    decltype(__UTL details::incrementable::post_incrementable_impl<T>(0));

template <typename T>
__UTL_HIDE_FROM_ABI auto nothrow_pre_incrementable_impl(int) noexcept
    -> __UTL bool_constant<noexcept(++__UTL declval<T&>())>;
template <typename T>
__UTL_HIDE_FROM_ABI auto nothrow_pre_incrementable_impl(float) noexcept -> __UTL false_type;

template <typename T>
using nothrow_pre_incrementable UTL_NODEBUG =
    decltype(__UTL details::incrementable::post_incrementable_impl<T>(0));

} // namespace incrementable
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_post_incrementable :
    details::incrementable::nothrow_post_incrementable<T> {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_pre_incrementable :
    details::incrementable::nothrow_pre_incrementable<T> {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_incrementable :
    conjunction<is_copyable<T>, is_default_constructible<T>, is_equality_comparable<T>,
        is_weakly_incrementable<T>, details::incrementable::post_incrementable<T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_incrementable_v = is_incrementable<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_pre_incrementable_v = is_nothrow_pre_incrementable<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_post_incrementable_v = is_nothrow_post_incrementable<T>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
