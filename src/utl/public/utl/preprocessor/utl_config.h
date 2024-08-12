/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/configuration/utl_abi.h"
#include "utl/configuration/utl_allocation.h"
#include "utl/configuration/utl_architecture.h"
#include "utl/configuration/utl_attributes.h"
#include "utl/configuration/utl_builtins.h"
#include "utl/configuration/utl_compiler.h"
#include "utl/configuration/utl_exceptions.h"
#include "utl/configuration/utl_keywords.h"
#include "utl/configuration/utl_namespace.h"
#include "utl/configuration/utl_pragma.h"
#include "utl/configuration/utl_simd.h"
#include "utl/configuration/utl_standard.h"
#include "utl/configuration/utl_target.h"
#include "utl/configuration/utl_types.h"

#if __OPTIMIZE__
#  define UTL_OPTIMIZATIONS_ENABLED 1
#endif
