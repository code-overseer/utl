// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::add_const;

#  if UTL_CXX14

using std::add_const_t;

#  else // UTL_CXX14

template <typename T>
using add_const_t = T const;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_add_const 1

#else // if UTL_USE_STD_TYPE_TRAITS

UTL_NAMESPACE_BEGIN

template <typename T>
struct add_const {
    using type = T const;
};

template <typename T>
using add_const_t = T const;

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_add_const 1

#endif // if UTL_USE_STD_TYPE_TRAITS
