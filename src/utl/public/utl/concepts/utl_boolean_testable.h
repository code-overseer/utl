// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_is_boolean_testable.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept boolean_testable = is_boolean_testable<T>::value;

UTL_NAMESPACE_END
#endif
