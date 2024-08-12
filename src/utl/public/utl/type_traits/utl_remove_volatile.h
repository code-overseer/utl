// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::remove_volatile;

#  if UTL_CXX14

using std::remove_volatile_t;

#  else // UTL_CXX14

template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_remove_volatile 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  if __UTL_SHOULD_USE_BUILTIN(remove_volatile)
#    define UTL_BUILTIN_add_volatile(...) __remove_volatile(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(add_volatile)

#  ifdef UTL_BUILTIN_remove_volatile

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_volatile {
    using type UTL_NODEBUG = UTL_BUILTIN_remove_volatile(T);
};

template <typename T>
using remove_volatile_t = UTL_BUILTIN_remove_volatile(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_volatile

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_volatile {
    using type UTL_NODEBUG = T;
};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_volatile<T volatile> {
    using type UTL_NODEBUG = T;
};

template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_volatile

#  define UTL_TRAIT_SUPPORTED_remove_volatile 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
