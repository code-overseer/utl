// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_indirectly_readable.h"
#include "utl/iterator/utl_input_or_output_iterator.h"
#include "utl/iterator/utl_iterator_concept_t.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept input_iterator = __UTL input_or_output_iterator<T> && __UTL indirectly_readable<T> &&
    __UTL details::iterator_concept::implements<T, __UTL input_iterator_tag>;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_input_iterator : __UTL bool_constant<input_iterator<T>> {};

template <typename T>
inline constexpr bool is_input_iterator_v = input_iterator<T>;

UTL_NAMESPACE_END
#else // UTL_CXX20

#  include "utl/type_traits/utl_is_base_of.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_input_iterator :
    __UTL conjunction<__UTL is_input_or_output_iterator<T>, __UTL is_indirectly_readable<T>,
        __UTL details::iterator_concept::implements<__UTL input_iterator_tag, T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_input_iterator_v = is_input_iterator<T>::value;
#  endif

UTL_NAMESPACE_END

#endif
