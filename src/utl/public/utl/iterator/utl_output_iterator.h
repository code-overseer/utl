// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_indirectly_writable.h"
#include "utl/iterator/utl_input_or_output_iterator.h"
#include "utl/utility/utl_forward.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename It, typename ValueType>
concept output_iterator = input_or_output_iterator<It> && indirectly_writable<It, ValueType> &&
    requires(It it, ValueType&& v) { *it++ = UTL_SCOPE forward<ValueType>(v); };

template <typename It, typename ValueType>
struct UTL_PUBLIC_TEMPLATE is_output_iterator : bool_constant<output_iterator<It, ValueType>> {};

template <typename It, typename ValueType>
inline constexpr bool is_output_iterator_v = output_iterator<It, ValueType>;

UTL_NAMESPACE_END
#else

#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_is_assignable.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace output_iterator {
template <typename Out, typename T>
UTL_HIDE_FROM_ABI auto check(float) -> UTL_SCOPE false_type;

template <typename Out, typename T>
UTL_HIDE_FROM_ABI auto check(int)
    -> UTL_SCOPE conjunction<UTL_SCOPE is_input_or_output_iterator<Out>,
        UTL_SCOPE is_indirectly_writable<Out, T>,
        UTL_SCOPE is_assignable<decltype(*UTL_SCOPE declval<Out&>()++), T>>;

template <typename Out, typename T>
using implemented UTL_NODEBUG = decltype(UTL_SCOPE details::output_iterator::check<Out, T>(0));

} // namespace output_iterator
} // namespace details

template <typename OutIt, typename ValueType>
struct UTL_PUBLIC_TEMPLATE is_output_iterator :
    UTL_SCOPE details::output_iterator::implemented<OutIt, ValueType> {};

#  if UTL_CXX14
template <typename OutIt, typename ValueType>
UTL_INLINE_CXX17 constexpr bool is_output_iterator_v = UTL_SCOPE is_output_iterator<OutIt, ValueType>::value;
#  endif

UTL_NAMESPACE_END

#endif
