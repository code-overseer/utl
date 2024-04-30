// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

UTL_NAMESPACE_BEGIN

template <size_t N>
struct unsigned_type;
template <size_t N>
using unsigned_type_t = typename unsigned_type<N>::type;

// clang-format off
template <> struct unsigned_type<1> { using type = uint8_t; };
template <> struct unsigned_type<2> { using type = uint16_t; };
template <> struct unsigned_type<4> { using type = uint32_t; };
template <> struct unsigned_type<8> { using type = uint64_t; };
// clang-format on

UTL_STD_NAMESPACE_END
