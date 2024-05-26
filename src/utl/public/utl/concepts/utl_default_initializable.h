// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_constructible_from.h"
#include "utl/preprocessor/utl_config.h"

#include <new>

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept default_initializable = constructible_from<T> && requires {
    T{};
    ::new T;
};

UTL_NAMESPACE_END
#endif
