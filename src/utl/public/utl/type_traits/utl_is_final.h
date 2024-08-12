// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX14

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_final;

#  if UTL_CXX17
using std::is_final_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_final_v = is_final<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_final 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_final)
#    define UTL_BUILTIN_is_final(...) __is_final(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_final)

#  ifdef UTL_BUILTIN_is_final

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_final : bool_constant<UTL_BUILTIN_is_final(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_final_v = UTL_BUILTIN_is_final(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_final 1

#  else // ifdef UTL_BUILTIN_is_final

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_final : undefined_trait<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_final_v = is_final<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_final 0

#  endif // ifdef UTL_BUILTIN_is_final

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#ifdef UTL_BUILTIN_is_final
#  define UTL_TRAIT_is_final(...) UTL_BUILTIN_is_final(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_final(...) UTL_SCOPE is_final_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_final(...) UTL_SCOPE is_final<__VA_ARGS__>::value
#endif
