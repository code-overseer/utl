// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_USE_STD_swap && UTL_CXX20

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_nothrow_swappable;
using std::is_nothrow_swappable_v;
using std::is_nothrow_swappable_with;
using std::is_nothrow_swappable_with_v;
using std::is_swappable;
using std::is_swappable_v;
using std::is_swappable_with;
using std::is_swappable_with_v;

UTL_NAMESPACE_END

#else // if UTL_USE_STD_TYPE_TRAITS && UTL_USE_STD_swap && UTL_CXX20

#  include "utl/ranges/utl_swap.h"
#  include "utl/type_traits/utl_constants.h"

#  if UTL_CXX20

#    include "utl/concepts/utl_referenceable.h"

UTL_NAMESPACE_BEGIN

template <typename L, typename R>
struct __UTL_PUBLIC_TEMPLATE is_swappable_with :
    bool_constant<ranges::details::swap::invocable<L, R>> {};
template <typename L, typename R>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_swappable_with :
    bool_constant<ranges::details::swap::nothrow_invocable<L, R>> {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_swappable : false_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_swappable : false_type {};

template <referenceable T>
struct __UTL_PUBLIC_TEMPLATE is_swappable<T> :
    bool_constant<ranges::details::swap::invocable<T&, T&>> {};
template <referenceable T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_swappable<T> :
    bool_constant<ranges::details::swap::nothrow_invocable<T&, T&>> {};

template <typename L, typename R>
inline constexpr bool is_swappable_with_v = ranges::details::swap::invocable<L, R>;
template <typename L, typename R>
inline constexpr bool is_nothrow_swappable_with_v = ranges::details::swap::nothrow_invocable<L, R>;
template <typename T>
inline constexpr bool is_swappable_v = referenceable<T> && ranges::details::swap::invocable<T&, T&>;
template <typename T>
inline constexpr bool is_nothrow_swappable_v =
    referenceable<T> && ranges::details::swap::nothrow_invocable<T&, T&>;

UTL_NAMESPACE_END

#  else // if UTL_CXX20

#    include "utl/type_traits/utl_declval.h"
#    include "utl/type_traits/utl_is_referenceable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace swappable {
template <typename L, typename R>
__UTL_HIDE_FROM_ABI auto trait_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename L, typename R>
__UTL_HIDE_FROM_ABI auto trait_impl(int) noexcept
    -> UTL_SCOPE always_true_type<decltype(UTL_SCOPE ranges::swap(
                                      UTL_SCOPE declval<L>(), UTL_SCOPE declval<R>())),
        decltype(UTL_SCOPE ranges::swap(UTL_SCOPE declval<R>(), UTL_SCOPE declval<L>()))>;

template <typename L, typename R>
__UTL_HIDE_FROM_ABI auto nothrow_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename L, typename R>
__UTL_HIDE_FROM_ABI auto nothrow_impl(int) noexcept
    -> UTL_SCOPE bool_constant<noexcept(UTL_SCOPE ranges::swap(
                                   UTL_SCOPE declval<L>(), UTL_SCOPE declval<R>())) &&
        noexcept(UTL_SCOPE ranges::swap(UTL_SCOPE declval<R>(), UTL_SCOPE declval<L>()))>;

template <typename L, typename R>
using trait UTL_NODEBUG = decltype(trait_impl<L, R>(0));
template <typename L, typename R>
using is_nothrow UTL_NODEBUG = decltype(nothrow_impl<L, R>(0));
} // namespace swappable
} // namespace details

template <typename L, typename R>
struct __UTL_PUBLIC_TEMPLATE is_swappable_with : details::swappable::trait<L, R> {};
template <typename L, typename R>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_swappable_with : details::swappable::is_nothrow<L, R> {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_swappable :
    conjunction<is_referenceable<T>, is_swappable_with<T&, T&>> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_swappable :
    conjunction<is_referenceable<T>, is_nothrow_swappable_with<T&, T&>> {};

#    if UTL_CXX14

template <typename L, typename R>
UTL_INLINE_CXX17 constexpr bool is_swappable_with_v = details::swappable::trait<L, R>::value;
template <typename L, typename R>
UTL_INLINE_CXX17 constexpr bool is_nothrow_swappable_with_v = details::swappable::is_nothrow<L, R>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_swappable_v = is_swappable<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

#    endif // if UTL_CXX14

UTL_NAMESPACE_END

#  endif // if UTL_CXX20

#endif // if UTL_USE_STD_TYPE_TRAITS && UTL_USE_STD_swap

#if UTL_CXX20
#  define UTL_TRAIT_is_swappable_with(...) UTL_SCOPE ranges::details::swap::invocable<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_swappable_with(...) \
      UTL_SCOPE ranges::details::swap::nothrow_invocable<__VA_ARGS__>
#  define UTL_TRAIT_is_swappable(...) UTL_SCOPE is_swappable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_swappable(...) UTL_SCOPE is_nothrow_swappable_v<__VA_ARGS__>
#elif UTL_CXX14
#  define UTL_TRAIT_is_swappable(...) UTL_SCOPE is_swappable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_swappable(...) UTL_SCOPE is_nothrow_swappable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_swappable_with(...) UTL_SCOPE is_swappable_with_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_swappable_with(...) \
      UTL_SCOPE is_nothrow_swappable_with_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_swappable(...) UTL_SCOPE is_swappable<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_swappable(...) UTL_SCOPE is_nothrow_swappable<__VA_ARGS__>::value
#  define UTL_TRAIT_is_swappable_with(...) UTL_SCOPE is_swappable_with<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_swappable_with(...) \
      UTL_SCOPE is_nothrow_swappable_with<__VA_ARGS__>::value
#endif

#define UTL_TRAIT_SUPPORTED_is_swappable 1
#define UTL_TRAIT_SUPPORTED_is_nothrow_swappable 1
#define UTL_TRAIT_SUPPORTED_is_swappable_with 1
#define UTL_TRAIT_SUPPORTED_is_nothrow_swappable_with 1
