// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_CXX17

#  define UTL_DEFINE_CUSTOMIZATION_POINT(TYPE, NAME) \
      inline constexpr TYPE NAME {}

#elif UTL_CXX14

UTL_NAMESPACE_BEGIN
namespace details {
namespace customization_point {
template <typename T>
__UTL_ABI_PUBLIC constexpr T constant{};
} // namespace customization_point
} // namespace details

UTL_NAMESPACE_END

#  define UTL_DEFINE_CUSTOMIZATION_POINT(TYPE, NAME)                                       \
      namespace {                                                                          \
      constexpr auto const& NAME = UTL_SCOPE details::customization_point::constant<TYPE>; \
      }                                                                                    \
      static_assert(true, "semi-colon required")

#else
UTL_NAMESPACE_BEGIN

namespace details {
namespace customization_point {
template <typename T>
struct __UTL_PUBLIC_TEMPLATE constant {
    static constexpr T value{};
};

template <typename T>
constexpr T constant<T>::value;
} // namespace customization_point
} // namespace details

UTL_NAMESPACE_END

#  define UTL_DEFINE_CUSTOMIZATION_POINT(TYPE, NAME)                                              \
      namespace {                                                                                 \
      constexpr auto const& NAME = UTL_SCOPE details::customization_point::constant<TYPE>::value; \
      }                                                                                           \
      static_assert(true, "semi-colon required")

#endif
