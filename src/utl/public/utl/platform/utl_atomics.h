// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/memory/utl_addressof.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_boolean.h"
#include "utl/type_traits/utl_is_pointer.h"
#include "utl/utility/utl_to_underlying.h"
#if UTL_CXX20
#  include "utl/concepts/utl_arithmetic_type.h"
#  include "utl/concepts/utl_enum_type.h"
#else
#  include "utl/type_traits/utl_is_arithmetic.h"
#  include "utl/type_traits/utl_is_enum.h"
#  include "utl/type_traits/utl_logical_traits.h"
#endif

/**
 * Atomic Standardization
 * Current support:
 * * GNU based compilers that rely on the GNU atomic intrinsics
 * * MSVC using the Interlock intrinsics
 *
 * By default only fundamental objects are supported:
 * * Custom atomic types must specialize the platform::custom_atomic_type type
 * trait
 * * If data type is a class with padding bits, it is the user's responsibility
 * to zero them out
 */

#if UTL_COMPILER_GNU_BASED

#  if !defined(__ATOMIC_RELAXED) || !defined(__ATOMIC_CONSUME) || !defined(__ATOMIC_ACQUIRE) || \
      !defined(__ATOMIC_RELEASE) || !defined(__ATOMIC_ACQ_REL) || !defined(__ATOMIC_SEQ_CST)
#    error Undefined atomic semantics
#  endif

UTL_NAMESPACE_BEGIN
namespace platform {
enum class memory_order : int {
    relaxed = __ATOMIC_RELAXED,
    consume = __ATOMIC_CONSUME,
    acquire = __ATOMIC_ACQUIRE,
    release = __ATOMIC_RELEASE,
    acq_rel = __ATOMIC_ACQ_REL,
    seq_cst = __ATOMIC_SEQ_CST,
};
} // namespace platform
UTL_NAMESPACE_END
#else

UTL_NAMESPACE_BEGIN
namespace platform {
enum class memory_order : int {
    relaxed,
    consume,
    acquire,
    release,
    acq_rel,
    seq_cst,
};
} // namespace platform
UTL_NAMESPACE_END

#endif

UTL_NAMESPACE_BEGIN

namespace platform {

template <memory_order>
struct atomic_operations;
using atomic_relaxed = atomic_operations<memory_order::relaxed>;
using atomic_consume = atomic_operations<memory_order::consume>;
using atomic_acquire = atomic_operations<memory_order::acquire>;
using atomic_release = atomic_operations<memory_order::release>;
using atomic_acq_rel = atomic_operations<memory_order::acq_rel>;
using atomic_seq_cst = atomic_operations<memory_order::seq_cst>;

template <typename T>
struct custom_atomic_type : false_type {};

#if UTL_CXX20
template <typename T>
concept native_atomic_type = (arithmetic_type<T> && !boolean_type<T>) || is_pointer_v<T>;
#endif

template <typename T>
struct is_native_atomic_type :
    disjunction<bool_constant<is_arithmetic<T>::value && !is_boolean<T>::value>, is_pointer<T>> {};

/**
 * It's not supported on any compilers anyway
 */
template <typename T>
T kill_dependency(T v) noexcept {
    return v;
}

template <memory_order O>
using memory_order_type = value_constant<memory_order, O>;

template <memory_order O>
struct compare_exchange_failure : memory_order_type<O> {
    constexpr explicit compare_exchange_failure() noexcept = default;
};

using relaxed_on_failed_t = compare_exchange_failure<memory_order::relaxed>;
using acquire_on_failed_t = compare_exchange_failure<memory_order::acquire>;
using seq_cst_on_failed_t = compare_exchange_failure<memory_order::seq_cst>;
UTL_INLINE_CXX17 constexpr relaxed_on_failed_t relaxed_on_failed{};
UTL_INLINE_CXX17 constexpr acquire_on_failed_t acquire_on_failed{};
UTL_INLINE_CXX17 constexpr seq_cst_on_failed_t seq_cst_on_failed{};

namespace atomics {
template <memory_order O>
UTL_CONSTEVAL bool is_load_order() noexcept {
    return O != memory_order::release || O != memory_order::acq_rel;
}

template <memory_order O>
UTL_CONSTEVAL bool is_store_order() noexcept {
    return O != memory_order::consume || O != memory_order::acquire || O != memory_order::acq_rel;
}
} // namespace atomics

} // namespace platform

UTL_NAMESPACE_END

#if UTL_COMPILER_GNU_BASED

#  include "utl/platform/gnu/utl_atomics.h"

#elif UTL_COMPILER_MSVC

#  include "utl/platform/winapi/utl_atomics.h"

