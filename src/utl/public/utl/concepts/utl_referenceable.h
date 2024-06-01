// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept referenceable = requires(T& t) { (void)t; };

UTL_NAMESPACE_END
#endif
