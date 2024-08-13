// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_convertible_to.h"
#  include "utl/concepts/utl_same_as.h"
#  include "utl/type_traits/utl_common_reference.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
concept common_reference_with = same_as<common_reference_t<T, U>, common_reference_t<U, T>> &&
    convertible_to<T, common_reference_t<T, U>> && convertible_to<U, common_reference_t<U, T>>;

UTL_NAMESPACE_END

#endif
