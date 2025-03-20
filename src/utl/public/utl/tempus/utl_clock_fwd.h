// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

namespace tempus {

template <typename>
struct __UTL_PUBLIC_TEMPLATE is_clock;

struct __UTL_ABI_PUBLIC system_clock_t;

struct __UTL_ABI_PUBLIC steady_clock_t;

struct __UTL_ABI_PUBLIC high_resolution_clock_t;

struct __UTL_ABI_PUBLIC process_clock_t;

struct __UTL_ABI_PUBLIC thread_clock_t;

struct __UTL_ABI_PUBLIC hardware_clock_t;

struct __UTL_ABI_PUBLIC file_clock_t;

template <typename>
struct __UTL_PUBLIC_TEMPLATE clock_traits;

template <typename>
class __UTL_PUBLIC_TEMPLATE time_point;

class __UTL_ABI_PUBLIC duration;

class __UTL_ABI_PUBLIC hardware_ticks;

enum class clock_order : signed char;

} // namespace tempus

UTL_NAMESPACE_END
