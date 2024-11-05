// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_input_iterator.h"
#include "utl/iterator/utl_iter_const_reference_t.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

#  include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

template <typename I>
concept constant_iterator =
    input_iterator<I> && same_as<iter_const_reference_t<I>, iter_reference_t<I>>;
template <typename I>
struct __UTL_PUBLIC_TEMPLATE is_constant_iterator : bool_constant<constant_iterator<I>> {};

template <typename I>
inline constexpr bool is_constant_iterator_v = constant_iterator<I>;

#  define UTL_TRAIT_is_constant_iterator(...) __UTL constant_iterator<__VA_ARGS__>

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_is_same.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

template <typename I>
struct __UTL_PUBLIC_TEMPLATE is_constant_iterator :
    conjunction<is_input_iterator<I>, is_same<iter_const_reference_t<I>, iter_reference_t<I>>> {};

#  if UTL_CXX14
template <typename I>
UTL_INLINE_CXX17 constexpr bool is_constant_iterator_v =
    conjunction_v<is_input_iterator<I>, is_same<iter_const_reference_t<I>, iter_reference_t<I>>>;
#    define UTL_TRAIT_is_constant_iterator(...) __UTL is_constant_iterator_v<__VA_ARGS__>

#  else
#    define UTL_TRAIT_is_constant_iterator(...) __UTL is_constant_iterator<__VA_ARGS__>::value
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20
