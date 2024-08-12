// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_empty;

#  if UTL_CXX17
using std::is_empty_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_empty_v = is_empty<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_empty 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(is_empty)
#    define UTL_BUILTIN_is_empty(...) __is_empty(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_empty)

#  ifdef UTL_BUILTIN_is_empty

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_empty : bool_constant<UTL_BUILTIN_is_empty(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_empty_v = UTL_BUILTIN_is_empty(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_empty 1

#  else // ifdef UTL_BUILTIN_is_empty

UTL_NAMESPACE_BEGIN
namespace details {
namespace emptiness {
template <typename T>
struct UTL_ATTRIBUTE(EMPTY_BASES) check : T {
    int data;
};
} // namespace emptiness

} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_empty :
    bool_constant<sizeof(T) == 1 &&
        sizeof(details::emptiness::check<T>) == sizeof(details::emptiness::check<T>::data)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_empty_v = is_empty<T>::value;
#    endif // UTL_CXX14

#    define UTL_TRAIT_SUPPORTED_is_empty 1

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_empty

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_empty
#  define UTL_TRAIT_is_empty(...) UTL_BUILTIN_is_empty(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_empty(...) UTL_SCOPE is_empty_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_empty(...) UTL_SCOPE is_empty<__VA_ARGS__>::value
#endif
