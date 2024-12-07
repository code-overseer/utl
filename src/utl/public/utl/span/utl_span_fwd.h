// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

UTL_INLINE_CXX17 constexpr size_t dynamic_extent = static_cast<size_t>(-1);

template <typename T, size_t E = dynamic_extent>
class __UTL_PUBLIC_TEMPLATE span;

UTL_NAMESPACE_END
