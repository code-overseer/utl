// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_is_complete.h"

#ifdef UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept complete = (sizeof(T) > 0);

UTL_NAMESPACE_END
#endif
