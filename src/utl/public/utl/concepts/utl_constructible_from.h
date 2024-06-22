// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_destructible.h"
#  include "utl/type_traits/utl_is_constructible.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename... Args>
concept constructible_from = destructible<T> && UTL_TRAIT_is_constructible(T, Args...);

UTL_NAMESPACE_END

#endif
