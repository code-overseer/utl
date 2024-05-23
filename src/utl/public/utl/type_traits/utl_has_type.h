// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/type_traits/utl_void_t.h"

UTL_NAMESPACE_BEGIN

namespace type_traits {
namespace details {
template <typename T, typename = void>
struct has_type_impl : false_type {};

template <typename T>
struct has_type_impl<T, void_t<typename T::type>> : true_type {};
} // namespace details
} // namespace type_traits

template <typename T>
struct has_type : type_traits::details::has_type_impl<T> {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool has_type_v = has_type<T>::value;
#endif // UTL_CXX14

UTL_NAMESPACE_END
