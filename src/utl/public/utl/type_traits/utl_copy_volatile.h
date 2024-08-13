// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_volatile {
    using type UTL_NODEBUG = To;
};
template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_volatile<From volatile, To> {
    using type UTL_NODEBUG = To volatile;
};

template <typename F, typename T>
using copy_volatile_t = typename copy_volatile<F, T>::type;

UTL_NAMESPACE_END
