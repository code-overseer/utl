// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

UTL_NAMESPACE_BEGIN
using size_t = decltype(sizeof(0));
UTL_NAMESPACE_END

#ifdef UTL_CXX14
#  define UTL_TRAIT_VALUE(TRAIT, ...) TRAIT##_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_VALUE(TRAIT, ...) TRAIT<__VA_ARGS__>::value
#endif
