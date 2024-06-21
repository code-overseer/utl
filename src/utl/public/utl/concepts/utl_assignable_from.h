// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_common_reference_with.h"
#  include "utl/concepts/utl_same_as.h"
#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_is_lvalue_reference.h"
#  include "utl/type_traits/utl_remove_reference.h"

UTL_NAMESPACE_BEGIN

template <typename L, typename R>
concept assignable_from = UTL_TRAIT_is_lvalue_reference(L) &&
    common_reference_with<remove_reference_t<L> const&, remove_reference_t<R> const&> &&
    requires(L l) {
        { l = declval<R>() } -> same_as<L>;
    };

UTL_NAMESPACE_END

#endif
