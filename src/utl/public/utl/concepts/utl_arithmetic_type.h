// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_floating_point.h"
#include "utl/concepts/utl_integral.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept arithmetic_type = integral<T> || floating_point<T>;

UTL_NAMESPACE_END
#endif
