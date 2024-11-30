// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/expected/utl_expected_common.h"
#define UTL_EXPECTED_PRIVATE_HEADER_GUARD
// TODO: ABI Selection
#if UTL_CXX20
#  include "utl/expected/utl_expected_cpp20.h"
#else
#  include "utl/expected/utl_expected_cpp17.h"
#endif
#undef UTL_EXPECTED_PRIVATE_HEADER_GUARD
