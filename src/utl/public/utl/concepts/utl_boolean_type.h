// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_same_as.h"
#include "utl/type_traits/utl_remove_cv.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept boolean_type = same_as<bool, remove_cv_t<T>>;

UTL_NAMESPACE_END
#endif
