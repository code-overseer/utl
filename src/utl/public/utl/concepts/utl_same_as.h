// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"

#ifdef UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T, typename U>
concept same_as = is_same_v<T, U>;

UTL_NAMESPACE_END
#endif
