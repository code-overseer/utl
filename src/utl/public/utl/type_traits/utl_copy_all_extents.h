// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN
template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_all_extents {
    using type UTL_NODEBUG = To;
};
template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_all_extents<From[], To> : copy_all_extents<From, To[]> {};
template <typename From, typename To, size_t N>
struct __UTL_PUBLIC_TEMPLATE copy_all_extents<From[N], To> : copy_all_extents<From, To[N]> {};

template <typename F, typename T>
using copy_all_extents_t = typename copy_all_extents<F, T>::type;

UTL_NAMESPACE_END
