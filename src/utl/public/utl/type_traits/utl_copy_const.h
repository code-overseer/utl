// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_const {
    using type UTL_NODEBUG = To;
};

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_const<From const, To> {
    using type UTL_NODEBUG = To const;
};

template <typename F, typename T>
using copy_const_t = typename copy_const<F, T>::type;

UTL_NAMESPACE_END
