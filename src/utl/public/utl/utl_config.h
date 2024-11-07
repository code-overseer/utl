/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/configuration/utl_abi.h"
#include "utl/configuration/utl_allocation.h"
#include "utl/configuration/utl_architecture.h"
#include "utl/configuration/utl_attributes.h"
#include "utl/configuration/utl_build_type.h"
#include "utl/configuration/utl_builtins.h"
#include "utl/configuration/utl_compiler.h"
#include "utl/configuration/utl_compiler_barrier.h"
#include "utl/configuration/utl_exceptions.h"
#include "utl/configuration/utl_keywords.h"
#include "utl/configuration/utl_memcmp.h"
#include "utl/configuration/utl_memcpy.h"
#include "utl/configuration/utl_namespace.h"
#include "utl/configuration/utl_pragma.h"
#include "utl/configuration/utl_simd.h"
#include "utl/configuration/utl_standard.h"
#include "utl/configuration/utl_target.h"
#include "utl/configuration/utl_types.h"

#include "utl/preprocessor/utl_attribute_list.h"

UTL_NAMESPACE_BEGIN

using ptrdiff_t = decltype((char*)0 - (char*)0);
using size_t = decltype(sizeof(0));

UTL_NAMESPACE_END

using __UTL ptrdiff_t;
using __UTL size_t;
