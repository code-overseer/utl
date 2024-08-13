// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::remove_all_extents;

#  if UTL_CXX14

using std::remove_all_extents_t;

#  else // UTL_CXX14

template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;

#    define UTL_TRAIT_SUPPORTED_remove_all_extents 1

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  if __UTL_SHOULD_USE_BUILTIN(remove_all_extents)
#    define UTL_BUILTIN_remove_all_extents(...) __remove_all_extents(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(remove_all_extents)

#  ifdef UTL_BUILTIN_remove_all_extents

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_all_extents {
    using type UTL_NODEBUG = UTL_BUILTIN_remove_all_extents(T);
};

template <typename T>
using remove_all_extents_t = UTL_BUILTIN_remove_all_extents(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_all_extents

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_all_extents {
    using type UTL_NODEBUG = T;
};
template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_all_extents<T[]> {
    using type UTL_NODEBUG = remove_all_extents_t<T>;
};
template <typename T, size_t N>
struct __UTL_PUBLIC_TEMPLATE remove_all_extents<T[N]> {
    using type UTL_NODEBUG = remove_all_extents_t<T>;
};

UTL_NAMESPACE_END

#  endif

#  define UTL_TRAIT_SUPPORTED_remove_all_extents 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
