// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_indirectly_movable.h"
#include "utl/iterator/utl_indirectly_writable.h"
#include "utl/iterator/utl_iter_rvalue_reference_t.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20
#  include "utl/concepts/utl_assignable_from.h"
#  include "utl/concepts/utl_constructible_from.h"
#  include "utl/concepts/utl_movable.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
concept indirectly_movable_storable = indirectly_movable<From, To>　&&
    indirectly_writable<To, iter_value_t<From>> && movable<iter_value_t<From>> &&
    constructible_from<iter_value_t<From>, iter_rvalue_reference_t<From>> &&
    assignable_from<iter_value_t<From>&, iter_rvalue_reference_t<From>>;

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_movable_storable :
    bool_constant<indirectly_movable_storable<From, To>> {};

template <typename From, typename To>
inline constexpr bool is_indirectly_movable_storable_v = indirectly_movable_storable<From, To>;

UTL_NAMESPACE_END

#else
#  include "utl/type_traits/utl_is_assignable.h"
#  include "utl/type_traits/utl_is_constructible.h"
#  include "utl/type_traits/utl_is_movable.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace indirectly_movable_storable {
template <typename From, typename To>
__UTL_HIDE_FROM_ABI auto trait_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename From, typename To>
__UTL_HIDE_FROM_ABI auto trait_impl(int) noexcept
    -> UTL_SCOPE conjunction<UTL_SCOPE is_indirectly_movable<From, To>,
        UTL_SCOPE is_indirectly_writable<To, UTL_SCOPE iter_value_t<From>>,
        UTL_SCOPE is_movable<UTL_SCOPE iter_value_t<From>>,
        UTL_SCOPE
            is_constructible<UTL_SCOPE iter_value_t<From>, UTL_SCOPE iter_rvalue_reference_t<From>>,
        UTL_SCOPE
            is_assignable<UTL_SCOPE iter_value_t<From>&, UTL_SCOPE iter_rvalue_reference_t<From>>>;

template <typename From, typename To>
using trait UTL_NODEBUG =
    decltype(UTL_SCOPE details::indirectly_movable_storable::trait_impl<From, To>(0));

} // namespace indirectly_movable_storable
} // namespace details

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_movable_storable :
    details::indirectly_movable_storable::trait<From, To> {};

#  if UTL_CXX14
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_indirectly_movable_storable_v =
    details::indirectly_movable_storable::trait<From, To>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
