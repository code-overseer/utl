// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX14)

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_final;

#  ifdef UTL_CXX17
using std::is_final_v;
#  elif defined(UTL_CXX14) // ifdef UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_final_v = is_final<T>::value;
#  endif                   // ifdef UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_final 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_is_final
#    define UTL_DISABLE_BUILTIN_is_final 0
#  endif // ifndef UTL_DISABLE_BUILTIN_is_final

#  if UTL_SHOULD_USE_BUILTIN(is_final)
#    define UTL_BUILTIN_is_final(...) __is_final(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_final)

#  ifdef UTL_BUILTIN_is_final

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_final : bool_constant<UTL_BUILTIN_is_final(T)> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_final_v = UTL_BUILTIN_is_final(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_final 1

#  else // ifdef UTL_BUILTIN_is_final

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_final : true_type {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_final_v = is_final<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_final 0

#  endif // ifdef UTL_BUILTIN_is_final

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
