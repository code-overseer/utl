// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_assignable_from.h"
#  include "utl/concepts/utl_move_constructible.h"
#  include "utl/concepts/utl_swappable.h"
#  include "utl/type_traits/utl_is_object.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept movable = is_object_v<T> && move_constructible<T> && assignable_from<T&, T> && swappable<T>;

UTL_NAMESPACE_END

#endif
