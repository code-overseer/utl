// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

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

#  include "utl/type_traits/utl_underlying_type.h"
#  if UTL_CXX20
#    include "utl/concepts/utl_enum_type.h"
#  else
#    include "utl/type_traits/utl_enable_if.h"
#    include "utl/type_traits/utl_is_enum.h"
#  endif

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(enum_type) T>
UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE, _HIDE_FROM_ABI) constexpr 
UTL_ENABLE_IF_CXX11(underlying_type_t<T>, UTL_TRAIT_is_enum(T))
to_underlying(T value) noexcept {
    return static_cast<underlying_type_t<T>>(value);
}

UTL_NAMESPACE_END

#endif // UTL_CXX23 && UTL_USE_STD_to_underlying
