// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::remove_reference;

#  if UTL_CXX14

using std::remove_reference_t;

#  else  // if UTL_CXX14
template <typename T>
using remove_reference_t = typename remove_reference<T>::type;
#  endif // if UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_remove_reference 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(remove_reference)
#    define UTL_BUILTIN_remove_reference(...) __remove_reference(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(remove_reference)

#  ifdef UTL_BUILTIN_remove_reference

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_reference {
    using type UTL_NODEBUG = UTL_BUILTIN_remove_reference(T);
};

template <typename T>
using remove_reference_t = UTL_BUILTIN_remove_reference(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_reference

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_reference {
    using type UTL_NODEBUG = T;
};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_reference<T&> {
    using type UTL_NODEBUG = T;
};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE remove_reference<T&&> {
    using type UTL_NODEBUG = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_reference

#  define UTL_TRAIT_SUPPORTED_remove_reference 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
