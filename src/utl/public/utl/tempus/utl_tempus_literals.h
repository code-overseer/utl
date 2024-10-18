// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/tempus/utl_duration.h"
#include "utl/type_traits/utl_constants.h"

UTL_DISABLE_WARNING_PUSH()
#if UTL_COMPILER_CLANG_BASED | UTL_COMPILER_GNU_BASED
UTL_DISABLE_WARNING("-Wuser-defined-literals")
#elif UTL_COMPILER_MSVC

#endif

UTL_NAMESPACE_BEGIN

inline namespace literals {
inline namespace tempus_literals {

UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr __UTL tempus::duration operator""h(
    unsigned long long hour) noexcept {
    using hour_to_seconds = integral_constant<unsigned long long, 3600>;
    return __UTL tempus::duration{static_cast<int64_t>(hour * hour_to_seconds::value), 0};
}

UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr __UTL tempus::duration operator""min(
    unsigned long long minutes) noexcept {
    using minute_to_seconds = integral_constant<unsigned long long, 60>;
    return __UTL tempus::duration{static_cast<int64_t>(minutes * minute_to_seconds::value), 0};
}

UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr __UTL tempus::duration operator""s(
    unsigned long long seconds) noexcept {
    return __UTL tempus::duration{static_cast<int64_t>(seconds), 0};
}

UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr __UTL tempus::duration operator""ms(
    unsigned long long milliseconds) noexcept {
    using ms_to_ns = integral_constant<unsigned long long, 1000000>;
    return __UTL tempus::duration{0, static_cast<int64_t>(milliseconds * ms_to_ns::value)};
}

UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr __UTL tempus::duration operator""us(
    unsigned long long microseconds) noexcept {
    using us_to_ns = integral_constant<unsigned long long, 1000>;
    return __UTL tempus::duration{0, static_cast<int64_t>(microseconds * us_to_ns::value)};
}

UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr __UTL tempus::duration operator""ns(
    unsigned long long nanoseconds) noexcept {
    return __UTL tempus::duration{0, static_cast<int64_t>(nanoseconds)};
}

} // namespace tempus_literals
} // namespace literals

UTL_NAMESPACE_END

UTL_DISABLE_WARNING_POP()