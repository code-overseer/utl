// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_equality_comparable.h"
#include "utl/concepts/utl_semiregular.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept regular = semiregular<T> && equality_comparable<T>;

UTL_NAMESPACE_END
#endif
