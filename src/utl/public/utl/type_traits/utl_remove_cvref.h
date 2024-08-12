// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::remove_cvref;

using std::remove_cvref_t;

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  if UTL_SHOULD_USE_BUILTIN(remove_cvref)
#    define UTL_BUILTIN_remove_cvref(...) __remove_cvref(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(remove_cvref)

#  ifdef UTL_BUILTIN_remove_cvref

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_cvref {
    using type UTL_NODEBUG = UTL_BUILTIN_remove_cvref(T);
};

template <typename T>
using remove_cvref_t = UTL_BUILTIN_remove_cvref(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_cvref

#    include "utl/type_traits/utl_remove_cv.h"
#    include "utl/type_traits/utl_remove_reference.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_cvref : remove_cv<remove_reference_t<T>> {};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_cvref

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_remove_cvref 1
