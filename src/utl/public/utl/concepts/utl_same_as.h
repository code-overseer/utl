// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_is_same.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

namespace details {
namespace concepts {
template <typename T, typename U>
concept same = UTL_TRAIT_is_same(T, U);
}
} // namespace details

template <typename T, typename U>
concept same_as = details::concepts::same<T, U> && details::concepts::same<U, T>;

UTL_NAMESPACE_END
#endif
