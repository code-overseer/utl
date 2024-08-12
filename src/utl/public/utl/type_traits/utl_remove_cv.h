// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::remove_cv;

#  if UTL_CXX14

using std::remove_cv_t;

#  else // UTL_CXX14

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_remove_cv 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  if UTL_SHOULD_USE_BUILTIN(remove_cv)
#    define UTL_BUILTIN_add_cv(...) __remove_cv(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(add_cv)

#  ifdef UTL_BUILTIN_remove_cv

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_cv {
    using type UTL_NODEBUG = UTL_BUILTIN_remove_cv(T);
};

template <typename T>
using remove_cv_t = UTL_BUILTIN_remove_cv(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_cv

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_cv {
    using type UTL_NODEBUG = T;
};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_cv<T volatile> {
    using type UTL_NODEBUG = T;
};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_cv<T const> {
    using type UTL_NODEBUG = T;
};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_cv<T const volatile> {
    using type UTL_NODEBUG = T;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_cv

#  define UTL_TRAIT_SUPPORTED_remove_cv 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
