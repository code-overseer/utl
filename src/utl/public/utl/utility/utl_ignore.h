// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

struct __UTL_ABI_PUBLIC ignore_t {
    __UTL_HIDE_FROM_ABI explicit constexpr ignore_t() noexcept = default;
#if UTL_CXX14
    template <typename T>
    __UTL_HIDE_FROM_ABI constexpr void operator=(T&&) const noexcept {}
#else
    template <typename T>
    __UTL_HIDE_FROM_ABI constexpr ignore_t const& operator=(T&&) const noexcept {
        return *this;
    }
#endif
};

UTL_INLINE_CXX17 constexpr ignore_t ignore{};

UTL_NAMESPACE_END
