// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_enum.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_underlying_type.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept unscoped_enum = enum_type<T> && UTL_TRAIT_is_convertible(T, underlying_type_t<T>);

UTL_NAMESPACE_END
#endif
