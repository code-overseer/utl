// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_pointer_interconvertible_base_of;
using std::is_pointer_interconvertible_base_of_v;

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_pointer_interconvertible_base_of 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_pointer_interconvertible_base_of)
#    define UTL_BUILTIN_is_pointer_interconvertible_base_of(...) \
        __is_pointer_interconvertible_base_of(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_pointer_interconvertible_base_of)

#  ifdef UTL_BUILTIN_is_pointer_interconvertible_base_of

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_pointer_interconvertible_base_of :
    bool_constant<UTL_BUILTIN_is_pointer_interconvertible_base_of(T, U)> {};

#    if UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_pointer_interconvertible_base_of_v =
    UTL_BUILTIN_is_pointer_interconvertible_base_of(T, U);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_pointer_interconvertible_base_of 1

#  else // ifdef UTL_BUILTIN_is_pointer_interconvertible_base_of

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_pointer_interconvertible_base_of : undefined_trait<T, U> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_pointer_interconvertible_base_of<T, T> : true_type {};

#    if UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_pointer_interconvertible_base_of_v =
    is_pointer_interconvertible_base_of<T, U>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_pointer_interconvertible_base_of

#endif // ifdef UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#ifdef UTL_BUILTIN_is_pointer_interconvertible_base_of
#  define UTL_TRAIT_is_pointer_interconvertible_base_of(...) \
      UTL_BUILTIN_is_pointer_interconvertible_base_of(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_pointer_interconvertible_base_of(...) \
      UTL_SCOPE is_pointer_interconvertible_base_of_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_pointer_interconvertible_base_of(...) \
      UTL_SCOPE is_pointer_interconvertible_base_of<__VA_ARGS__>::value
#endif
