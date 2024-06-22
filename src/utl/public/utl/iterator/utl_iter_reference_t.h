// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_type_identity.h"

#if UTL_CXX20

#  include "utl/concepts/utl_dereferenceable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_reference {

template <dereferenceable T>
UTL_SCOPE type_identity<decltype(*UTL_SCOPE declval<T&>())> resolve(int) noexcept;

} // namespace iter_reference
} // namespace details

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_is_dereferenceable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_reference {

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_dereferenceable(T))>
UTL_SCOPE type_identity<decltype(*UTL_SCOPE declval<T&>())> resolve(int) noexcept;

} // namespace iter_reference
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_reference {

struct fallback_t {};
template <typename>
fallback_t resolve(float) noexcept;

template <typename T>
using impl = decltype(resolve<T>(0));

} // namespace iter_reference
} // namespace details

template <typename T>
using iter_reference_t = typename details::iter_reference::impl<T>::type;

UTL_NAMESPACE_END
