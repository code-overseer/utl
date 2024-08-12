// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_is_enum.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept enum_type = UTL_TRAIT_is_enum(T);

UTL_NAMESPACE_END
#endif
