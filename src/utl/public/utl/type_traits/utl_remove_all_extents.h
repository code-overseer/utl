// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::remove_all_extents;

#  ifdef UTL_CXX14

using std::remove_all_extents_t;

#  else // UTL_CXX14

template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;

#    define UTL_TRAIT_SUPPORTED_remove_all_extents 1

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  ifndef UTL_DISABLE_BUILTIN_remove_all_extents
#    define UTL_DISABLE_BUILTIN_remove_all_extents 0
#  endif

#  if UTL_SHOULD_USE_BUILTIN(remove_all_extents)
#    define UTL_BUILTIN_remove_all_extents(...) __remove_all_extents(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(remove_all_extents)

#  ifdef UTL_BUILTIN_remove_all_extents

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_all_extents {
    using type = UTL_BUILTIN_remove_all_extents(T);
};

template <typename T>
using remove_all_extents_t = UTL_BUILTIN_remove_all_extents(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_all_extents

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_all_extents {
    using type = T;
};
template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;

template <typename T>
struct remove_all_extents<T[]> {
    using type = remove_all_extents_t<T>;
};
template <typename T, size_t N>
struct remove_all_extents<T[N]> {
    using type = remove_all_extents_t<T>;
};

UTL_NAMESPACE_END

#  endif

#  define UTL_TRAIT_SUPPORTED_remove_all_extents 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
