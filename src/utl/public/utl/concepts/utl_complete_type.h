// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_is_complete.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept complete_type = UTL_TRAIT_is_complete(T);

UTL_NAMESPACE_END
#endif
