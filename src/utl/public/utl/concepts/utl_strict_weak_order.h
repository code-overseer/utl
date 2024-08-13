// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_relation.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename R, typename T, typename U>
concept strict_weak_order = relation<R, T, U>;

UTL_NAMESPACE_END
#endif
