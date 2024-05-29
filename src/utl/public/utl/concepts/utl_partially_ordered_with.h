// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_boolean_testable.h"
#  include "utl/type_traits/utl_remove_reference.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
concept partially_ordered_with =
    requires(remove_reference_t<T> const& t, remove_reference_t<U> const& u) {
        { t < u } -> boolean_testable;
        { t > u } -> boolean_testable;
        { t <= u } -> boolean_testable;
        { t >= u } -> boolean_testable;
        { u < t } -> boolean_testable;
        { u > t } -> boolean_testable;
        { u <= t } -> boolean_testable;
        { u >= t } -> boolean_testable;
    };

UTL_NAMESPACE_END

#endif
