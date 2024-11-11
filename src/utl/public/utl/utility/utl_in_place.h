// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_STD_NAMESPACE_BEGIN
struct in_place_t;
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN

struct __UTL_ABI_PUBLIC in_place_t {
    __UTL_HIDE_FROM_ABI explicit constexpr in_place_t() noexcept = default;
};

#if UTL_CXX14
UTL_INLINE_CXX17 constexpr in_place_t in_place{};
#endif

UTL_NAMESPACE_END
