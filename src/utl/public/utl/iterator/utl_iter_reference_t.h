// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_dereferenceable.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_dereferenceable.h"
#include "utl/type_traits/utl_remove_cvref.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_reference {

template <UTL_CONCEPT_CXX20(__UTL dereferenceable) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_dereferenceable(T))>
__UTL_HIDE_FROM_ABI auto resolve(int) noexcept -> decltype(*__UTL declval<T&>());

} // namespace iter_reference
} // namespace details

template <UTL_CONCEPT_CXX20(dereferenceable) T>
using iter_reference_t = decltype(details::iter_reference::resolve<__UTL remove_cvref_t<T>>(0));

UTL_NAMESPACE_END
