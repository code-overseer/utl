// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_is_void.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept void_type = __UTL is_void_v<T>;

UTL_NAMESPACE_END
#endif
