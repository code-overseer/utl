// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

namespace tempus {

template <typename>
struct is_clock;

struct system_clock_t;

struct steady_clock_t;

struct high_resolution_clock_t;

struct process_clock_t;

struct thread_clock_t;

struct hardware_clock_t;

template <typename>
struct clock_traits;

template <typename>
class time_point;

class duration;

class hardware_ticks;

enum class clock_order : signed char;

} // namespace tempus

enum class instruction_order : int;

template <instruction_order O>
struct instr_order_t;

UTL_NAMESPACE_END
