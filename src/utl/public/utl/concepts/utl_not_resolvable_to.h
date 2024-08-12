// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_convertible_to.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/type_traits/utl_remove_cvref.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
concept not_resolvable_to = !same_as<U, remove_cvref_t<T>> && !convertible_to<T, U>;

UTL_NAMESPACE_END

#endif
