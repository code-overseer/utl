// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_indirectly_readable.h"
#include "utl/iterator/utl_indirectly_writable.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
concept indirectly_copyable =
    indirectly_readable<From> && indirectly_writable<To, iter_reference_t<From>>;

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_copyable :
    bool_constant<indirectly_copyable<From, To>> {};

template <typename From, typename To>
inline constexpr bool is_indirectly_copyable_v = indirectly_copyable<From, To>;
#  define UTL_TRAIT_is_indirectly_copyable(...) __UTL indirectly_copyable<__VA_ARGS__>

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace indirectly_copyable {

template <typename From, typename To>
__UTL_HIDE_FROM_ABI auto trait_impl(float) noexcept -> __UTL false_type;
template <typename From, typename To>
__UTL_HIDE_FROM_ABI auto trait_impl(int) noexcept
    -> __UTL conjunction<__UTL is_indirectly_readable<From>,
        __UTL is_indirectly_writable<To, __UTL iter_reference_t<From>>>;

template <typename From, typename To>
using trait UTL_NODEBUG = decltype(__UTL details::indirectly_copyable::trait_impl<From, To>(0));

} // namespace indirectly_copyable
} // namespace details

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_copyable :
    details::indirectly_copyable::trait<From, To> {};

#  if UTL_CXX14
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_indirectly_copyable_v =
    details::indirectly_copyable::trait<From, To>::value;
#    define UTL_TRAIT_is_indirectly_copyable(...) __UTL is_indirectly_copyable_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_indirectly_copyable(...) __UTL is_indirectly_copyable<__VA_ARGS__>::value
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
