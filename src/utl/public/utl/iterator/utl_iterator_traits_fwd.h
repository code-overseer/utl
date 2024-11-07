// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct iterator_traits;

namespace details {
namespace iterator_traits {
template <typename T>
struct impl_tag {};

#if UTL_CXX20

template <typename T>
concept is_specialized = UTL_TRAIT_is_complete(__UTL iterator_traits<T>) &&
    !UTL_TRAIT_is_base_of(impl_tag<T>, __UTL iterator_traits<T>);

#else

template <typename T, typename R = decltype(__UTL details::complete_type::is_complete<T>(0))>
using is_specialized =
    __UTL conjunction<R, __UTL negation<__UTL is_base_of<impl_tag<T>, __UTL iterator_traits<T>>>>;

#endif
} // namespace iterator_traits
} // namespace details

UTL_NAMESPACE_END
