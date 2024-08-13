// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_common_reference_with.h"
#  include "utl/concepts/utl_convertible_to.h"
#  include "utl/concepts/utl_same_as.h"
#  include "utl/type_traits/utl_add_lvalue_reference.h"
#  include "utl/type_traits/utl_common_reference.h"
#  include "utl/type_traits/utl_common_type.h"
#  include "utl/type_traits/utl_declval.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
concept common_with = same_as<common_type_t<T, U>, common_type_t<U, T>> &&
    requires {
        static_cast<common_type_t<T, U>>(__UTL declval<T>());
        static_cast<common_type_t<T, U>>(__UTL declval<U>());
    } && common_reference_with<add_lvalue_reference_t<T const>, add_lvalue_reference_t<U const>> &&
    common_reference_with<add_lvalue_reference_t<common_type_t<T, U>>,
        common_reference_t<add_lvalue_reference_t<T const>, add_lvalue_reference_t<U const>>>;

UTL_NAMESPACE_END

#endif
