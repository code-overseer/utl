// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/preprocessor/utl_pragma.h"

#if defined(UTL_CXX17) && defined(UTL_USE_STD_as_const) && UTL_USE_STD_as_const

#include <utility>

UTL_NAMESPACE_BEGIN

using std::as_const

UTL_NAMESPACE_END

#else   // defined(UTL_CXX17) && defined(UTL_USE_STD_as_const) && UTL_USE_STD_as_const

#if defined(UTL_USE_STD_as_const) && UTL_USE_STD_as_const
UTL_PRAGMA_WARN("The current standard does not implement constexpr as_const, `UTL_USE_STD_as_const` ignored")
#endif  // defined(UTL_USE_STD_as_const) && UTL_USE_STD_as_const

UTL_NAMESPACE_BEGIN

template<typename T>
void as_const(T&&) noexcept = delete;

template<typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC)
constexpr T const& as_const(T const& value UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept { return value; }

template<typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC)
constexpr T const& as_const(T& value UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept { return value; }

UTL_NAMESPACE_END

#endif   // defined(UTL_CXX17) && defined(UTL_USE_STD_as_const) && UTL_USE_STD_as_const
