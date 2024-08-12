// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX23 && UTL_USE_STD_unreachable

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::unreachable

    UTL_NAMESPACE_END

#else // UTL_CXX23 && UTL_USE_STD_unreachable

#  if UTL_USE_STD_unreachable
UTL_PRAGMA_WARN(
    "The current standard does not implement unreachable, `UTL_USE_STD_unreachable` ignored")
#  endif // UTL_USE_STD_unreachable

UTL_NAMESPACE_BEGIN

UTL_ATTRIBUTES(NORETURN, _HIDE_FROM_ABI) inline void unreachable() {
    UTL_BUILTIN_unreachable();
}

UTL_NAMESPACE_END

#endif // UTL_CXX23 && UTL_USE_STD_unreachable
