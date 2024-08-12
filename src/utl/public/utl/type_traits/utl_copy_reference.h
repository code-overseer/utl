// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_reference {
    using type UTL_NODEBUG = To;
};
template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_reference<From&&, To> {
    using type UTL_NODEBUG = To&&;
};
template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_reference<From&, To> {
    using type UTL_NODEBUG = To&;
};

template <typename F, typename T>
using copy_reference_t = typename copy_reference<F, T>::type;

UTL_NAMESPACE_END
