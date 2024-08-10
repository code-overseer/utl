// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

namespace utility {
namespace details {
template <typename T>
UTL_HIDE_FROM_ABI T&& declval_impl(int) noexcept;

template <typename T>
UTL_HIDE_FROM_ABI T declval_impl(float) noexcept;
} // namespace details
} // namespace utility

template <typename T>
UTL_HIDE_FROM_ABI auto declval() noexcept -> decltype(utility::details::declval_impl<T>(0));

UTL_NAMESPACE_END
