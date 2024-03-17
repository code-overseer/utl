// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct copy_pointer {
    using type = To;
};
template <typename From, typename To>
struct copy_pointer<From*, To> {
    using type = To*;
};

template <typename From, typename To>
using copy_pointer_t = typename copy_pointer<From, To>::type;

UTL_NAMESPACE_END
