// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_is_lvalue_reference.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept lvalue_reference = UTL_TRAIT_is_lvalue_reference(T);

UTL_NAMESPACE_END
#endif
