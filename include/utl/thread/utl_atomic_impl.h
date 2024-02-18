// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"

enum class memory_order
{
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst,
    memory_order_consume = memory_order_acquire
};

UTL_INLINE_CXX17 memory_order memory_order_acquire = memory_order::memory_order_acquire;
UTL_INLINE_CXX17 memory_order memory_order_release = memory_order::memory_order_release;
UTL_INLINE_CXX17 memory_order memory_order_acq_rel = memory_order::memory_order_acq_rel;
UTL_INLINE_CXX17 memory_order memory_order_seq_cst = memory_order::memory_order_seq_cst;
UTL_INLINE_CXX17 memory_order memory_order_consume = memory_order::memory_order_consume;

#define UTL_ATOMIC_LOAD(MEM, ORDER)
#define UTL_ATOMIC_STORE(MEM, VAL, ORDER)




