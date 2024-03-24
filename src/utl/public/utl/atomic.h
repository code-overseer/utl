// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"

#ifdef UTL_CXX20

#  include <atomic>

UTL_NAMESPACE_BEGIN

using std::atomic;
using std::atomic_signal_fence;
using std::atomic_thread_fence;
using std::memory_order_acquire;
using std::memory_order_relaxed;
using std::memory_order_release;

UTL_NAMESPACE_END

#endif
