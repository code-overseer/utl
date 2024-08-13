// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_indirectly_readable.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/type_traits/utl_common_reference.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_const_reference {

template <UTL_CONCEPT_CXX20(__UTL indirectly_readable) T UTL_CONSTRAINT_CXX11(
    __UTL is_indirectly_readable<T>::value)>
__UTL_HIDE_FROM_ABI auto resolve(int) noexcept
    -> __UTL common_reference_t<const __UTL iter_value_t<T>&&, __UTL iter_reference_t<T>>;
} // namespace iter_const_reference
} // namespace details

template <UTL_CONCEPT_CXX20(__UTL indirectly_readable) T>
using iter_const_reference_t = decltype(details::iter_const_reference::resolve<T>(0));

UTL_NAMESPACE_END
