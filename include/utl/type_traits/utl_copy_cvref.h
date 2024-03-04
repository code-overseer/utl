// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/type_traits/utl_copy_x_cv.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To> struct copy_cvref : copy_cv<From, To> {};
template <typename From, typename To> struct copy_cvref<From&, To> {
    using type = typename copy_cv<From, To>::type&;
};
template <typename From, typename To> struct copy_cvref<From&&, To> {
    using type = typename copy_cv<From, To>::type&&;
};

template <typename F, typename T> using copy_cvref_t = typename copy_cvref<F, T>::type;

UTL_NAMESPACE_END
