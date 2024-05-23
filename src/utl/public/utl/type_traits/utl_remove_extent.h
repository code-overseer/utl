// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::remove_extent;

#  if UTL_CXX14

using std::remove_extent_t;

#  else // UTL_CXX14

template <typename T>
using remove_extent_t = typename remove_extent<T>::type;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_remove_extent 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  ifndef UTL_DISABLE_BUILTIN_remove_extent
#    define UTL_DISABLE_BUILTIN_remove_extent 0
#  endif

#  if UTL_SHOULD_USE_BUILTIN(remove_extent)
#    define UTL_BUILTIN_remove_extent(...) __remove_extent(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(remove_extent)

#  ifdef UTL_BUILTIN_remove_extent

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_extent {
    using type = UTL_BUILTIN_remove_extent(T);
};

template <typename T>
using remove_extent_t = UTL_BUILTIN_remove_extent(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_extent

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_extent {
    using type = T;
};
template <typename T>
struct remove_extent<T[]> {
    using type = T;
};
template <typename T, size_t N>
struct remove_extent<T[N]> {
    using type = T;
};
template <typename T>
using remove_extent_t = typename remove_extent<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_extent

#  define UTL_TRAIT_SUPPORTED_remove_extent 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
