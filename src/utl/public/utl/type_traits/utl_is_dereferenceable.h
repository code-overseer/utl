// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_referenceable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace dereferenceable {
template <typename T>
UTL_HIDE_FROM_ABI auto trait_impl(int) noexcept
    -> UTL_SCOPE is_referenceable<decltype(*UTL_SCOPE declval<T>())>;
template <typename T>
UTL_HIDE_FROM_ABI auto trait_impl(float) noexcept -> UTL_SCOPE false_type;

template <typename T>
using trait UTL_NODEBUG = decltype(trait_impl<T>(0));

template <typename T>
UTL_HIDE_FROM_ABI auto nothrow_impl(int) noexcept
    -> UTL_SCOPE bool_constant<noexcept(*UTL_SCOPE declval<T>())>;
template <typename T>
UTL_HIDE_FROM_ABI auto nothrow_impl(float) noexcept -> UTL_SCOPE false_type;

template <typename T>
using is_nothrow UTL_NODEBUG = decltype(nothrow_impl<T>(0));

} // namespace dereferenceable
} // namespace details

template <typename T>
struct UTL_PUBLIC_TEMPLATE is_dereferenceable : details::dereferenceable::trait<T> {};

template <typename T>
struct UTL_PUBLIC_TEMPLATE is_nothrow_dereferenceable : details::dereferenceable::is_nothrow<T> {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_dereferenceable_v = details::dereferenceable::trait<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_dereferenceable_v = details::dereferenceable::is_nothrow<T>::value;
#endif

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_dereferenceable(...) UTL_SCOPE is_dereferenceable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_dereferenceable(...) \
      UTL_SCOPE is_nothrow_dereferenceable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_dereferenceable(...) UTL_SCOPE is_dereferenceable<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_dereferenceable(...) \
      UTL_SCOPE is_nothrow_dereferenceable<__VA_ARGS__>::value
#endif
