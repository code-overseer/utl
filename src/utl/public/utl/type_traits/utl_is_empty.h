// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_empty;

#  ifdef UTL_CXX17
using std::is_empty_v;
#  elif defined(UTL_CXX14) // ifdef UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_empty_v = is_empty<T>::value;
#  endif                   // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_empty 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_empty
#    define UTL_DISABLE_BUILTIN_is_empty 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_empty

#  if UTL_SHOULD_USE_BUILTIN(is_empty)
#    define UTL_BUILTIN_is_empty(...) __is_empty(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_empty)

#  ifdef UTL_BUILTIN_is_empty

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_empty : bool_constant<UTL_BUILTIN_is_empty(T)> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_empty_v = UTL_BUILTIN_is_empty(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_empty 1

#  else // ifdef UTL_BUILTIN_is_empty

UTL_NAMESPACE_BEGIN
namespace details {
template <typename T>
struct is_empty_check : T {
    int data;
};
} // namespace details

template <typename T>
struct is_empty :
    bool_constant<sizeof(details::is_empty_check<T>) == sizeof(details::is_empty_check<T>::data)> {
};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_empty_v = is_empty<T>::value;
#    endif // UTL_CXX14

#    define UTL_TRAIT_SUPPORTED_is_empty 1

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_empty

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
