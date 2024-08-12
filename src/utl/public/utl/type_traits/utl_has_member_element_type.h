// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN
template <typename T>
struct __UTL_PUBLIC_TEMPLATE has_member_element_type : UTL_SCOPE false_type {};

template <typename T>
requires requires { typename T::element_type; }
struct __UTL_PUBLIC_TEMPLATE has_member_element_type<T> : UTL_SCOPE true_type {};

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_void_t.h"
UTL_NAMESPACE_BEGIN

namespace type_traits {
namespace details {
template <typename T, typename = void>
struct has_member_element_type_impl : UTL_SCOPE false_type {};

template <typename T>
struct has_member_element_type_impl<T, void_t<typename T::element_type>> : UTL_SCOPE true_type {};
} // namespace details
} // namespace type_traits

template <typename T>
struct __UTL_PUBLIC_TEMPLATE has_member_element_type :
    type_traits::details::has_member_element_type_impl<T> {};

UTL_NAMESPACE_END

#endif

UTL_NAMESPACE_BEGIN

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool has_member_element_type_v = has_member_element_type<T>::value;
#endif // UTL_CXX14

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_has_member_element_type(...) UTL_SCOPE has_member_element_type_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_has_member_element_type(...) \
      UTL_SCOPE has_member_element_type<__VA_ARGS__>::value
#endif
