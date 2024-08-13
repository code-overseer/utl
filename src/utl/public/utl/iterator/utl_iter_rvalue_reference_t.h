// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_iter_move.h"
#include "utl/type_traits/utl_declval.h"

#if UTL_CXX20

#  include "utl/concepts/utl_dereferenceable.h"
#  include "utl/concepts/utl_referenceable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_rvalue_reference {

template <__UTL dereferenceable T>
requires requires(T& t) {
    { ranges::iter_move(t) } -> __UTL referenceable;
}
__UTL_HIDE_FROM_ABI auto resolve(int) noexcept -> decltype(ranges::iter_move(__UTL declval<T&>()));

} // namespace iter_rvalue_reference
} // namespace details

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_is_dereferenceable.h"
#  include "utl/type_traits/utl_is_referenceable.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_rvalue_reference {

template <UTL_CONCEPT_CXX20(dereferenceable) T UTL_REQUIRES_CXX11(UTL_TRAIT_is_dereferenceable(T))>
__UTL_HIDE_FROM_ABI auto resolve(int) noexcept -> __UTL
    enable_if_t<UTL_TRAIT_is_referenceable(decltype(ranges::iter_move(__UTL declval<T&>()))),
        decltype(ranges::iter_move(__UTL declval<T&>()))>;

} // namespace iter_rvalue_reference
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_CXX20

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(dereferenceable) T>
using iter_rvalue_reference_t =
    decltype(details::iter_rvalue_reference::resolve<__UTL remove_cvref_t<T>>(0));

UTL_NAMESPACE_END
