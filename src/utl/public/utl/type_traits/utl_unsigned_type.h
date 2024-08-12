// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

UTL_NAMESPACE_BEGIN

template <size_t N>
struct unsigned_type;
template <size_t N>
using unsigned_type_t = typename unsigned_type<N>::type;

// clang-format off
template <> struct __UTL_PUBLIC_TEMPLATE unsigned_type<1> { using type UTL_NODEBUG = uint8_t; };
template <> struct __UTL_PUBLIC_TEMPLATE unsigned_type<2> { using type UTL_NODEBUG = uint16_t; };
template <> struct __UTL_PUBLIC_TEMPLATE unsigned_type<4> { using type UTL_NODEBUG = uint32_t; };
template <> struct __UTL_PUBLIC_TEMPLATE unsigned_type<8> { using type UTL_NODEBUG = uint64_t; };
// clang-format on

UTL_STD_NAMESPACE_END
