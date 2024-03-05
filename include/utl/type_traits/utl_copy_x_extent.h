// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct copy_extent {
    using type = To;
};
template <typename From, typename To>
struct copy_extent<From[], To> {
    using type = To[];
};
template <typename From, typename To, size_t N>
struct copy_extent<From[N], To> {
    using type = To[N];
};

template <typename From, typename To>
struct copy_all_extents {
    using type = To;
};
template <typename From, typename To>
struct copy_all_extents<From[], To> : copy_all_extents<From, To[]> {};
template <typename From, typename To, size_t N>
struct copy_all_extents<From[N], To> : copy_all_extents<From, To[N]> {};

template <typename F, typename T>
using copy_extent_t = typename copy_extent<F, T>::type;
template <typename F, typename T>
using copy_all_extents_t = typename copy_all_extents<F, T>::type;

UTL_NAMESPACE_END
