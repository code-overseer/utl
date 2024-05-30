// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_relation.h"
#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename R, typename T, typename U>
concept strict_weak_order = relation<R, T, U>;

UTL_NAMESPACE_END
#endif
