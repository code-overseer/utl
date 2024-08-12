// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX17

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::has_unique_object_representations;

using std::has_unique_object_representations_v;

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_has_unique_object_representations 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  if UTL_SHOULD_USE_BUILTIN(has_unique_object_representations)
#    define UTL_BUILTIN_has_unique_object_representations(...) \
        __has_unique_object_representations(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(has_unique_object_representations)

#  ifdef UTL_BUILTIN_has_unique_object_representations

#    include "utl/type_traits/utl_constants.h"
UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE has_unique_object_representations :
    bool_constant<UTL_BUILTIN_has_unique_object_representations(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool has_unique_object_representations_v =
    UTL_BUILTIN_has_unique_object_representations(T);
#    endif

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_has_unique_object_representations 1

#  endif // ifdef UTL_BUILTIN_has_unique_object_representations

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
