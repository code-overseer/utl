// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_pointer {
    using type UTL_NODEBUG = To;
};
template <typename From, typename To>
struct __UTL_PUBLIC_TEMPLATE copy_pointer<From*, To> {
    using type UTL_NODEBUG = To*;
};

template <typename From, typename To>
using copy_pointer_t = typename copy_pointer<From, To>::type;

UTL_NAMESPACE_END
