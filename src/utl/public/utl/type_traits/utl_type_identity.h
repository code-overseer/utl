// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct UTL_PUBLIC_TEMPLATE type_identity {
    using type UTL_NODEBUG = T;
};
template <typename T>
using type_identity_t = typename type_identity<T>::type;

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_type_dentity 1