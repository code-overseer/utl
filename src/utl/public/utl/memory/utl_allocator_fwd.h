// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

UTL_STD_NAMESPACE_BEGIN
/* UTL_UNDEFINED_BEHAVIOUR */
template <typename>
class __UTL_PUBLIC_TEMPLATE allocator;
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN
template <typename>
class __UTL_PUBLIC_TEMPLATE allocator;

template <typename>
struct __UTL_PUBLIC_TEMPLATE allocator_traits;

template <typename pointer, typename size_type>
struct __UTL_PUBLIC_TEMPLATE allocation_result {
    pointer ptr;
    size_type size;
};

UTL_NAMESPACE_END
