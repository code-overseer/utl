// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

#  include "utl/concepts/utl_copyable.h"
#  include "utl/concepts/utl_referenceable.h"
#  include "utl/iterator/utl_incrementable.h"
#  include "utl/iterator/utl_input_or_output_iterator.h"

UTL_NAMESPACE_BEGIN

template <typename It>
concept legacy_iterator = input_or_output_iterator<It> && copyable<It>;

template <typename It>
struct UTL_PUBLIC_TEMPLATE is_legacy_iterator : bool_constant<legacy_iterator<It>> {};

template <typename It>
inline constexpr bool is_legacy_iterator_v = legacy_iterator<It>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_is_copyable.h"
#  include "utl/type_traits/utl_is_dereferenceable.h"
#  include "utl/type_traits/utl_is_referenceable.h"
#  include "utl/type_traits/utl_is_same.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace legacy_iterator {

template <typename T>
UTL_HIDE_FROM_ABI auto post_incrementable(float) noexcept -> UTL_SCOPE false_type;

template <typename T>
UTL_HIDE_FROM_ABI auto post_incrementable(int) noexcept
    -> UTL_SCOPE is_referenceable<decltype(*static_cast<T& (*)()>(0)()++)>;

template <typename T>
UTL_HIDE_FROM_ABI auto pre_incrementable(float) noexcept -> UTL_SCOPE false_type;

template <typename T>
UTL_HIDE_FROM_ABI auto pre_incrementable(int) noexcept
    -> UTL_SCOPE is_same<T&, decltype(++static_cast<T& (*)()>(0)())>;

template <typename T>
using implemented UTL_NODEBUG = UTL_SCOPE conjunction<UTL_SCOPE is_dereferenceable<T>,
    decltype(UTL_SCOPE details::legacy_iterator::post_incrementable<T>(0)),
    decltype(UTL_SCOPE details::legacy_iterator::pre_incrementable<T>(0)),
    UTL_SCOPE is_copyable<T>>;

} // namespace legacy_iterator
} // namespace details

template <typename It>
struct UTL_PUBLIC_TEMPLATE is_legacy_iterator : details::legacy_iterator::implemented<It> {};

#  if UTL_CXX14
template <typename It>
UTL_INLINE_CXX17 constexpr bool is_legacy_iterator_v = UTL_SCOPE is_legacy_iterator<It>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20

#if UTL_CXX14
#  define UTL_TRAIT_is_legacy_iterator(...) UTL_SCOPE is_legacy_iterator_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_legacy_iterator(...) UTL_SCOPE is_legacy_iterator<__VA_ARGS__>::value
#endif
