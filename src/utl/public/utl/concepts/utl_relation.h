// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_predicate.h"
#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename R, typename T, typename U>
concept relation =
    predicate<R, T, T> && predicate<R, U, U> && predicate<R, T, U> && predicate<R, U, T>;

UTL_NAMESPACE_END
#endif
