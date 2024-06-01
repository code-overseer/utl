// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_reference.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept dereferenceable = requires(T& t) {
    { *t } -> reference;
};

UTL_NAMESPACE_END

#endif
