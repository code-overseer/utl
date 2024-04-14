// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if defined(UTL_CXX23) && defined(UTL_USE_STD_unreachable) && UTL_USE_STD_unreachable

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::unreachable

    UTL_NAMESPACE_END

#else // defined(UTL_CXX23) && defined(UTL_USE_STD_unreachable) && UTL_USE_STD_unreachable

#  if defined(UTL_USE_STD_unreachable) && UTL_USE_STD_unreachable
UTL_PRAGMA_WARN(
    "The current standard does not implement unreachable, `UTL_USE_STD_unreachable` ignored")
#  endif // defined(UTL_USE_STD_unreachable) && UTL_USE_STD_unreachable

UTL_NAMESPACE_BEGIN

[[noreturn]] inline void unreachable() {
    UTL_BUILTIN_unreachable();
}

UTL_NAMESPACE_END

#endif // defined(UTL_CXX23) && defined(UTL_USE_STD_unreachable) && UTL_USE_STD_unreachable
