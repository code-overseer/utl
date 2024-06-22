// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_indirectly_readable_traits.h"
#include "utl/iterator/utl_iterator_traits_fwd.h"
#include "utl/preprocessor/utl_config.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace iter_value {

template <typename T>
struct impl;
template <typename T>
struct impl<T&> : impl<T> {};
template <typename T>
struct impl<T&&> : impl<T> {};
template <typename T>
struct impl<T const> : impl<T> {};
template <typename T>
struct impl<T volatile> : impl<T> {};
template <typename T>
struct impl<T const volatile> : impl<T> {};

} // namespace iter_value
} // namespace details

template <typename T>
using iter_value_t = typename details::iter_value::impl<T>::type;

UTL_NAMESPACE_END

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_value {
template <typename T>
struct impl {
    using type = typename UTL_SCOPE indirectly_readable_traits<T>::value_type;
};

template <UTL_SCOPE details::iterator_traits::is_specialized T>
struct impl<T> {
    using type = UTL_SCOPE iterator_traits<T>::value_type;
};
} // namespace iter_value
} // namespace details

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iter_value {
template <typename T>
struct impl {
    using type =
        UTL_SCOPE conditional_t<UTL_SCOPE details::iterator_traits::is_specialized<T>::value,
            UTL_SCOPE iterator_traits<T>::value_type,
            typename UTL_SCOPE indirectly_readable_traits<T>::value_type>;
};
} // namespace iter_value
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_CXX20
