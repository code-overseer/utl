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

template <typename From, typename To>
struct copy_volatile {
    using type = To;
};
template <typename From, typename To>
struct copy_volatile<From volatile, To> {
    using type = To volatile;
};

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
using copy_const_t = typename copy_const<F, T>::type;
template <typename F, typename T>
using copy_volatile_t = typename copy_volatile<F, T>::type;
template <typename F, typename T>
using copy_cv_t = typename copy_cv<F, T>::type;

UTL_NAMESPACE_END
