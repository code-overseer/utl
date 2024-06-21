// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_lvalue_reference.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace referenceable {
template <typename T>
T& check_trait(int) noexcept;
template <typename T>
T check_trait(float) noexcept;

template <typename T>
using trait = bool_constant<UTL_TRAIT_is_lvalue_reference(decltype(check_trait<T>(0)))>;
} // namespace referenceable
} // namespace details

template <typename T>
struct is_referenceable : details::referenceable::trait<T> {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_referenceable_v =
    UTL_TRAIT_is_lvalue_reference(decltype(details::referenceable::check_trait<T>(0)));
#endif

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_referenceable(...) UTL_SCOPE is_referenceable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_referenceable(...) UTL_SCOPE is_referenceable<__VA_ARGS__>::value
#endif
