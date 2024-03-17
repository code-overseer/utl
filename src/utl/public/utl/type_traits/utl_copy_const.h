// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct copy_const {
    using type = To;
};

template <typename From, typename To>
struct copy_const<From const, To> {
    using type = To const;
};

template <typename F, typename T>
using copy_const_t = typename copy_const<F, T>::type;

UTL_NAMESPACE_END
