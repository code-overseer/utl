// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_iter_move.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iter_rvalue_reference_t.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_remove_cvref.h"

#if UTL_CXX20
#  include "utl/concepts/utl_dereferenceable.h"
#  include "utl/iterator/utl_weakly_incrementable.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept input_or_output_iterator = __UTL dereferenceable<T> && __UTL weakly_incrementable<T>;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_input_or_output_iterator :
    __UTL bool_constant<input_or_output_iterator<T>> {};

template <typename T>
inline constexpr bool is_input_or_output_iterator_v = input_or_output_iterator<T>;

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_is_referenceable.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace input_or_output_iterator {

template <typename T>
__UTL_HIDE_FROM_ABI auto check(float) noexcept -> __UTL false_type;

template <typename T>
__UTL_HIDE_FROM_ABI auto check(int) noexcept
    -> __UTL conjunction<__UTL is_referenceable<decltype(*static_cast<T (*)()>(0)())>,
        __UTL is_weakly_incrementable<T>>;

template <typename T>
using implemented UTL_NODEBUG = decltype(__UTL details::input_or_output_iterator::check<T>(0));

} // namespace input_or_output_iterator
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_input_or_output_iterator :
    details::input_or_output_iterator::implemented<T> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_input_or_output_iterator_v =
    details::input_or_output_iterator::implemented<T>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
