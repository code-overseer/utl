// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_constructible_from.h"
#  include "utl/concepts/utl_convertible_to.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

UTL_NAMESPACE_END

#endif
