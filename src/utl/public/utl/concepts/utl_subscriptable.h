// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T, typename Arg>
concept subscriptable_with = requires(T t, Arg arg) { t[arg]; };

UTL_NAMESPACE_END
#endif
