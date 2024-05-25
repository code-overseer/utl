// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::remove_const;

#  if UTL_CXX14

using std::remove_const_t;

#  else // UTL_CXX14

template <typename T>
using remove_const_t = typename remove_const<T>::type;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_remove_const 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  if UTL_SHOULD_USE_BUILTIN(remove_const)
#    define UTL_BUILTIN_add_const(...) __remove_const(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(add_const)

#  ifdef UTL_BUILTIN_remove_const

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_const {
    using type = UTL_BUILTIN_remove_const(T);
};

template <typename T>
using remove_const_t = UTL_BUILTIN_remove_const(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_const

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_const {
    using type = T;
};
template <typename T>
struct remove_const<T const> {
    using type = T;
};

template <typename T>
using remove_const_t = typename remove_const<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_const

#  define UTL_TRAIT_SUPPORTED_remove_const 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
