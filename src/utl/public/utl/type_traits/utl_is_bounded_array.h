// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX20)

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_bounded_array;

using std::is_bounded_array_v;

UTL_NAMESPACE_END

#else // defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX20)

#  include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_bounded_array : false_type {};
template <typename T, size_t N>
struct is_bounded_array<T[N]> : true_type {};

#  ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
#  endif // UTL_CXX14

UTL_NAMESPACE_END

#endif // defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX20)

#define UTL_TRAIT_SUPPORTED_is_bounded_array 1
