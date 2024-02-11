// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/preprocessor/utl_pragma.h"

#if UTL_HAS_BUILTIN(__builtin_unreachable)
#define UTL_BUILTIN_unreachable() __builtin_unreachable()
#elif UTL_COMPILER_GCC_AT_LEAST(4,5,0)
#define UTL_BUILTIN_unreachable() __builtin_unreachable()
#elif UTL_COMPILER_ICC_AT_LEAST(1300)
#define UTL_BUILTIN_unreachable() __builtin_unreachable()
#elif defined(UTL_COMPILER_MSVC)   /* UTL_HAS_BUILTIN(__builtin_unreachable) */
#define UTL_BUILTIN_unreachable() __assume(0)
#else   /* UTL_HAS_BUILTIN(__builtin_unreachable) */
#error "Compiler does not contain an 'unreachable' intrinsic"
#endif  /* UTL_HAS_BUILTIN(__builtin_unreachable) */

#if defined(UTL_CXX23) && defined(UTL_USE_STD_unreachable) && UTL_USE_STD_unreachable

#include <utility>

UTL_NAMESPACE_BEGIN

using std::unreachable

UTL_NAMESPACE_END

#else   // defined(UTL_CXX23) && defined(UTL_USE_STD_unreachable) && UTL_USE_STD_unreachable

#if defined(UTL_USE_STD_unreachable) && UTL_USE_STD_unreachable
UTL_PRAGMA_WARN("The current standard does not implement unreachable, `UTL_USE_STD_unreachable` ignored")
#endif  // defined(UTL_USE_STD_unreachable) && UTL_USE_STD_unreachable

UTL_NAMESPACE_BEGIN

[[noreturn]] inline void unreachable() { UTL_BUILTIN_unreachable(); }

UTL_NAMESPACE_END

#endif   // defined(UTL_CXX23) && defined(UTL_USE_STD_unreachable) && UTL_USE_STD_unreachable
