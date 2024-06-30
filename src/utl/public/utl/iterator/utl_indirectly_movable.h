// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_indirectly_readable.h"
#include "utl/iterator/utl_indirectly_writable.h"
#include "utl/iterator/utl_iter_rvalue_reference_t.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
concept indirectly_movable =
    indirectly_readable<To> && indirectly_writable<From, iter_rvalue_reference_t<To>>;

template <typename From, typename To>
struct is_indirectly_movable : bool_constant<indirectly_movable<From, To>> {};

template <typename From, typename To>
inline constexpr bool is_indirectly_movable_v = indirectly_movable<From, To>;

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace indirectly_movable {

template <typename From, typename To>
auto trait_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename From, typename To>
auto trait_impl(int) noexcept -> UTL_SCOPE conjunction<UTL_SCOPE is_indirectly_readable<From>,
    UTL_SCOPE is_indirectly_writable<To, UTL_SCOPE iter_rvalue_reference_t<From>>>;

template <typename From, typename To>
using trait = decltype(UTL_SCOPE details::indirectly_movable::trait_impl<From, To>(0));

} // namespace indirectly_movable
} // namespace details

template <typename From, typename To>
struct is_indirectly_movable : details::indirectly_movable::trait<From, To> {};

#  if UTL_CXX14
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_indirectly_movable_v =
    details::indirectly_movable::trait<From, To>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
