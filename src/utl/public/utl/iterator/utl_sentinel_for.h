// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20
#  include "utl/concepts/utl_equality_comparable.h"
#  include "utl/concepts/utl_semiregular.h"
#  include "utl/iterator/utl_input_or_output_iterator.h"
UTL_NAMESPACE_BEGIN
template <typename S, typename I>
concept sentinel_for =
    semiregular<S> && input_or_output_iterator<I> && details::equality_comparable::weak<S, I>;

template <typename S, typename I>
struct __UTL_PUBLIC_TEMPLATE is_sentinel_for : bool_constant<sentinel_for<S, I>> {};

template <typename S, typename I>
inline constexpr bool is_sentinel_for_v = sentinel_for<S, I>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_is_copyable.h"
#  include "utl/type_traits/utl_is_default_constructible.h"
#  include "utl/type_traits/utl_is_equality_comparable.h"
#  include "utl/type_traits/utl_is_inequality_comparable.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

template <typename S, typename I>
struct __UTL_PUBLIC_TEMPLATE is_sentinel_for :
    conjunction<is_copyable<S>, is_default_constructible<S>, is_input_or_output_iterator<I>,
        is_equality_comparable_with<S, I>, is_inequality_comparable_with<S, I>,
        is_equality_comparable_with<I, S>, is_inequality_comparable_with<I, S>> {};

#  if UTL_CXX14
template <typename S, typename I>
UTL_INLINE_CXX17 constexpr bool is_sentinel_for_v = is_sentinel_for<S, I>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
