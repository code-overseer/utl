// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/type_traits/utl_is_convertible.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To>
concept convertible_to =
    UTL_TRAIT_is_convertible(From, To) && requires(From f) { static_cast<To>(f); };

UTL_NAMESPACE_END

#endif
