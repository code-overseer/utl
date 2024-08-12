// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_standard.h"

#include "utl/configuration/utl_namespace.h"
#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

namespace type_traits {
namespace details {
using size_t = decltype(sizeof(0));

template <typename T, size_t = sizeof(T)>
UTL_HIDE_FROM_ABI true_type is_complete(int);

template <typename T>
UTL_HIDE_FROM_ABI false_type is_complete(float);

} // namespace details
} // namespace type_traits

template <typename T, typename R = decltype(type_traits::details::is_complete<T>(0))>
struct __UTL_PUBLIC_TEMPLATE is_complete : R {};

#if UTL_CXX14
template <typename T, typename R = decltype(type_traits::details::is_complete<T>(0))>
UTL_INLINE_CXX17 constexpr bool is_complete_v = R::value;
#endif // UTL_CXX14

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_complete(...) UTL_SCOPE is_complete_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_complete(...) UTL_SCOPE is_complete<__VA_ARGS__>::value
#endif
