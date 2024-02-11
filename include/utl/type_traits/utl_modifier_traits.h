// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_modify_x_cv.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include <type_traits>

UTL_NAMESPACE_BEGIN

using ::std::make_signed;
using ::std::make_unsigned;

using ::std::remove_extent;
using ::std::remove_all_extents;

using ::std::decay;

template<typename T> using make_signed_t = typename make_signed<T>::type;
template<typename T> using make_unsigned_t = typename make_unsigned<T>::type;

template<typename T> using remove_extent_t = typename remove_extent<T>::type;
template<typename T> using remove_all_extents_t = typename remove_all_extents<T>::type;

template<typename T> using decay_t = typename decay<T>::type;


UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_make_signed 1
#define UTL_TRAIT_SUPPORTED_make_unsigned 1

#define UTL_TRAIT_SUPPORTED_remove_extent 1
#define UTL_TRAIT_SUPPORTED_remove_all_extents 1

#define UTL_TRAIT_SUPPORTED_decay 1

