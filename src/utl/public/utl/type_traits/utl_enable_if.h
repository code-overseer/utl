// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

template <bool, typename = void>
struct __UTL_PUBLIC_TEMPLATE enable_if {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE enable_if<true, T> {
    using type UTL_NODEBUG = T;
};

template <bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_enable_if 1