extern "C" void* memcpy(void* UTL_RESTRICT, void const* UTL_RESTRICT, size_t);
#  pragma intrinsic(memcpy)

#  include "utl/numeric/utl_sized_floating_point.h"
#  include "utl/numeric/utl_sized_integral.h"

UTL_NAMESPACE_BEGIN

namespace platform {

template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T atomic_load(T const* ctx, memory_order o) noexcept {
    if (o == memory_order::seq_cst) {
        __UTL_MEMORY_BARRIER();
    }

    T const loaded = details::atomics::load(ctx);
    if (o != memory_order::relaxed) {
        __UTL_ACQUIRE_BARRIER();
    }

    return loaded;
}

template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline void atomic_store(
    T* ctx, T value, memory_order o) noexcept {
    if (o >= memory_order::seq_cst) {
        __UTL_HW_MEMORY_BARRIER();
    } else if (o > memory_order::acquire) {
        __UTL_MEMORY_BARRIER();
    }
    details::atomics::store(ctx, value);
    if (o == memory_order::seq_cst) {
        __UTL_STORE_BARRIER();
    }
}

template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T atomic_exchange(
    T* ctx, T value, memory_order o) noexcept {
    // TODO figure out how aggresive the inlining is, if not, maybe better to
    // expose template
    switch (o) {
    case memory_order::relaxed:
        return details::atomics::exchange<memory_order::relaxed>(ctx, value);
    case memory_order::consume:
        return details::atomics::exchange<memory_order::consume>(ctx, value);
    case memory_order::acquire:
        return details::atomics::exchange<memory_order::acquire>(ctx, value);
    case memory_order::release:
        return details::atomics::exchange<memory_order::release>(ctx, value);
    case memory_order::acq_rel:
        return details::atomics::exchange<memory_order::acq_rel>(ctx, value);
    default:
        return details::atomics::exchange<memory_order::seq_cst>(ctx, value);
    };
}

} // namespace platform

UTL_NAMESPACE_END
#  pragma function(memcpy)
#else
#  error Unsupported platform
#endif

UTL_NAMESPACE_BEGIN
namespace platform {

template <>
struct atomic_operations<memory_order::relaxed> :
    atomics::load_operations<memory_order::relaxed>,
    atomics::store_operations<memory_order::relaxed>,
    atomics::exchange_operations<memory_order::relaxed>,
    atomics::fence_operations<memory_order::relaxed>,
    atomics::compare_exchange_operations<memory_order::relaxed, memory_order::relaxed> {};

template <>
struct atomic_operations<memory_order::acquire> :
    atomics::load_operations<memory_order::acquire>,
    atomics::exchange_operations<memory_order::acquire>,
    atomics::fence_operations<memory_order::acquire>,
    atomics::compare_exchange_operations<memory_order::acquire, memory_order::acquire>,
    atomics::compare_exchange_operations<memory_order::acquire, memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::acquire, memory_order::relaxed> {};

template <>
struct atomic_operations<memory_order::consume> :
    atomics::load_operations<memory_order::consume>,
    atomics::exchange_operations<memory_order::consume>,
    atomics::fence_operations<memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::consume, memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::consume, memory_order::relaxed> {};

template <>
struct atomic_operations<memory_order::release> :
    atomics::store_operations<memory_order::release>,
    atomics::exchange_operations<memory_order::release>,
    atomics::fence_operations<memory_order::release>,
    atomics::compare_exchange_operations<memory_order::release, memory_order::acquire>,
    atomics::compare_exchange_operations<memory_order::release, memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::release, memory_order::relaxed> {};

template <>
struct atomic_operations<memory_order::acq_rel> :
    atomics::exchange_operations<memory_order::acq_rel>,
    atomics::fence_operations<memory_order::acq_rel>,
    atomics::compare_exchange_operations<memory_order::acq_rel, memory_order::acquire>,
    atomics::compare_exchange_operations<memory_order::acq_rel, memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::acq_rel, memory_order::relaxed> {};

template <>
struct atomic_operations<memory_order::seq_cst> :
    atomics::load_operations<memory_order::seq_cst>,
    atomics::store_operations<memory_order::seq_cst>,
    atomics::exchange_operations<memory_order::seq_cst>,
    atomics::fence_operations<memory_order::seq_cst>,
    atomics::compare_exchange_operations<memory_order::seq_cst, memory_order::seq_cst>,
    atomics::compare_exchange_operations<memory_order::seq_cst, memory_order::acquire>,
    atomics::compare_exchange_operations<memory_order::seq_cst, memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::seq_cst, memory_order::relaxed> {};

} // namespace platform
UTL_NAMESPACE_END
