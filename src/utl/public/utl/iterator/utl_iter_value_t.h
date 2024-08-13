// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_iterator_traits_fwd.h"

#include "utl/iterator/utl_indirectly_readable_traits.h"
#include "utl/type_traits/utl_remove_cvref.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_value {
template <__UTL details::iterator_traits::is_specialized T>
__UTL_HIDE_FROM_ABI typename __UTL iterator_traits<T>::value_type resolve(int) noexcept;
} // namespace iter_value
} // namespace details

UTL_NAMESPACE_END

#else // UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_value {

template <typename T UTL_CONSTRAINT_CXX11(__UTL details::iterator_traits::is_specialized<T>::value)>
__UTL_HIDE_FROM_ABI typename __UTL iterator_traits<T>::value_type resolve(int) noexcept;
} // namespace iter_value
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_value {

template <typename T>
__UTL_HIDE_FROM_ABI typename __UTL indirectly_readable_traits<T>::value_type resolve(
    float) noexcept;

} // namespace iter_value
} // namespace details

template <typename T>
using iter_value_t = decltype(details::iter_value::resolve<__UTL remove_cvref_t<T>>(0));

UTL_NAMESPACE_END
