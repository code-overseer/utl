// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct UTL_PUBLIC_TEMPLATE alignment_of : size_constant<alignof(T)> {};

template <typename T>
UTL_INLINE_CXX17 constexpr size_t alignment_of_v = alignof(T);

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_alignment_of 1
