// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_bidirectional_iterator.h"
#include "utl/iterator/utl_iter_difference_t.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iterator_concept_t.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/iterator/utl_sized_sentinel_for.h"
#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"
#  include "utl/concepts/utl_same_as.h"
#  include "utl/concepts/utl_totally_ordered.h"

UTL_NAMESPACE_BEGIN
template <typename T>
concept random_access_iterator = UTL_SCOPE bidirectional_iterator<T> &&
    UTL_SCOPE details::iterator_concept::implements<T, UTL_SCOPE random_access_iterator_tag> &&
    UTL_SCOPE totally_ordered<T> && UTL_SCOPE sized_sentinel_for<T, T> &&
    requires(T t, T const u, UTL_SCOPE iter_difference_t<T> const n) {
        { t += n } -> UTL_SCOPE same_as<T&>;
        { u + n } -> UTL_SCOPE same_as<T>;
        { n + u } -> UTL_SCOPE same_as<T>;
        { t -= n } -> UTL_SCOPE same_as<T&>;
        { u - n } -> UTL_SCOPE same_as<T>;
        { u[n] } -> UTL_SCOPE same_as<UTL_SCOPE iter_reference_t<T>>;
    };

template <typename T>
struct is_random_access_iterator : UTL_SCOPE bool_constant<random_access_iterator<T>> {};

template <typename T>
inline constexpr bool is_random_access_iterator_v = random_access_iterator<T>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_is_base_of.h"
#  include "utl/type_traits/utl_is_same.h"
#  include "utl/type_traits/utl_is_totally_ordered.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace random_access_iterator {

template <typename T>
auto indexible(int) noexcept -> UTL_SCOPE
    conjunction<UTL_SCOPE is_same<decltype(UTL_SCOPE declval<T&>() +=
                                      UTL_SCOPE declval<UTL_SCOPE iter_difference_t<T>>()),
                    T&>,
        UTL_SCOPE is_same<decltype(UTL_SCOPE declval<T&>() -=
                              UTL_SCOPE declval<UTL_SCOPE iter_difference_t<T>>()),
            T&>,
        UTL_SCOPE is_same<decltype(UTL_SCOPE declval<T const&>() +
                              UTL_SCOPE declval<UTL_SCOPE iter_difference_t<T>>()),
            T>,
        UTL_SCOPE is_same<decltype(UTL_SCOPE declval<UTL_SCOPE iter_difference_t<T>>() +
                              UTL_SCOPE declval<T const&>()),
            T>,
        UTL_SCOPE is_same<decltype(UTL_SCOPE declval<T const&>() -
                              UTL_SCOPE declval<UTL_SCOPE iter_difference_t<T>>()),
            T>,
        UTL_SCOPE is_same<decltype(UTL_SCOPE declval<
                              T const&>()[UTL_SCOPE declval<UTL_SCOPE iter_difference_t<T>>()]),
            UTL_SCOPE iter_reference_t<T>>>;

template <typename T>
auto indexible(float) noexcept -> UTL_SCOPE false_type;

template <typename T>
using is_indexible = decltype(UTL_SCOPE details::random_access_iterator::indexible<T>(0));

} // namespace random_access_iterator
} // namespace details

template <typename T>
struct is_random_access_iterator :
    UTL_SCOPE conjunction<UTL_SCOPE is_bidirectional_iterator<T>,
        UTL_SCOPE details::iterator_concept::implements<UTL_SCOPE random_access_iterator_tag, T>,
        UTL_SCOPE is_totally_ordered<T>, UTL_SCOPE is_sized_sentinel_for<T, T>,
        UTL_SCOPE details::random_access_iterator::is_indexible<T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_random_access_iterator_v = is_random_access_iterator<T>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20
