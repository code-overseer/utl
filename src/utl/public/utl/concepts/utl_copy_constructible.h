// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_constructible_from.h"
#  include "utl/concepts/utl_convertible_to.h"
#  include "utl/concepts/utl_move_constructible.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept copy_constructible = move_constructible<T> && constructible_from<T, T&> &&
    convertible_to<T&, T> && constructible_from<T, T const&> && convertible_to<T const&, T> &&
    constructible_from<T, T const> && convertible_to<T const, T>;

UTL_NAMESPACE_END

#endif
