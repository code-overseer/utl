// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::underlying_type;

#  if UTL_CXX14

using std::underlying_type_t;

#  else // UTL_CXX14

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  if UTL_SHOULD_USE_BUILTIN(underlying_type)
#    define UTL_BUILTIN_underlying_type(...) __underlying_type(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(underlying_type)

#  ifdef UTL_BUILTIN_underlying_type

UTL_NAMESPACE_BEGIN

template <typename T>
struct underlying_type {
    using type = UTL_BUILTIN_underlying_type(T);
};

template <typename T>
using underlying_type_t = UTL_BUILTIN_underlying_type(T);

#    define UTL_TRAIT_SUPPORTED_underlying_type 1

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_underlying_type

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct underlying_type : undefined_trait<T> {};

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_underlying_type

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
