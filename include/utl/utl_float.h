// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#ifdef UTL_CXX23
#  include <stdfloat>

UTL_NAMESPACE_BEGIN

#  ifdef UTL_SUPPORTS_FLOAT16
using std::float16;
#  endif
#  ifdef UTL_SUPPORTS_FLOAT32
using std::float32;
#  endif
#  ifdef UTL_SUPPORTS_FLOAT64
using std::float64;
#  endif
#  ifdef UTL_SUPPORTS_FLOAT128
using std::float128;
#  endif
#  ifdef UTL_SUPPORTS_BFLOAT16
using std::bfloat16;
#  endif

UTL_NAMESPACE_END
