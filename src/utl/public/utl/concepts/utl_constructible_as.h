// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_constructible_from.h"
#  include "utl/type_traits/utl_type_identity.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename U, template <typename> class M = type_identity>
concept constructible_as = constructible_from<U, typename M<T>::type>;

UTL_NAMESPACE_END

#endif
