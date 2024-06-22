// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_iter_move.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iter_rvalue_reference_t.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_remove_cvref.h"

#if UTL_CXX20
#  include "utl/concepts/utl_dereferenceable.h"
#  include "utl/iterator/utl_weakly_incrementable.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept input_or_output_iterator =
    UTL_SCOPE dereferenceable<T> && UTL_SCOPE weakly_incrementable<T>;

template <typename T>
struct is_input_or_output_iterator : UTL_SCOPE bool_constant<input_or_output_iterator<T>> {};

template <typename T>
inline constexpr bool is_input_or_output_iterator_v = input_or_output_iterator<T>;

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_is_referenceable.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace input_or_output_iterator {
template <typename T, typename = void>
struct trait : UTL_SCOPE false_type {};

template <typename T>
struct trait<T,
    UTL_SCOPE
        enable_if_t<UTL_SCOPE is_referenceable<decltype(*static_cast<T (*)()>(0)())>::value>> :
    UTL_SCOPE is_weakly_incrementable<T> {};

} // namespace input_or_output_iterator
} // namespace details

template <typename T>
struct is_input_or_output_iterator : details::input_or_output_iterator::trait<T> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_input_or_output_iterator_v =
    details::input_or_output_iterator::trait<T>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
