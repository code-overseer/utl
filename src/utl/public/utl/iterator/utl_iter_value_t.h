// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_indirectly_readable_traits.h"
#include "utl/iterator/utl_iterator_traits_fwd.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_remove_cvref.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_value {
template <UTL_SCOPE details::iterator_traits::is_specialized T>
UTL_SCOPE iterator_traits<T>::value_type resolve(int) noexcept;
} // namespace iter_value
} // namespace details

UTL_NAMESPACE_END

#else // UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_value {

template <typename T UTL_REQUIRES_CXX11(
    UTL_SCOPE details::iterator_traits::is_specialized<T>::value)>
UTL_SCOPE iterator_traits<T>::value_type resolve(int) noexcept;
} // namespace iter_value
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_value {

template <typename T>
typename UTL_SCOPE indirectly_readable_traits<T>::value_type resolve(float) noexcept;

} // namespace iter_value
} // namespace details

template <typename T>
using iter_value_t = decltype(details::iter_value::resolve<UTL_SCOPE remove_cvref_t<T>>(0));

UTL_NAMESPACE_END
