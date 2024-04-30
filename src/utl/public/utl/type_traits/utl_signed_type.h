// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

UTL_NAMESPACE_BEGIN

template <size_t N>
struct signed_type;
template <size_t N>
using signed_type_t = typename signed_type<N>::type;

// clang-format off
template <> struct signed_type<1> { using type = int8_t; };
template <> struct signed_type<2> { using type = int16_t; };
template <> struct signed_type<4> { using type = int32_t; };
template <> struct signed_type<8> { using type = int64_t; };
// clang-format on

UTL_STD_NAMESPACE_END
