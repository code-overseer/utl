// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_dereferenceable.h"
#include "utl/iterator/utl_indirectly_readable.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_common_reference.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_const_reference {

template <UTL_CONCEPT_CXX20(UTL_SCOPE indirectly_readable) T UTL_REQUIRES_CXX11(
    UTL_SCOPE is_indirectly_readable<T>::value)>
auto resolve(int) noexcept -> UTL_SCOPE
    common_reference_t<const UTL_SCOPE iter_value_t<T>&&, UTL_SCOPE iter_reference_t<T>>;
} // namespace iter_const_reference
} // namespace details

template <UTL_CONCEPT_CXX20(indirectly_readable) T>
using iter_const_reference_t = decltype(details::iter_const_reference::resolve<T>(0));

UTL_NAMESPACE_END
