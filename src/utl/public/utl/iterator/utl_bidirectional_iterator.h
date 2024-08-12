// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_forward_iterator.h"
#include "utl/iterator/utl_iterator_concept_t.h"
#include "utl/iterator/utl_iterator_tags.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"
#  include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN
template <typename T>
concept bidirectional_iterator = UTL_SCOPE forward_iterator<T> &&
    UTL_SCOPE details::iterator_concept::implements<T, UTL_SCOPE bidirectional_iterator_tag> &&
    requires(T t) {
        { --t } -> UTL_SCOPE same_as<T&>;
        { t-- } -> UTL_SCOPE same_as<T>;
    };

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_bidirectional_iterator :
    UTL_SCOPE bool_constant<bidirectional_iterator<T>> {};

template <typename T>
inline constexpr bool is_bidirectional_iterator_v = bidirectional_iterator<T>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_is_base_of.h"
#  include "utl/type_traits/utl_is_same.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace bidirectional_iterator {

template <typename T>
__UTL_HIDE_FROM_ABI auto reversible(int) noexcept
    -> UTL_SCOPE conjunction<UTL_SCOPE is_same<decltype(--static_cast<T (*)()>(0)()), T&>,
        UTL_SCOPE is_same<decltype(static_cast<T (*)()>(0)()--), T>>;

template <typename T>
__UTL_HIDE_FROM_ABI auto reversible(float) noexcept -> UTL_SCOPE false_type;

template <typename T>
using is_reversible UTL_NODEBUG = decltype(reversible<T>(0));

} // namespace bidirectional_iterator
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_bidirectional_iterator :
    UTL_SCOPE conjunction<UTL_SCOPE is_forward_iterator<T>,
        UTL_SCOPE details::iterator_concept::implements<UTL_SCOPE bidirectional_iterator_tag, T>,
        UTL_SCOPE details::bidirectional_iterator::is_reversible<T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_bidirectional_iterator_v = is_bidirectional_iterator<T>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20
