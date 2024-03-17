// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
struct copy_all_pointers {
    using type = To;
};
template <typename From, typename To>
struct copy_all_pointers<From*, To> : copy_all_pointers<From, To*> {};

template <typename From, typename To>
using copy_all_pointers_t = typename copy_all_pointers<From, To>::type;

UTL_NAMESPACE_END
