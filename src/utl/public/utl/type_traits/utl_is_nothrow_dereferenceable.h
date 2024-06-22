// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_dereferenceable.h"
#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_dereferenceable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace nothrow_dereferenceable {
template <UTL_CONCEPT_CXX20(dereferenceable) T UTL_REQUIRES_CXX11(UTL_TRAIT_is_dereferenceable(T))>
auto evaluate(int) noexcept -> bool_constant<noexcept(*declval<T>())>;
template <typename T>
false_type evaluate(float) noexcept;

template <typename T>
using trait = decltype(evaluate<T>(0));
} // namespace nothrow_dereferenceable
} // namespace details

template <typename T>
struct is_nothrow_dereferenceable : details::nothrow_dereferenceable::trait<T> {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_dereferenceable_v =
    details::nothrow_dereferenceable::trait<T>::value;
#endif

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_nothrow_dereferenceable(...) \
      UTL_SCOPE is_nothrow_dereferenceable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_nothrow_dereferenceable(...) \
      UTL_SCOPE is_nothrow_dereferenceable<__VA_ARGS__>::value
#endif
