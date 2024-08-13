// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX20
#  include "utl/type_traits/utl_is_nothrow_destructible.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept destructible = UTL_TRAIT_is_nothrow_destructible(T);

UTL_NAMESPACE_END

#endif
