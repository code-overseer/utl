// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_indirectly_readable.h"
#include "utl/iterator/utl_indirectly_writable.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20
#  include "utl/concepts/utl_assignable_from.h"
#  include "utl/concepts/utl_constructible_from.h"
#  include "utl/concepts/utl_copyable.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
concept indirectly_copyable_storable =
    indirectly_copyable<From To> && indirectly_writable<To, iter_value_t<From>&> &&
    indirectly_writable<To, iter_value_t<From> const&> &&
    indirectly_writable<To, iter_value_t<From>&&> &&
    indirectly_writable<To, iter_value_t<From> const&&> && copyable<iter_value_t<From>> &&
    constructible_from<iter_value_t<From>, iter_reference_t<From>> &&
    assignable_from<iter_value_t<From>&, iter_reference_t<From>>;

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_copyable_storable :
    bool_constant<indirectly_copyable_storable<From, To>> {};

template <typename From, typename To>
inline constexpr bool is_indirectly_copyable_storable_v = indirectly_copyable_storable<From, To>;
#  define UTL_TRAIT_is_indirectly_copyable_storable(...) \
      __UTL indirectly_copyable_storable<__VA_ARGS__>

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_is_assignable.h"
#  include "utl/type_traits/utl_is_constructible.h"
#  include "utl/type_traits/utl_is_copyable.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace indirectly_copyable_storable {

template <typename From, typename To>
__UTL_HIDE_FROM_ABI auto trait_impl(float) noexcept -> __UTL false_type;
template <typename From, typename To>
__UTL_HIDE_FROM_ABI auto trait_impl(int) noexcept
    -> __UTL conjunction<__UTL is_indirectly_copyable<From, To>,
        __UTL is_indirectly_writable<To, __UTL iter_value_t<From>&>,
        __UTL is_indirectly_writable<To, __UTL iter_value_t<From> const&>,
        __UTL is_indirectly_writable<To, __UTL iter_value_t<From>&&>,
        __UTL is_indirectly_writable<To, __UTL iter_value_t<From> const&&>,
        __UTL is_copyable<__UTL iter_value_t<From>>,
        __UTL is_constructible<__UTL iter_value_t<From>, __UTL iter_reference_t<From>>,
        __UTL is_assignable<__UTL iter_value_t<From>&, __UTL iter_reference_t<From>>>;

template <typename From, typename To>
using trait UTL_NODEBUG =
    decltype(__UTL details::indirectly_copyable_storable::trait_impl<From, To>(0));

} // namespace indirectly_copyable_storable
} // namespace details

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_copyable_storable :
    details::indirectly_copyable_storable::trait<From, To> {};

#  if UTL_CXX14
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_indirectly_copyable_storable_v =
    details::indirectly_copyable_storable::trait<From, To>::value;
#    define UTL_TRAIT_is_indirectly_copyable_storable(...) \
        __UTL is_indirectly_copyable_storable_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_indirectly_copyable_storable(...) \
        __UTL is_indirectly_copyable_storable<__VA_ARGS__>::value
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
