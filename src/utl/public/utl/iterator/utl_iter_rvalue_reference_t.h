// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_iter_move.h"
#include "utl/type_traits/utl_declval.h"

#if UTL_CXX20

#  include "utl/concepts/utl_dereferenceable.h"
#  include "utl/concepts/utl_referenceable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_rvalue_reference {

template <UTL_SCOPE dereferenceable T>
requires requires(T& t) {
    { ranges::iter_move(t) } -> UTL_SCOPE referenceable;
}
auto resolve(int) noexcept -> decltype(ranges::iter_move(UTL_SCOPE declval<T&>()));

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
auto resolve(int) noexcept -> UTL_SCOPE enable_if_t<
    UTL_SCOPE is_referenceable<decltype(ranges::iter_move(UTL_SCOPE declval<T&>()))>::value,
    decltype(ranges::iter_move(UTL_SCOPE declval<T&>()))>;

} // namespace iter_rvalue_reference
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_CXX20

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(dereferenceable) T>
using iter_rvalue_reference_t =
    decltype(details::iter_rvalue_reference::resolve<UTL_SCOPE remove_cvref_t<T>>(0));

UTL_NAMESPACE_END
