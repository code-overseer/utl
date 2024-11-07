// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_bidirectional_iterator.h"
#include "utl/iterator/utl_iter_difference_t.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iterator_concept_t.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/iterator/utl_sized_sentinel_for.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"
#  include "utl/concepts/utl_same_as.h"
#  include "utl/concepts/utl_totally_ordered.h"

UTL_NAMESPACE_BEGIN
template <typename T>
concept random_access_iterator = bidirectional_iterator<T> &&
    details::iterator_concept::implements<T, random_access_iterator_tag> && totally_ordered<T> &&
    sized_sentinel_for<T, T> && requires(T t, T const u, iter_difference_t<T> const n) {
        { t += n } -> same_as<T&>;
        { u + n } -> same_as<T>;
        { n + u } -> same_as<T>;
        { t -= n } -> same_as<T&>;
        { u - n } -> same_as<T>;
        { u[n] } -> same_as<iter_reference_t<T>>;
    };

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_random_access_iterator :
    bool_constant<random_access_iterator<T>> {};

template <typename T>
inline constexpr bool is_random_access_iterator_v = random_access_iterator<T>;
#  define UTL_TRAIT_is_random_access_iterator(...) __UTL random_access_iterator<__VA_ARGS__>

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
__UTL_HIDE_FROM_ABI auto indexible(int) noexcept -> __UTL conjunction<
    __UTL is_same<decltype(__UTL declval<T&>() += __UTL declval<__UTL iter_difference_t<T>>()), T&>,
    __UTL is_same<decltype(__UTL declval<T&>() -= __UTL declval<__UTL iter_difference_t<T>>()), T&>,
    __UTL is_same<decltype(__UTL declval<T const&>() + __UTL declval<__UTL iter_difference_t<T>>()),
        T>,
    __UTL is_same<decltype(__UTL declval<__UTL iter_difference_t<T>>() + __UTL declval<T const&>()),
        T>,
    __UTL is_same<decltype(__UTL declval<T const&>() - __UTL declval<__UTL iter_difference_t<T>>()),
        T>,
    __UTL is_same<decltype(__UTL declval<T const&>()[__UTL declval<__UTL iter_difference_t<T>>()]),
        __UTL iter_reference_t<T>>>;

template <typename T>
__UTL_HIDE_FROM_ABI auto indexible(float) noexcept -> __UTL false_type;

template <typename T>
using is_indexible UTL_NODEBUG = decltype(__UTL details::random_access_iterator::indexible<T>(0));

} // namespace random_access_iterator
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_random_access_iterator :
    conjunction<is_bidirectional_iterator<T>,
        details::iterator_concept::implements<random_access_iterator_tag, T>, is_totally_ordered<T>,
        is_sized_sentinel_for<T, T>, details::random_access_iterator::is_indexible<T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_random_access_iterator_v = is_random_access_iterator<T>::value;
#    define UTL_TRAIT_is_random_access_iterator(...) __UTL is_random_access_iterator_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_random_access_iterator(...) \
        __UTL is_random_access_iterator<__VA_ARGS__>::value
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20
