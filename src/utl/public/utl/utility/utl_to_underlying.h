// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX23 && UTL_USE_STD_to_underlying

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::to_underlying;

UTL_NAMESPACE_END

#else // UTL_CXX23 && UTL_USE_STD_to_underlying

#  if UTL_USE_STD_to_underlying
UTL_PRAGMA_WARN(
    "The current standard does not implement to_underlying, `UTL_USE_STD_to_underlying` ignored")
#  endif // UTL_USE_STD_to_underlying

#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_enum.h"
#  include "utl/type_traits/utl_underlying_type.h"

UTL_NAMESPACE_BEGIN

template <typename Enum>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC)
constexpr enable_if_t<is_enum<Enum>::value, underlying_type_t<Enum>> to_underlying(
    Enum value) noexcept {
    return static_cast<underlying_type_t<Enum>>(value);
}

UTL_NAMESPACE_END

#endif // UTL_CXX23 && UTL_USE_STD_to_underlying
