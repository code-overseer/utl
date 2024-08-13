// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_is_floating_point.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept floating_point = UTL_TRAIT_is_floating_point(T);

UTL_NAMESPACE_END
#endif
