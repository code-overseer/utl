// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_legacy_bidirectional_iterator.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

#  include "utl/concepts/utl_lvalue_reference.h"
#  include "utl/iterator/utl_random_access_iterator.h"

UTL_NAMESPACE_BEGIN

template <typename It>
concept legacy_random_access_iterator =
    UTL_SCOPE legacy_bidirectional_iterator<It> && UTL_SCOPE random_access_iterator<It>;

template <typename It>
struct is_legacy_random_access_iterator :
    UTL_SCOPE bool_constant<legacy_random_access_iterator<It>> {};

template <typename It>
inline constexpr bool is_legacy_random_access_iterator_v =
    UTL_SCOPE legacy_random_access_iterator<It>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/iterator/utl_indirectly_readable_traits.h"
#  include "utl/iterator/utl_random_access_iterator.h"
#  include "utl/type_traits/utl_is_totally_ordered.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace legacy_random_access_iterator {

template <typename It>
auto check(float) -> UTL_SCOPE false_type;

template <typename It>
auto check(int) -> UTL_SCOPE conjunction<UTL_SCOPE is_legacy_bidirectional_iterator<It>,
    UTL_SCOPE is_totally_ordered<It>, UTL_SCOPE details::random_access_iterator::is_indexible<It>>;

template <typename It>
using implemented = decltype(UTL_SCOPE details::legacy_random_access_iterator::check<It>(0));

} // namespace legacy_random_access_iterator
} // namespace details

template <typename It>
struct is_legacy_random_access_iterator :
    UTL_SCOPE details::legacy_random_access_iterator::implemented<It> {};

#  if UTL_CXX14
template <typename It>
UTL_INLINE_CXX17 constexpr bool is_legacy_random_access_iterator_v =
    UTL_SCOPE is_legacy_random_access_iterator<It>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20

#if UTL_CXX14
#  define UTL_TRAIT_is_legacy_random_access_iterator(...) \
      UTL_SCOPE is_legacy_random_access_iterator_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_legacy_random_access_iterator(...) \
      UTL_SCOPE is_legacy_random_access_iterator<__VA_ARGS__>::value
#endif
