// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct copy_cv {
    using type = To;
};
template <typename From, typename To>
struct copy_cv<From const, To> {
    using type = To const;
};
template <typename From, typename To>
struct copy_cv<From volatile, To> {
    using type = To volatile;
};
template <typename From, typename To>
struct copy_cv<From const volatile, To> {
    using type = To const volatile;
};

template <typename F, typename T>
using copy_cv_t = typename copy_cv<F, T>::type;

UTL_NAMESPACE_END
