// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_integral.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_is_unsigned.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept unsigned_integral = UTL_TRAIT_is_unsigned(T) && integral<T>;

UTL_NAMESPACE_END
#endif
