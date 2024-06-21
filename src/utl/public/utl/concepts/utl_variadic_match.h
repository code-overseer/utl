// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T, typename U>
concept variadic_match = true;

UTL_NAMESPACE_END
#endif
