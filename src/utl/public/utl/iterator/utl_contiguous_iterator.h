// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

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
concept contiguous_iterator = __UTL random_access_iterator<T> &&
    __UTL details::iterator_concept::implements<T, __UTL contiguous_iterator_tag> &&
    __UTL lvalue_reference<__UTL iter_reference_t<T>> &&
    __UTL same_as<__UTL iter_value_t<T>, __UTL remove_cvref_t<__UTL iter_reference_t<T>>> &&
    requires(T const& t) {
        { __UTL to_address(t) } -> __UTL same_as<__UTL add_pointer_t<__UTL iter_reference_t<T>>>;
    };

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_contiguous_iterator :
    __UTL bool_constant<contiguous_iterator<T>> {};

template <typename T>
inline constexpr bool is_contiguous_iterator_v = contiguous_iterator<T>;
#  define UTL_TRAIT_is_contiguous_iterator(...) __UTL contiguous_iterator<__VA_ARGS__>

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
__UTL_HIDE_FROM_ABI auto check(int) noexcept
    -> __UTL conjunction<__UTL is_random_access_iterator<T>,
        __UTL details::iterator_concept::implements<__UTL contiguous_iterator_tag, T>,
        __UTL is_lvalue_reference<__UTL iter_reference_t<T>>,
        __UTL is_same<__UTL iter_value_t<T>, __UTL remove_cvref_t<__UTL iter_reference_t<T>>>,
        __UTL is_same<decltype(__UTL to_address(__UTL declval<T const&>())),
            __UTL add_pointer_t<__UTL iter_reference_t<T>>>>;

template <typename T>
__UTL_HIDE_FROM_ABI auto check(float) noexcept -> __UTL false_type;

template <typename T>
using implemented UTL_NODEBUG = decltype(__UTL details::contiguous_iterator::check<T>(0));

} // namespace contiguous_iterator
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_contiguous_iterator :
    __UTL details::contiguous_iterator::implemented<T> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<T>::value;
#    define UTL_TRAIT_is_contiguous_iterator(...) __UTL is_contiguous_iterator_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_contiguous_iterator(...) __UTL is_contiguous_iterator<__VA_ARGS__>::value
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20
