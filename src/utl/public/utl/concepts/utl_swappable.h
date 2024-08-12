// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX20

#  include "utl/concepts/utl_common_reference_with.h"
#  include "utl/ranges/utl_swap.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
concept swappable_with = common_reference_with<T, U> && ranges::details::swap::invocable<T, U> &&
    ranges::details::swap::invocable<U, U> && ranges::details::swap::invocable<T, T>;

template <typename T>
concept swappable = swappable_with<T&, T&>;

UTL_NAMESPACE_END

#endif
