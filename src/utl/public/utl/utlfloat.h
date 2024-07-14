// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

UTL_NAMESPACE_BEGIN

#if UTL_SUPPORTS_FLOAT16
using float16 = decltype(0.0f16);
#endif
#if UTL_SUPPORTS_FLOAT32
using float32 = decltype(0.0f32);
#endif
#if UTL_SUPPORTS_FLOAT64
using float64 = decltype(0.0f64);
#endif
#if UTL_SUPPORTS_FLOAT128
using float128 = decltype(0.0f128);
#endif
#if UTL_SUPPORTS_BFLOAT16
using bfloat16 = decltype(0.0bf16);
#endif

UTL_NAMESPACE_END
