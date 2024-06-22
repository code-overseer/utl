// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_assignable_from.h"
#  include "utl/concepts/utl_copy_constructible.h"
#  include "utl/concepts/utl_movable.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept copyable = copy_constructible<T> && movable<T> && assignable_from<T&, T&> &&
    assignable_from<T&, T const&> && assignable_from<T&, T const>;

UTL_NAMESPACE_END

#endif
