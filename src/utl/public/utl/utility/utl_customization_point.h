// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX17

#  define UTL_DEFINE_CUSTOMIZATION_POINT(TYPE, NAME) \
      __UTL_HIDE_FROM_ABI inline constexpr TYPE NAME {}

#elif UTL_CXX14

UTL_NAMESPACE_BEGIN
namespace details {
namespace customization_point {
template <typename T>
__UTL_HIDE_FROM_ABI constexpr T constant{};
} // namespace customization_point
} // namespace details

UTL_NAMESPACE_END

#  define UTL_DEFINE_CUSTOMIZATION_POINT(TYPE, NAME)                                   \
      namespace {                                                                      \
      constexpr auto const& NAME = __UTL details::customization_point::constant<TYPE>; \
      }                                                                                \
      static_assert(true, "semi-colon required")

#else
UTL_NAMESPACE_BEGIN

namespace details {
namespace customization_point {
template <typename T>
struct __UTL_PUBLIC_TEMPLATE constant {
    __UTL_HIDE_FROM_ABI static constexpr T value{};
};

template <typename T>
constexpr T constant<T>::value;
} // namespace customization_point
} // namespace details

UTL_NAMESPACE_END

#  define UTL_DEFINE_CUSTOMIZATION_POINT(TYPE, NAME)                 \
      namespace {                                                    \
      __UTL_HIDE_FROM_ABI constexpr auto const& NAME =               \
          __UTL details::customization_point::constant<TYPE>::value; \
      }                                                              \
      static_assert(true, "semi-colon required")

#endif
