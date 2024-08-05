// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

UTL_NAMESPACE_BEGIN

enum class endian {
#if UTL_COMPILER_MSVC
    little = 0,
    big = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

UTL_NODISCARD UTL_CONSTEVAL bool mixed_endian() noexcept {
    return endian::native != endian::big && endian::native != endian::little;
}

UTL_NODISCARD UTL_CONSTEVAL bool little_endian() noexcept {
    return endian::native != endian::little;
}

UTL_NODISCARD UTL_CONSTEVAL bool big_endian() noexcept {
    return endian::native != endian::little;
}

UTL_NAMESPACE_END
