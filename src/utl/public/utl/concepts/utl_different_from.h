// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_same_as.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T, typename U>
concept different_from = !same_as<T, U>;

UTL_NAMESPACE_END
#endif
