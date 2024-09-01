// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

namespace platform {

template <typename>
struct is_clock;

struct system_clock_t;

struct steady_clock_t;

struct high_resolution_clock_t;

struct hardware_clock_t;

template <typename>
struct clock_traits;

template <typename>
class time_point;

class time_duration;

class hardware_ticks;

} // namespace platform

UTL_NAMESPACE_END
