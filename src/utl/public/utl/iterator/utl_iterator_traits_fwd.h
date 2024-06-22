// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_base_of.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct iterator_traits;

namespace details {
namespace iterator_traits {
template <typename T>
struct impl_tag {};

#if UTL_CXX20

template <typename T>
concept is_specialized = UTL_TRAIT_is_base_of(impl_tag<T>, UTL_SCOPE iterator_traits<T>);

#else

template <typename T>
using is_specialized =
    bool_constant<UTL_TRAIT_is_base_of(impl_tag<T>, UTL_SCOPE iterator_traits<T>)>;

#endif
} // namespace iterator_traits
} // namespace details

UTL_NAMESPACE_END
