// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_enum.h"
#include "utl/type_traits/utl_underlying_type.h"
#include "utl/utility/utl_swap.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_unscoped_enum :
    bool_constant<UTL_TRAIT_is_enum(T) && UTL_TRAIT_is_convertible(T, underlying_type_t<T>)> {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unscoped_enum_v =
    UTL_TRAIT_is_enum(T) && UTL_TRAIT_is_convertible(T, underlying_type_t<T>);
#endif // UTL_CXX14

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_unscoped_enum(...) UTL_SCOPE is_unscoped_enum_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_unscoped_enum(...) UTL_SCOPE is_unscoped_enum<__VA_ARGS__>::value
#endif
