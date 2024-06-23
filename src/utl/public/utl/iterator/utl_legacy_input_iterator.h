// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_legacy_iterator.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

#  include "utl/concepts/utl_copyable.h"
#  include "utl/concepts/utl_equality_comparable.h"
#  include "utl/concepts/utl_referenceable.h"
#  include "utl/iterator/utl_input_iterator.h"

UTL_NAMESPACE_BEGIN

template <typename It>
concept legacy_input_iterator = UTL_SCOPE legacy_iterator<It> && UTL_SCOPE input_iterator<It> &&
    UTL_SCOPE equality_comparable<It>;

template <typename It>
struct is_legacy_input_iterator : UTL_SCOPE bool_constant<legacy_input_iterator<It>> {};

template <typename It>
inline constexpr bool is_legacy_input_iterator_v = UTL_SCOPE legacy_input_iterator<It>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_is_equality_comparable.h"
#  include "utl/type_traits/utl_is_inequality_comparable.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

template <typename It>
struct is_legacy_input_iterator :
    UTL_SCOPE conjunction<UTL_SCOPE is_legacy_iterator<It>, UTL_SCOPE is_equality_comparable<It>,
        UTL_SCOPE is_inequality_comparable<It>> {};

#  if UTL_CXX14
template <typename It>
UTL_INLINE_CXX17 constexpr bool is_legacy_input_iterator_v = UTL_SCOPE is_legacy_input_iterator<It>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20

#if UTL_CXX14
#  define UTL_TRAIT_is_legacy_input_iterator(...) UTL_SCOPE is_legacy_input_iterator_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_legacy_input_iterator(...) \
      UTL_SCOPE is_legacy_input_iterator<__VA_ARGS__>::value
#endif
