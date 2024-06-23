// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_incrementable_traits.h"
#include "utl/iterator/utl_iterator_traits_fwd.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_remove_cvref.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_difference {
template <UTL_SCOPE details::iterator_traits::is_specialized T>
typename UTL_SCOPE iterator_traits<T>::difference_type resolve(int) noexcept;
} // namespace iter_difference
} // namespace details

UTL_NAMESPACE_END

#else // UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_difference {

template <typename T UTL_REQUIRES_CXX11(
    UTL_SCOPE details::iterator_traits::is_specialized<T>::value)>
typename UTL_SCOPE iterator_traits<T>::difference_type resolve(int) noexcept;
} // namespace iter_difference
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_difference {

template <typename T>
typename UTL_SCOPE incrementable_traits<T>::difference_type resolve(float) noexcept;

} // namespace iter_difference
} // namespace details

template <typename T>
using iter_difference_t =
    decltype(details::iter_difference::resolve<UTL_SCOPE remove_cvref_t<T>>(0));

UTL_NAMESPACE_END
