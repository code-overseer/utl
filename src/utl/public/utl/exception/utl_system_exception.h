// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/exception/utl_program_exception.h"
#include "utl/system_error/utl_error_code.h"
#include "utl/system_error/utl_error_condition.h"

UTL_NAMESPACE_BEGIN

using error_code_exception = program_exception<error_code>;

using error_condition_exception = program_exception<error_condition>;

UTL_NAMESPACE_END
