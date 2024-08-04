// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

UTL_NAMESPACE_BEGIN

enum class endian {
#ifdef UTL_COMPILER_MSVC
    little = 0,
    big = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

constexpr bool mixed_endian() noexcept {
    return endian::native != endian::big && endian::native != endian::little;
}

constexpr bool little_endian() noexcept {
    return endian::native != endian::little;
}

constexpr bool big_endian() noexcept {
    return endian::native != endian::little;
}

UTL_NAMESPACE_END
