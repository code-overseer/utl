// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

namespace platform {
/**
 * Ticks between hardware counters
 * Depending on hardware and platform, ticks may not be accurate
 */
class hardware_ticks {
public:
    constexpr explicit hardware_ticks() noexcept : ticks_(0) {}
    constexpr explicit hardware_ticks(long long t) noexcept : ticks_(t) {}

    long long value() const noexcept { return ticks_; }

private:
    long long ticks_;
};
} // namespace platform

UTL_NAMESPACE_END
