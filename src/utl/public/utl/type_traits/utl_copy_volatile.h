// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct copy_volatile {
    using type = To;
};
template <typename From, typename To>
struct copy_volatile<From volatile, To> {
    using type = To volatile;
};

template <typename F, typename T>
using copy_volatile_t = typename copy_volatile<F, T>::type;

UTL_NAMESPACE_END
