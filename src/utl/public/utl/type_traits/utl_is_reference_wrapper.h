// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"

UTL_STD_NAMESPACE_BEGIN
/* UTL_UNDEFINED_BEHAVIOUR */
template <typename>
class reference_wrapper;
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN

template <typename>
class reference_wrapper;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_reference_wrapper : false_type {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_reference_wrapper<reference_wrapper<T>> : true_type {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_reference_wrapper<::std::reference_wrapper<T>> : true_type {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;
#endif

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_reference_wrapper(TYPE) UTL_SCOPE is_reference_wrapper_v<TYPE>
#else
#  define UTL_TRAIT_is_reference_wrapper(TYPE) UTL_SCOPE is_reference_wrapper<TYPE>::value
#endif
