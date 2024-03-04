// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To> struct copy_reference {
    using type = To;
};
template <typename From, typename To> struct copy_reference<From&&, To> {
    using type = To&&;
};
template <typename From, typename To> struct copy_reference<From&, To> {
    using type = To&;
};

template <typename F, typename T> using copy_reference_t = typename copy_reference<F, T>::type;

UTL_NAMESPACE_END
