// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_is_rvalue_reference.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept rvalue_reference = UTL_TRAIT_is_rvalue_reference(T);

UTL_NAMESPACE_END
#endif
