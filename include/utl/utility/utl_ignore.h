// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"

UTL_NAMESPACE_BEGIN

struct ignore_t {
#ifdef UTL_CXX14
    explicit constexpr ignore_t() noexcept = default;
    template<typename T>
    constexpr void operator=(T&&) const noexcept {}
#else
    template<typename T>
    constexpr ignore_t const& operator=(T&&) const noexcept { return *this; }
#endif
};

UTL_INLINE_CXX17 constexpr ignore_t ignore = ignore_t{};

UTL_NAMESPACE_END
