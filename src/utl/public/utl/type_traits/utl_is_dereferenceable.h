// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_referenceable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace dereferenceable {
template <typename T, bool R = UTL_TRAIT_is_referenceable(decltype(*declval<T>()))>
bool_constant<R> evaluate(int) noexcept;
template <typename T>
false_type evaluate(float) noexcept;

template <typename T>
using trait = decltype(evaluate<T>(0));
} // namespace dereferenceable
} // namespace details

template <typename T>
struct is_dereferenceable : details::referenceable::trait<T> {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_dereferenceable_v = details::referenceable::trait<T>::value;
#endif

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_dereferenceable(...) UTL_SCOPE is_dereferenceable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_dereferenceable(...) UTL_SCOPE is_dereferenceable<__VA_ARGS__>::value
#endif
