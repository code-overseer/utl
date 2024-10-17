// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

/**
 * Analogue to memory_order; provides a hint to introduce appropriate instruction fences to prevent
 * instruction reorder on hardware with out-of-order execution pipelines. The hint may introduce
 * memory fences depending on hardware and may resolve to no-op on hardware with in-order pipelines.
 */
enum class instruction_order : int {
    /* No constraints on instruction reordering. */
    relaxed,
    /* Prevents later (in-program order) instructions from executing before the affected instruction
     */
    acquire,
    /* Prevents prior (in-program order) instructions from executing after the affected instruction
     */
    release,
    /* Full barrier, prevents instructions from reordering around the affected instruction  */
    acq_rel,
    /* Strongest semantics, full barrier  */
    seq_cst = acq_rel
};

template <instruction_order N>
struct instruction_order_type : value_constant<instruction_order, N> {};

UTL_INLINE_CXX17 constexpr instruction_order_type<instruction_order::relaxed> instruction_order_relaxed{};
UTL_INLINE_CXX17 constexpr instruction_order_type<instruction_order::acquire> instruction_order_acquire{};
UTL_INLINE_CXX17 constexpr instruction_order_type<instruction_order::release> instruction_order_release{};
UTL_INLINE_CXX17 constexpr instruction_order_type<instruction_order::acq_rel> instruction_order_acq_rel{};
UTL_INLINE_CXX17 constexpr instruction_order_type<instruction_order::seq_cst> instruction_order_seq_cst{};

UTL_NAMESPACE_END
