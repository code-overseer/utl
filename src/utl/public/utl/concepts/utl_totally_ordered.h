// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_equality_comparable.h"
#  include "utl/concepts/utl_partially_ordered_with.h"
#  include "utl/type_traits/utl_common_reference.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept totally_ordered = equality_comparable<T> && partially_ordered_with<T, T>;

template <typename T, typename U>
concept totally_ordered_with =
    totally_ordered<T> && totally_ordered<U> && equality_comparable_with<T, U> &&
    totally_ordered<
        common_reference_t<remove_reference_t<T> const&, remove_reference_t<U> const&>> &&
    partially_ordered_with<T, U>;

UTL_NAMESPACE_END

#endif
