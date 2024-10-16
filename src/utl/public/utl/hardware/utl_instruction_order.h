// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

enum class instruction_order : int {
    relaxed,
    acquire,
    release,
    acq_rel,
    seq_cst
};

template <instruction_order N>
struct instruction_order_type : value_constant<instruction_order, N> {};

UTL_INLINE_CXX17 constexpr instruction_order_type<instruction_order::relaxed> instruction_order_relaxed{};
UTL_INLINE_CXX17 constexpr instruction_order_type<instruction_order::acquire> instruction_order_acquire{};
UTL_INLINE_CXX17 constexpr instruction_order_type<instruction_order::release> instruction_order_release{};
UTL_INLINE_CXX17 constexpr instruction_order_type<instruction_order::acq_rel> instruction_order_acq_rel{};
UTL_INLINE_CXX17 constexpr instruction_order_type<instruction_order::seq_cst> instruction_order_seq_cst{};

UTL_NAMESPACE_END
