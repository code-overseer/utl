// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_STD_NAMESPACE_BEGIN
/* UTL_UNDEFINED_BEHAVIOUR */
template <typename> class reference_wrapper;
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN

template <typename> class reference_wrapper;

template <typename T> struct unwrap_reference {
    using type = T;
};

template <typename T> struct unwrap_reference<reference_wrapper<T>> {
    using type = T&;
};

template <typename T> struct unwrap_reference<::std::reference_wrapper<T>> {
    using type = T&;
};

template <typename T> using unwrap_reference_t = typename unwrap_reference<T>::type;

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_unwrap_reference 1
