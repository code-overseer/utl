// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

namespace utility {
namespace details {
template<typename T>
T&& declval_impl(int);

template<typename T>
T declval_impl(float);
}
}

template<typename T>
auto declval() noexcept -> decltype(utility::details::declval_impl<T>(0));

UTL_NAMESPACE_END
