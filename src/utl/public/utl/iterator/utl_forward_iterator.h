// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_incrementable.h"
#include "utl/iterator/utl_input_iterator.h"
#include "utl/iterator/utl_iterator_concept_t.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/iterator/utl_sentinel_for.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"

UTL_NAMESPACE_BEGIN
template <typename T>
concept forward_iterator = UTL_SCOPE input_iterator<T> &&
    UTL_SCOPE details::iterator_concept::implements<T, UTL_SCOPE forward_iterator_tag> &&
    UTL_SCOPE incrementable<T> && UTL_SCOPE sentinel_for<T, T>;

template <typename T>
struct is_forward_iterator : UTL_SCOPE bool_constant<forward_iterator<T>> {};

template <typename T>
inline constexpr bool is_forward_iterator_v = forward_iterator<T>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_is_base_of.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_forward_iterator :
    UTL_SCOPE conjunction<UTL_SCOPE is_input_iterator<T>,
        UTL_SCOPE details::iterator_concept::implements<UTL_SCOPE forward_iterator_tag, T>,
        UTL_SCOPE is_incrementable<T>, UTL_SCOPE is_sentinel_for<T, T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_forward_iterator_v = is_forward_iterator<T>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20
