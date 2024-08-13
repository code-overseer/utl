// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace complete_type {

template <typename T, int = sizeof(T)>
__UTL_HIDE_FROM_ABI __UTL true_type is_complete(int) noexcept;

template <typename T>
__UTL_HIDE_FROM_ABI __UTL false_type is_complete(float) noexcept;

} // namespace complete_type
} // namespace details

template <typename T, typename R = decltype(details::complete_type::is_complete<T>(0))>
struct __UTL_PUBLIC_TEMPLATE is_complete : R {};

#if UTL_CXX14
template <typename T, typename R = decltype(details::complete_type::is_complete<T>(0))>
UTL_INLINE_CXX17 constexpr bool is_complete_v = R::value;
#endif // UTL_CXX14

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_complete(...) __UTL is_complete_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_complete(...) __UTL is_complete<__VA_ARGS__>::value
#endif
