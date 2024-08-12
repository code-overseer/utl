// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_assignable_from.h"
#  include "utl/type_traits/utl_type_identity.h"

UTL_NAMESPACE_BEGIN

template <typename R, typename L, template <typename> class M = type_identity>
concept assignable_to = assignable_from<L, typename M<R>::type>;

UTL_NAMESPACE_END

#endif
