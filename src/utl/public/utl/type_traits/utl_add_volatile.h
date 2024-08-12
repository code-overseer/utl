// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::add_volatile;

#  if UTL_CXX14

using std::add_volatile_t;

#  else // UTL_CXX14

template <typename T>
using add_volatile_t = T volatile;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_add_volatile 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE add_volatile {
    using type UTL_NODEBUG = T volatile;
};

template <typename T>
using add_volatile_t = T volatile;

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_add_volatile 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
