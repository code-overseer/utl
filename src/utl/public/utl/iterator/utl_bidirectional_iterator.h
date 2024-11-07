// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_forward_iterator.h"
#include "utl/iterator/utl_iterator_concept_t.h"
#include "utl/iterator/utl_iterator_tags.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"
#  include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN
template <typename T>
concept bidirectional_iterator = __UTL forward_iterator<T> &&
    __UTL details::iterator_concept::implements<T, __UTL bidirectional_iterator_tag> &&
    requires(T t) {
        { --t } -> __UTL same_as<T&>;
        { t-- } -> __UTL same_as<T>;
    };

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_bidirectional_iterator :
    __UTL bool_constant<bidirectional_iterator<T>> {};

template <typename T>
inline constexpr bool is_bidirectional_iterator_v = bidirectional_iterator<T>;
#  define UTL_TRAIT_is_bidirectional_iterator(...) __UTL bidirectional_iterator<__VA_ARGS__>

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_is_base_of.h"
#  include "utl/type_traits/utl_is_same.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace bidirectional_iterator {

template <typename T>
__UTL_HIDE_FROM_ABI auto reversible(int) noexcept
    -> __UTL conjunction<__UTL is_same<decltype(--__UTL declval<T&>()), T&>,
        __UTL is_same<decltype(__UTL declval<T&>()--), T>>;

template <typename T>
__UTL_HIDE_FROM_ABI auto reversible(float) noexcept -> __UTL false_type;

template <typename T>
using is_reversible UTL_NODEBUG = decltype(reversible<T>(0));

} // namespace bidirectional_iterator
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_bidirectional_iterator :
    __UTL conjunction<__UTL is_forward_iterator<T>,
        __UTL details::iterator_concept::implements<__UTL bidirectional_iterator_tag, T>,
        __UTL details::bidirectional_iterator::is_reversible<T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_bidirectional_iterator_v = is_bidirectional_iterator<T>::value;
#    define UTL_TRAIT_is_bidirectional_iterator(...) __UTL is_bidirectional_iterator_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_bidirectional_iterator(...) \
        __UTL is_bidirectional_iterator<__VA_ARGS__>::value
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20
