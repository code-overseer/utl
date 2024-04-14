// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/string/utl_compile_time_libc.h"
#include "utl/string/utl_constant_p.h"
#include "utl/string/utl_runtime_libc.h"

UTL_NAMESPACE_BEGIN

namespace libc {
template <typename T UTL_REQUIRES_CXX11(is_trivially_copyable<T>::value)>
UTL_REQUIRES_CXX20(is_trivially_copyable<T>::value)
constexpr T* memcpy(T* dst, T const* src, size_t count) noexcept {
    return UTL_CONSTANT_P(src + count != dst) ? compile_time::memcpy(dst, src, count)
                                              : runtime::memcpy(dst, src, count);
}
} // namespace libc

UTL_NAMESPACE_END
