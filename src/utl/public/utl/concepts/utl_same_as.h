// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_is_same.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T, typename U>
concept same_as = UTL_TRAIT_is_same(T, U);

UTL_NAMESPACE_END
#endif
