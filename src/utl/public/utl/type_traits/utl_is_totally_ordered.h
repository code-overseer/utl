// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/concepts/utl_totally_ordered.h"
#include "utl/type_traits/utl_is_equality_comparable.h"
#include "utl/type_traits/utl_is_inequality_comparable.h"
#include "utl/type_traits/utl_is_subordinate_comparable.h"
#include "utl/type_traits/utl_is_superordinate_comparable.h"
#include "utl/type_traits/utl_is_three_way_comparable.h"
#include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

#if UTL_CXX20
template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_totally_ordered_with :
    UTL_SCOPE bool_constant<UTL_SCOPE totally_ordered_with<T, U>> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_totally_ordered :
    UTL_SCOPE bool_constant<UTL_SCOPE totally_ordered<T>> {};

#else
template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_totally_ordered_with :
    conjunction<is_equality_comparable_with<T, U>, is_inequality_comparable_with<T, U>,
        is_subordinate_comparable_with<T, U>, is_superordinate_comparable_with<T, U>,
        is_strict_subordinate_comparable_with<T, U>,
        is_strict_superordinate_comparable_with<T, U>> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_totally_ordered :
    conjunction<is_equality_comparable<T>, is_inequality_comparable<T>,
        is_subordinate_comparable<T>, is_superordinate_comparable<T>,
        is_strict_subordinate_comparable<T>, is_strict_superordinate_comparable<T>> {};
#endif

#if UTL_CXX20

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_totally_ordered_with_v = UTL_SCOPE totally_ordered_with<T, U>;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_totally_ordered_v = UTL_SCOPE totally_ordered<T>;

#  define UTL_TRAIT_is_totally_ordered_with(...) UTL_SCOPE totally_ordered_with<T, U>
#  define UTL_TRAIT_is_totally_ordered(...) UTL_SCOPE totally_ordered<T>

#elif UTL_CXX14

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_totally_ordered_with_v = is_totally_ordered_with<T, U>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_totally_ordered_v = is_totally_ordered<T>::value;

#  define UTL_TRAIT_is_totally_ordered_with(...) UTL_SCOPE is_totally_ordered_with_v<__VA_ARGS__>
#  define UTL_TRAIT_is_totally_ordered(...) UTL_SCOPE is_totally_ordered_v<__VA_ARGS__>

#else // UTL_CXX14

#  define UTL_TRAIT_is_totally_ordered_with(...) \
      UTL_SCOPE is_totally_ordered_with<__VA_ARGS__>::value
#  define UTL_TRAIT_is_totally_ordered(...) UTL_SCOPE is_totally_ordered<__VA_ARGS__>::value

#endif // UTL_CXX14

UTL_NAMESPACE_END
