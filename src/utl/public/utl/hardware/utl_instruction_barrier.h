// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

/**
 * Provides a hint to introduce appropriate instruction fences to prevent
 * instruction reorder on hardware with out-of-order execution pipelines. The hint may introduce
 * memory fences depending on hardware and may resolve to no-op on hardware with in-order pipelines.
 */
enum class instruction_barrier : int {
    /* No constraints on instruction reordering. */
    none,
    /* Ensure execution before later (in-program order) instructions
     */
    after,
    /* Ensure prior (in-program order) instructions complete
     */
    before,
    /* Full barrier, prevents instructions from reordering around the affected instruction  */
    enclose
};

template <instruction_barrier N>
struct instruction_barrier_type : value_constant<instruction_barrier, N> {};

UTL_INLINE_CXX17 constexpr instruction_barrier_type<instruction_barrier::none> instruction_barrier_none{};
UTL_INLINE_CXX17 constexpr instruction_barrier_type<instruction_barrier::after> instruction_barrier_after{};
UTL_INLINE_CXX17 constexpr instruction_barrier_type<instruction_barrier::before> instruction_barrier_before{};
UTL_INLINE_CXX17 constexpr instruction_barrier_type<instruction_barrier::enclose>
    instruction_barrier_enclose{};

UTL_NAMESPACE_END
