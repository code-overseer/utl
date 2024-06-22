// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_indirectly_readable.h"
#include "utl/iterator/utl_iterator_concept.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept input_iterator = UTL_SCOPE input_or_output_iterator<T> &&
    UTL_SCOPE indirectly_readable<T> && requires { typename UTL_SCOPE iterator_concept_t<T>; } &&
    UTL_SCOPE derived_from<UTL_SCOPE iterator_concept_t<T>, UTL_SCOPE input_iterator_tag>;

template <typename T>
struct is_input_iterator : UTL_SCOPE bool_constant<input_iterator<T>> {};

template <typename T>
inline constexpr bool is_input_iterator_v = input_iterator<T>;

UTL_NAMESPACE_END
#else // UTL_CXX20

#  include "utl/type_traits/utl_is_base_of.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace input_iterator {

template <typename T, typename = void>
struct has_concept : UTL_SCOPE false_type {};

template <typename T>
struct has_concept<T, UTL_SCOPE void_t<UTL_SCOPE iterator_concept_t<T>>> :
    UTL_SCOPE is_base_of<UTL_SCOPE input_iterator_tag, UTL_SCOPE iterator_concept_t<T>> {};

} // namespace input_iterator
} // namespace details

template <typename T>
struct is_input_iterator :
    UTL_SCOPE conjunction<UTL_SCOPE is_input_or_output_iterator<T>,
        UTL_SCOPE is_indirectly_readable<T>, UTL_SCOPE details::input_iterator::has_concept<T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_input_iterator_v = is_input_iterator<T>::value;
#  endif

UTL_NAMESPACE_END

#endif
