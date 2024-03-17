// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::add_cv;

#  ifdef UTL_CXX14

using std::add_cv_t;

#  else // UTL_CXX14

template <typename T>
using add_cv_t = T const volatile;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_add_cv 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

UTL_NAMESPACE_BEGIN

template <typename T>
struct add_cv {
    using type = T const volatile;
};

template <typename T>
using add_cv_t = T const volatile;

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_add_cv 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
