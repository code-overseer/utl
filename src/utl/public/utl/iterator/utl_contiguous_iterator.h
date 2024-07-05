// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/iterator/utl_iterator_concept_t.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/iterator/utl_random_access_iterator.h"
#include "utl/memory/utl_to_address.h"
#include "utl/type_traits/utl_add_pointer.h"
#include "utl/type_traits/utl_remove_cvref.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"
#  include "utl/concepts/utl_lvalue_reference.h"
#  include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN
template <typename T>
concept contiguous_iterator = UTL_SCOPE random_access_iterator<T> &&
    UTL_SCOPE details::iterator_concept::implements<T, UTL_SCOPE contiguous_iterator_tag> &&
    UTL_SCOPE lvalue_reference<UTL_SCOPE iter_reference_t<T>> &&
    UTL_SCOPE same_as<UTL_SCOPE iter_value_t<T>,
        UTL_SCOPE remove_cvref_t<UTL_SCOPE iter_reference_t<T>>> &&
    requires(T const& t) {
        {
            UTL_SCOPE to_address(t)
        } -> UTL_SCOPE same_as<UTL_SCOPE add_pointer_t<UTL_SCOPE iter_reference_t<T>>>;
    };

template <typename T>
struct is_contiguous_iterator : UTL_SCOPE bool_constant<contiguous_iterator<T>> {};

template <typename T>
inline constexpr bool is_contiguous_iterator_v = contiguous_iterator<T>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_is_base_of.h"
#  include "utl/type_traits/utl_is_lvalue_reference.h"
#  include "utl/type_traits/utl_is_same.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace contiguous_iterator {

template <typename T>
auto check(int) noexcept -> UTL_SCOPE conjunction<UTL_SCOPE is_random_access_iterator<T>,
    UTL_SCOPE details::iterator_concept::implements<UTL_SCOPE contiguous_iterator_tag, T>,
    UTL_SCOPE is_lvalue_reference<UTL_SCOPE iter_reference_t<T>>,
    UTL_SCOPE
        is_same<UTL_SCOPE iter_value_t<T>, UTL_SCOPE remove_cvref_t<UTL_SCOPE iter_reference_t<T>>>,
    UTL_SCOPE is_same<decltype(UTL_SCOPE to_address(UTL_SCOPE declval<T const&>())),
        UTL_SCOPE add_pointer_t<UTL_SCOPE iter_reference_t<T>>>>;

template <typename T>
auto check(float) noexcept -> UTL_SCOPE false_type;

template <typename T>
using implemented = decltype(UTL_SCOPE details::contiguous_iterator::check<T>(0));

} // namespace contiguous_iterator
} // namespace details

template <typename T>
struct is_contiguous_iterator : UTL_SCOPE details::contiguous_iterator::implemented<T> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<T>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20
