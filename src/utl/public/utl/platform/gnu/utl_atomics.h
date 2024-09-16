// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#include "utl/numeric/utl_sized_integral.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_copy_cv.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_make_unsigned.h"
#include "utl/type_traits/utl_underlying_type.h"

#include <cstdint>

#if !UTL_COMPILER_GNU_BASED
#  error Invalid compiler
#endif

#include "utl/platform/gnu/utl_memory_barriers.h"

UTL_NAMESPACE_BEGIN

namespace platform {
namespace atomics {
template <memory_order O>
struct fence_operations {
protected:
    static constexpr int order = (int)O;
    UTL_CONSTEXPR_CXX20 ~fence_operations() noexcept = default;

public:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence() noexcept {
        __atomic_thread_fence(order);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void signal_fence() noexcept {
        __atomic_signal_fence(order);
    }
};

template <memory_order O>
struct load_operations {
protected:
    static_assert(is_load_order<O>(), "Invalid order");
    static constexpr int order = (int)O;
    UTL_CONSTEXPR_CXX20 ~load_operations() noexcept = default;

public:
    template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const* ctx) noexcept {
        return __atomic_load_n(ctx, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const* ctx) noexcept {
        using type = copy_cv_t<T const, underlying_type_t<T>>;
        return (T)load((type*)ctx);
    }
};

template <memory_order O>
struct store_operations {
private:
    template <typename T>
    using value_type = remove_cv_t<T>;

protected:
    static_assert(is_store_order<O>(), "Invalid order");
    static constexpr int order = (int)O;
    UTL_CONSTEXPR_CXX20 ~store_operations() noexcept = default;

public:
    template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value) noexcept {
        __atomic_store(ctx, __UTL addressof(value), order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value) noexcept {
        using type = copy_cv_t<T, underlying_type_t<T>>;
        store((type*)ctx, (underlying_type_t<T>)value, order);
    }
};

template <memory_order O>
struct exchange_operations {
private:
    template <typename T>
    using value_type = remove_cv_t<T>;

protected:
    static constexpr int order = (int)O;
    UTL_CONSTEXPR_CXX20 ~exchange_operations() noexcept = default;

public:
    template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value) noexcept {
        return __atomic_exchange_n(ctx, val, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value) noexcept {
        using type = copy_cv_t<T, underlying_type_t<T>>;
        return (T)exchange((type*)ctx, (underlying_type_t<T>)value);
    }

    template <integral T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> delta) noexcept {
        return __atomic_fetch_add(ctx, delta, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, underlying_type_t<T> delta) noexcept {
        using type = copy_cv_t<T, underlying_type_t<T>>;
        return (T)fetch_add((type*)ctx, (underlying_type_t<T>)delta);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(T* ctx, ptrdiff_t offset) noexcept {
        return __atomic_fetch_add(ctx, offset * sizeof(T), order);
    }

    template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(
        T* ctx, value_type<T> delta) noexcept {
        return __atomic_fetch_sub(ctx, delta, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(
        T* ctx, underlying_type_t<T> delta) noexcept {
        using type = copy_cv_t<T, underlying_type_t<T>>;
        return (T)fetch_sub((type*)ctx, delta);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(T* ctx, ptrdiff_t offset) noexcept {
        return __atomic_fetch_sub(ctx, offset * sizeof(T), order);
    }

    template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(T* ctx, T mask) noexcept {
        return __atomic_fetch_and(ctx, mask, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, underlying_type_t<T> mask) noexcept {
        using type = copy_cv_t<T, underlying_type_t<T>>;
        return (T)fetch_and((type*)ctx, mask);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(T* ctx, uintptr_t mask) noexcept {
        return __atomic_fetch_and(ctx, mask, order);
    }

    template <integral T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(T* ctx, T mask) noexcept {
        return __atomic_fetch_or(ctx, mask, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, underlying_type_t<T> mask) noexcept {
        using type = copy_cv_t<T, underlying_type_t<T>>;
        return (T)fetch_or((type*)ctx, (underlying_type_t<T>)mask);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(T* ctx, uintptr_t mask) noexcept {
        return __atomic_fetch_or(ctx, mask, order);
    }

    template <integral T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(T* ctx, T mask) noexcept {
        return __atomic_fetch_xor(ctx, mask, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, underlying_type_t<T> mask) noexcept {
        using type = copy_cv_t<T, underlying_type_t<T>>;
        return (T)fetch_xor((type*)ctx, mask);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(T* ctx, uintptr_t mask) noexcept {
        return __atomic_fetch_xor(ctx, mask, order);
    }
};

template <memory_order S, memory_order F>
struct compare_exchange_operations {
private:
    template <typename T>
    using value_type = remove_cv_t<T>;
    template <typename T>
    using pointer = value_type<T>;

protected:
    static_assert(is_load_order<F>(), "Invalid order");
    static constexpr int success = (int)S;
    static constexpr int fail = (int)F;
    static constexpr bool strong = false;
    static constexpr bool weak = true;
    UTL_CONSTEXPR_CXX20 ~compare_exchange_operations() noexcept = default;

private:
    template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_strong(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        return __atomic_compare_exchange_n(ctx, expected, value, strong, success, fail);
    }

    template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        return __atomic_compare_exchange_n(ctx, expected, value, weak, success, fail);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_strong(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        return __atomic_compare_exchange_n(ctx, expected, value, strong, success, fail);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        return __atomic_compare_exchange_n(ctx, expected, value, weak, success, fail);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_strong(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        using type = copy_cv_t<T, underlying_type_t<T>>;
        return compare_exchange_strong(
            (type*)ctx, (underlying_type_t<T>*)expected, (underlying_type_t<T>)value);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        using type = copy_cv_t<T, underlying_type_t<T>>;
        return compare_exchange_weak(
            (type*)ctx, (underlying_type_t<T>*)expected, (underlying_type_t<T>)value);
    }

public:
    template <typename T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline auto compare_exchange_strong(
        T* ctx, pointer<T> expected, value_type<T> value, compare_exchange_failure<F>)
        -> decltype(compare_exchange_strong(ctx, expected, value)) {
        return compare_exchange_strong(ctx, expected, value);
    }

    template <typename T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline auto compare_exchange_weak(
        T* ctx, pointer<T> expected, value_type<T> value, compare_exchange_failure<F>)
        -> decltype(compare_exchange_weak(ctx, expected, value)) {
        return compare_exchange_weak(ctx, expected, value);
    }
};

} // namespace atomics
} // namespace platform

UTL_NAMESPACE_END
