// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_is_array.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept array_type = UTL_TRAIT_is_array(T);

UTL_NAMESPACE_END
#endif
