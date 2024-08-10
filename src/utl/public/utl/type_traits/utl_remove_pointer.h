// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::remove_pointer;

#  if UTL_CXX14

using std::remove_pointer_t;

#  else // UTL_CXX14

template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  if UTL_SHOULD_USE_BUILTIN(remove_pointer)
#    define UTL_BUILTIN_remove_pointer(...) __remove_pointer(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(remove_pointer)

#  ifdef UTL_BUILTIN_remove_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct UTL_PUBLIC_TEMPLATE remove_pointer {
    using type UTL_NODEBUG = UTL_BUILTIN_remove_pointer(T);
};

template <typename T>
using remove_pointer_t = UTL_BUILTIN_remove_pointer(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct UTL_PUBLIC_TEMPLATE remove_pointer {
    using type UTL_NODEBUG = T;
};
template <typename T>
struct UTL_PUBLIC_TEMPLATE remove_pointer<T*> {
    using type UTL_NODEBUG = T;
};
template <typename T>
struct UTL_PUBLIC_TEMPLATE remove_pointer<T* const> {
    using type UTL_NODEBUG = T;
};
template <typename T>
struct UTL_PUBLIC_TEMPLATE remove_pointer<T* volatile> {
    using type UTL_NODEBUG = T;
};
template <typename T>
struct UTL_PUBLIC_TEMPLATE remove_pointer<T* const volatile> {
    using type UTL_NODEBUG = T;
};

template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_pointer

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_remove_pointer 1
