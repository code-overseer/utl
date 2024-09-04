// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

namespace platform {
/**
 * Ticks between hardware counters
 * Depending on hardware and platform, ticks may not be accurate
 */
class __UTL_ABI_PUBLIC hardware_ticks {
public:
    static constexpr hardware_ticks invalid() noexcept { return hardware_ticks(-1); }

    constexpr explicit hardware_ticks() noexcept : ticks_(0) {}
    constexpr explicit hardware_ticks(long long t) noexcept : ticks_(t) {}

    __UTL_HIDE_FROM_ABI long long value() const noexcept { return ticks_; }

    /**
     * Whether ticks can be converted to duration
     */
    __UTL_ABI_PUBLIC static bool invariant_frequency() noexcept;

    __UTL_ABI_PUBLIC friend time_duration to_time_duration(hardware_ticks) noexcept;

private:
    long long ticks_;
};
} // namespace platform

UTL_NAMESPACE_END
