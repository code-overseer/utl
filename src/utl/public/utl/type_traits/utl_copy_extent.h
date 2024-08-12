// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_extent {
    using type UTL_NODEBUG = To;
};
template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_extent<From[], To> {
    using type UTL_NODEBUG = To[];
};
template <typename From, typename To, size_t N>
struct __UTL_PUBLIC_TEMPLATE copy_extent<From[N], To> {
    using type UTL_NODEBUG = To[N];
};

template <typename F, typename T>
using copy_extent_t = typename copy_extent<F, T>::type;

UTL_NAMESPACE_END
