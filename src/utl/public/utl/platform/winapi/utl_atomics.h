// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#include "utl/concept/utl_implicit_lifetime.h"
#include "utl/numeric/utl_sized_integral.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_make_unsigned.h"

#if !UTL_COMPILER_MSVC
#  error Invalid compiler
#endif

UTL_NAMESPACE_BEGIN

namespace platform {
namespace atomics {
template <memory_order O>
struct fence_operations;
template <memory_order O>
struct fence_operations {
protected:
    UTL_CONSTEXPR_CXX20 ~fence_operations() noexcept = default;

public:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence() noexcept {
        __UTL_MEMORY_BARRIER();
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void signal_fence() noexcept {
        __UTL_COMPILER_BARRIER();
    }
};

template <>
struct fence_operations<memory_order::relaxed> {
protected:
    UTL_CONSTEXPR_CXX20 ~fence_operations() noexcept = default;

public:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence() noexcept {}
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void signal_fence() noexcept {}
};

template <>
struct fence_operations<memory_order::acquire> {
protected:
    UTL_CONSTEXPR_CXX20 ~fence_operations() noexcept = default;

public:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence() noexcept {
        __UTL_ACQUIRE_BARRIER();
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void signal_fence() noexcept {
        __UTL_COMPILER_BARRIER();
    }
};

template <>
struct fence_operations<memory_order::consume> : fence_operations<memory_order::acquire> {};

namespace details {
template <memory_order O>
struct load_impl;

#ifdef UTL_BUILTIN_launder
#  define __UTL_launder(T, ptr) (UTL_BUILTIN_launder(reinterpret_cast<T*>(ptr)))
#else
#  define __UTL_launder(T, ptr) (reinterpret_cast<T*>(ptr));
#endif

template <>
struct load_impl<memory_order::relaxed> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__iso_volatile_load8((__int8 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__iso_volatile_load16((__int16 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__iso_volatile_load32((__int32 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__iso_volatile_load64((__int64 const volatile*)ctx);
    }
};

template <>
struct load_impl<memory_order::acquire> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__ldar8((__int8 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__ldar16((__int16 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__ldar32((__int32 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__ldar64((__int64 const volatile*)ctx);
    }
};

template <>
struct load_impl<memory_order::consume> : load_impl<memory_order::acquire> {};

template <>
struct load_impl<memory_order::seq_cst> : load_impl<memory_order::seq_cst> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__iso_volatile_load8((__int8 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__iso_volatile_load16((__int16 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__iso_volatile_load32((__int32 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const volatile* ctx) noexcept {
        return (T)__iso_volatile_load64((__int64 const volatile*)ctx);
    }
};
} // namespace details

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
        return (T)load((underlying_type_t<T> const*)ctx);
    }

    template <UTL_CONCEPT_CXX20(boolean_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_boolean(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T const* ctx) noexcept {
        return (T)load((unsigned char const*)ctx);
    }
};

template <memory_order O>
struct store_operations {
protected:
    static_assert(is_store_order<O>(), "Invalid order");
    static constexpr int order = (int)O;
    UTL_CONSTEXPR_CXX20 ~store_operations() noexcept = default;

public:
    template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(T* ctx, T value) noexcept {
        __atomic_store(ctx, __UTL addressof(value), order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(T* ctx, T value) noexcept {
        store((underlying_type_t<T>*)ctx, (underlying_type_t<T>)value, order);
    }
};

template <memory_order O>
struct exchange_operations {
protected:
    static constexpr int order = (int)O;
    UTL_CONSTEXPR_CXX20 ~exchange_operations() noexcept = default;

public:
    template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(T* ctx, T value) noexcept {
        return __atomic_exchange_n(ctx, __UTL addressof(val), order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(T* ctx, T value) noexcept {
        return (T)exchange((underlying_type_t<T>*)ctx, (underlying_type_t<T>)value);
    }

    template <integral T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(T* ctx, T delta) noexcept {
        return __atomic_fetch_add(ctx, delta, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(T* ctx, T delta) noexcept {
        return (T)fetch_add((underlying_type_t<T>*)ctx, (underlying_type_t<T>)delta);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(T* ctx, ptrdiff_t offset) noexcept {
        return __atomic_fetch_add(ctx, offset * sizeof(T), order);
    }

    template <integral T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(T* ctx, T delta) noexcept {
        return __atomic_fetch_sub(ctx, delta, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(T* ctx, T delta) noexcept {
        return (T)fetch_sub((underlying_type_t<T>*)ctx, (underlying_type_t<T>)delta);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(T* ctx, ptrdiff_t offset) noexcept {
        return __atomic_fetch_sub(ctx, offset * sizeof(T), order);
    }

    template <integral T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(T* ctx, T mask) noexcept {
        return __atomic_fetch_and(ctx, mask, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(T* ctx, T mask) noexcept {
        return (T)fetch_and((underlying_type_t<T>*)ctx, (underlying_type_t<T>)mask);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(T* ctx, intptr_t mask) noexcept {
        return __atomic_fetch_and((intptr_t*)ctx, mask, order);
    }

    template <integral T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(T* ctx, T mask) noexcept {
        return __atomic_fetch_or(ctx, mask, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(T* ctx, T mask) noexcept {
        return (T)fetch_or((underlying_type_t<T>*)ctx, (underlying_type_t<T>)mask);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(T* ctx, intptr_t mask) noexcept {
        return __atomic_fetch_or((intptr_t*)ctx, mask, order);
    }

    template <integral T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(T* ctx, T mask) noexcept {
        return __atomic_fetch_xor(ctx, mask, order);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(T* ctx, T mask) noexcept {
        return (T)fetch_xor((underlying_type_t<T>*)ctx, (underlying_type_t<T>)mask);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(T* ctx, intptr_t mask) noexcept {
        return __atomic_fetch_xor((intptr_t*)ctx, mask, order);
    }
};

template <memory_order S, memory_order F>
struct compare_exchange_operations {
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
        T* ctx, T* expected, T value) noexcept {
        return __atomic_compare_exchange_n(ctx, expected, value, strong, success, fail);
    }

    template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak(
        T* ctx, T* expected, T value) noexcept {
        return __atomic_compare_exchange_n(ctx, expected, value, weak, success, fail);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_strong(
        T* ctx, T* expected, T value) noexcept {
        return __atomic_compare_exchange_n(ctx, expected, value, strong, success, fail);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak(
        T* ctx, T* expected, T value) noexcept {
        return __atomic_compare_exchange_n(ctx, expected, value, weak, success, fail);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_strong(
        T* ctx, T* expected, T value) noexcept {
        return compare_exchange_strong((underlying_type_t<T>*)ctx, (underlying_type_t<T>*)expected,
            (underlying_type_t<T>)value);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak(
        T* ctx, T* expected, T value) noexcept {
        return compare_exchange_weak((underlying_type_t<T>*)ctx, (underlying_type_t<T>*)expected,
            (underlying_type_t<T>)value);
    }

public:
    template <typename T>
    UTL_CONSTRAINT_CXX20(requires(T* ctx, T* expected, T value) {
        compare_exchange_strong(ctx, expected, value);
    })
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline auto compare_exchange_strong(
        T* ctx, T* expected, T value, compare_exchange_failure_order<F>)
        -> decltype(compare_exchange_strong(ctx, expected, value)) {
        return compare_exchange_strong(ctx, expected, value);
    }

    template <typename T>
    UTL_CONSTRAINT_CXX20(requires(T* ctx, T* expected, T value) {
        compare_exchange_weak(ctx, expected, value);
    })
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline auto compare_exchange_weak(
        T* ctx, T* expected, T value, compare_exchange_failure_order<F>)
        -> decltype(compare_exchange_weak(ctx, expected, value)) {
        return compare_exchange_weak(ctx, expected, value);
    }
};

} // namespace atomics
} // namespace platform

UTL_NAMESPACE_END

UTL_NAMESPACE_BEGIN

namespace platform {
namespace details {
namespace atomics {

template <typename T>
T load(T const volatile* ctx) noexcept = delete;
template <typename T>
void store(T volatile* ctx, T value) noexcept = delete;

#if UTL_CXX20
template <typename T, size_t N>
concept atomic_accessible = implicit_lifetime<T> && !sized_integral<T, N> && sizeof(T) == N;
#elif UTL_CXX14
template <typename T, size_t N>
UTL_INLINE_CXX17 constexpr bool is_atomic_accessible_v =
    UTL_TRAIT_is_implicit_lifetime(T) && !is_sized_integral_v<T, N> && sizeof(T) == N;
#  define UTL_TRAIT_is_atomic_accessible(T, N) \
      __UTL platform::details::atomics::is_atomic_accessible_v<T, N>
#else
template <typename T, size_t N>
using is_atomic_accessible = bool_constant<UTL_TRAIT_is_implicit_lifetime(T) &&
    is_sized_integral<T, N>::value && sizeof(T) == N>;
#  define UTL_TRAIT_is_atomic_accessible(T, N) \
      __UTL platform::details::atomics::is_atomic_accessible<T, N>::value
#endif

#ifdef UTL_BUILTIN_launder
#  define __UTL_launder(T, ptr) (UTL_BUILTIN_launder(reinterpret_cast<T*>(ptr)))
#else
#  define __UTL_launder(T, ptr) (reinterpret_cast<T*>(ptr));
#endif

template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_integral(1, T))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T load(T const volatile* ctx) noexcept {
    return (T)__iso_volatile_load8((__int8 const volatile*)ctx);
}

template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_integral(2, T))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T load(T const volatile* ctx) noexcept {
    return (T)__iso_volatile_load16((__int16 const volatile*)ctx);
}

template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_integral(4, T))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T load(T const volatile* ctx) noexcept {
    return (T)__iso_volatile_load32((__int32 const volatile*)ctx);
}

template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_integral(8, T))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T load(T const volatile* ctx) noexcept {
    return (T)__iso_volatile_load64((__int64 const volatile*)ctx);
}

template <UTL_CONCEPT_CXX20(atomic_accessible<1>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_atomic_accessible(T, 1))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T load(T const volatile* ctx) noexcept {
    auto const f = load((__int8 const volatile*)(ctx));
    alignas(T) unsigned char out[sizeof(T)];
    auto ptr = ::memcpy(&out, &f, sizeof(T));
    return *__UTL_launder(T, ptr);
}

template <UTL_CONCEPT_CXX20(atomic_accessible<2>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_atomic_accessible(T, 2))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T load(T const volatile* ctx) noexcept {
    auto const f = load((__int16 const volatile*)(ctx));
    alignas(T) unsigned char out[sizeof(T)];
    auto ptr = ::memcpy(&out, &f, sizeof(T));
    return *__UTL_launder(T, ptr);
}

template <UTL_CONCEPT_CXX20(atomic_accessible<4>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_atomic_accessible(T, 4))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T load(T const volatile* ctx) noexcept {
    auto const f = load((__int32 const volatile*)(ctx));
    alignas(T) unsigned char out[sizeof(T)];
    auto ptr = ::memcpy(&out, &f, sizeof(T));
    return *__UTL_launder(T, ptr);
}

template <UTL_CONCEPT_CXX20(atomic_accessible<8>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_atomic_accessible(T, 8))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T load(T const volatile* ctx) noexcept {
    auto const f = load((__int64 const volatile*)(ctx));
    alignas(T) unsigned char out[sizeof(T)];
    auto ptr = ::memcpy(&out, &f, sizeof(T));
    return *__UTL_launder(T, ptr);
}

template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline void store(T volatile* ctx, T value) noexcept {
    __iso_volatile_store16((__int16 volatile*)ctx, value);
}

template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline void store(T volatile* ctx, T value) noexcept {
    __iso_volatile_store16((__int16 volatile*)ctx, value);
}

template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline void store(T volatile* ctx, T value) noexcept {
    __iso_volatile_store32((__int32 volatile*)ctx, value);
}

template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline void store(T volatile* ctx, T value) noexcept {
    __iso_volatile_store64((__int64 volatile*)ctx, value);
}

template <UTL_CONCEPT_CXX20(atomic_accessible<1>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_atomic_accessible(T, 1))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline void store(T volatile* ctx, T value) noexcept {
    __int8 src;
    ::memcpy(&src, value, sizeof(T));
    __iso_volatile_store8((__int8 volatile*)ctx, src);
}

template <UTL_CONCEPT_CXX20(atomic_accessible<2>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_atomic_accessible(T, 2))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline void store(T volatile* ctx, T value) noexcept {
    __int16 src;
    ::memcpy(&src, value, sizeof(T));
    __iso_volatile_store16((__int16 volatile*)ctx, src);
}

template <UTL_CONCEPT_CXX20(atomic_accessible<4>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_atomic_accessible(T, 4))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline void store(T volatile* ctx, T value) noexcept {
    __int32 src;
    ::memcpy(&src, value, sizeof(T));
    __iso_volatile_store32((__int32 volatile*)ctx, src);
}

template <UTL_CONCEPT_CXX20(atomic_accessible<8>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_atomic_accessible(T, 8))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline void store(T volatile* ctx, T value) noexcept {
    __int64 src;
    ::memcpy(&src, value, sizeof(T));
    __iso_volatile_store64((__int64 volatile*)ctx, src);
}

template <memory_order, typename T>
T exchange(T volatile* ctx, T value) noexcept = delete;

template <memory_order>
struct exchange_t;

#if UTL_ARCH_ARM
template <>
struct exchange_t<atomic_relaxed> {

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange8_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange16_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange32_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange64_nf(ctx, value);
    }
};

template <>
struct exchange_t<atomic_acquire> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange8_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange16_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange32_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange64_acq(ctx, value);
    }
};

template <>
struct exchange_t<atomic_consume> : exchange_t<atomic_acquire> {};

template <>
struct exchange_t<atomic_release> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange8_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange16_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange32_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange64_rel(ctx, value);
    }
};

#endif

template <memory_order>
struct exchange_t {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange8(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange16(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange32(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchange64(ctx, value);
    }
};

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto exchange(T volatile* ctx, T value) noexcept
    -> UTL_ENABLE_IF_CXX11(
    T, sizeof(exchange_t<o>{}(ctx, value))) {
    static constexpr exchange_t<o> impl;
    return impl(ctx, value);
}

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto exchange(T* volatile* ctx, T* value) noexcept -> T* {
    return reinterpret_cast<T*>(_InterlockedExchangePointer(ctx, value));
}

template <memory_order o, UTL_CONCEPT_CXX20(atomic_accessible<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_atomic_accessible(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T exchange(T volatile* ctx, T value) const noexcept {
    __int8 tmp;
    auto ptr = ::memcpy(&tmp, &value, sizeof(T));
    tmp = exchange<o>(reinterpret_cast<__int8 volatile*>(ctx), tmp);
    ::memcpy(&value, &tmp, sizeof(T));
    return value;
}

template <memory_order o, UTL_CONCEPT_CXX20(atomic_accessible<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_atomic_accessible(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T exchange(T volatile* ctx, T value) const noexcept {
    __int16 tmp;
    auto ptr = ::memcpy(&tmp, &value, sizeof(T));
    tmp = exchange<o>(reinterpret_cast<__int16 volatile*>(ctx), tmp);
    ::memcpy(&value, &tmp, sizeof(T));
    return value;
}

template <memory_order o, UTL_CONCEPT_CXX20(atomic_accessible<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_atomic_accessible(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T exchange(T volatile* ctx, T value) const noexcept {
    __int32 tmp;
    auto ptr = ::memcpy(&tmp, &value, sizeof(T));
    tmp = exchange<o>(reinterpret_cast<__int32 volatile*>(ctx), tmp);
    ::memcpy(&value, &tmp, sizeof(T));
    return value;
}

template <memory_order o, UTL_CONCEPT_CXX20(atomic_accessible<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_atomic_accessible(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T exchange(T volatile* ctx, T value) const noexcept {
    __int64 tmp;
    auto ptr = ::memcpy(&tmp, &value, sizeof(T));
    tmp = exchange<o>(reinterpret_cast<__int64 volatile*>(ctx), tmp);
    ::memcpy(&value, &tmp, sizeof(T));
    return value;
}

template <memory_order>
struct fetch_add_t;

#if UTL_ARCH_ARM
template <>
struct fetch_add_t<atomic_relaxed> {

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd8_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd16_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd32_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd64_nf(ctx, value);
    }
};

template <>
struct fetch_add_t<atomic_acquire> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd8_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd16_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd32_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd64_acq(ctx, value);
    }
};

template <>
struct fetch_add_t<atomic_consume> : fetch_add_t<atomic_acquire> {};

template <>
struct fetch_add_t<atomic_release> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd8_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd16_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd32_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd64_rel(ctx, value);
    }
};

#endif

template <memory_order>
struct fetch_add_t {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd8(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd16(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd32(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedExchangeAdd64(ctx, value);
    }
};

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto fetch_add(T volatile* ctx, T value) noexcept
    -> UTL_ENABLE_IF_CXX11(
    T, sizeof(fetch_add_t<o>{}(ctx, value))) {
    static constexpr fetch_add_t<o> impl;
    return impl(ctx, value);
}

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto fetch_add(T* volatile* ctx, intptr_t value) noexcept
    -> T* {
    return reinterpret_cast<T*>(
        fetch_add(reinterpret_cast<intptr_t volatile*>(ctx), value * sizeof(T)));
}

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto fetch_sub(T volatile* ctx, T value) noexcept
    -> UTL_ENABLE_IF_CXX11(
    T, sizeof(fetch_add_t<o>{}(ctx, value))) {
    static constexpr fetch_add_t<o> impl;
    return impl(ctx, static_cast<T>(0U - static_cast<make_unsigned_t<T>>(value)));
}

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto fetch_sub(T* volatile* ctx, intptr_t value) noexcept
    -> T* {
    return reinterpret_cast<T*>(fetch_add(reinterpret_cast<intptr_t volatile*>(ctx), -value));
}

template <memory_order>
struct fetch_and_t;

#if UTL_ARCH_ARM
template <>
struct fetch_and_t<atomic_relaxed> {

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd8_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd16_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd32_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd64_nf(ctx, value);
    }
};

template <>
struct fetch_and_t<atomic_acquire> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd8_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd16_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd32_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd64_acq(ctx, value);
    }
};

template <>
struct fetch_and_t<atomic_consume> : fetch_and_t<atomic_acquire> {};

template <>
struct fetch_and_t<atomic_release> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd8_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd16_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd32_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd64_rel(ctx, value);
    }
};

#endif

template <memory_order>
struct fetch_and_t {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd8(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd16(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd32(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedAnd64(ctx, value);
    }
};

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto fetch_and(T volatile* ctx, T value) noexcept
    -> UTL_ENABLE_IF_CXX11(
    T, sizeof(fetch_and_t<o>{}(ctx, value))) {
    static constexpr fetch_and_t<o> impl;
    return impl(ctx, value);
}

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto fetch_and(T* volatile* ctx, intptr_t value) noexcept
    -> T* {
    return reinterpret_cast<T*>(fetch_and(reinterpret_cast<intptr_t volatile*>(ctx), value));
}

template <memory_order>
struct fetch_or_t;

#if UTL_ARCH_ARM
template <>
struct fetch_or_t<atomic_relaxed> {

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr8_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr16_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr32_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr64_nf(ctx, value);
    }
};

template <>
struct fetch_or_t<atomic_acquire> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr8_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr16_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr32_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr64_acq(ctx, value);
    }
};

template <>
struct fetch_or_t<atomic_consume> : fetch_or_t<atomic_acquire> {};

template <>
struct fetch_or_t<atomic_release> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr8_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr16_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr32_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr64_rel(ctx, value);
    }
};

#endif

template <memory_order>
struct fetch_or_t {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr8(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr16(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr32(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr64(ctx, value);
    }
};

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto fetch_or(T volatile* ctx, T value) noexcept
    -> UTL_ENABLE_IF_CXX11(
    T, sizeof(fetch_or_t<o>{}(ctx, value))) {
    static constexpr fetch_or_t<o> impl;
    return impl(ctx, value);
}

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto fetch_or(T* volatile* ctx, intptr_t value) noexcept
    -> T* {
    return reinterpret_cast<T*>(fetch_or(reinterpret_cast<intptr_t volatile*>(ctx), value));
}

template <memory_order>
struct fetch_xor_t;

#if UTL_ARCH_ARM
template <>
struct fetch_xor_t<atomic_relaxed> {

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedXor8_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr16_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr32_nf(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr64_nf(ctx, value);
    }
};

template <>
struct fetch_xor_t<atomic_acquire> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedXor8_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr16_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr32_acq(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr64_acq(ctx, value);
    }
};

template <>
struct fetch_xor_t<atomic_consume> : fetch_xor_t<atomic_acquire> {};

template <>
struct fetch_xor_t<atomic_release> {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedXor8_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr16_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr32_rel(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr64_rel(ctx, value);
    }
};

#endif

template <memory_order>
struct fetch_xor_t {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedXor8(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr16(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr32(ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline T operator()(T volatile* ctx, T value) const noexcept {
        return _InterlockedOr64(ctx, value);
    }
};

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto fetch_xor(T volatile* ctx, T value) noexcept
    -> UTL_ENABLE_IF_CXX11(
    T, sizeof(fetch_xor_t<o>{}(ctx, value))) {
    static constexpr fetch_xor_t<o> impl;
    return impl(ctx, value);
}

template <memory_order o, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto fetch_xor(T* volatile* ctx, intptr_t value) noexcept
    -> T* {
    return reinterpret_cast<T*>(fetch_xor(reinterpret_cast<intptr_t volatile*>(ctx), value));
}

template <memory_order, memory_order>
struct compare_exchange_t;

#if UTL_ARCH_ARM
template <memory_order s, memory_order f>
struct compare_exchange_t {
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool operator()(
        T volatile* ctx, T* expected, T desired) const noexcept {
        auto prev = (T)(_InterlockedCompareExchange8(
            (char volatile*)ctx, (char)desired, *((char*)expected)));
        if (*expected != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool operator()(
        T volatile* ctx, T* expected, T desired) const noexcept {
        auto prev = (T)(_InterlockedCompareExchange16(
            (short volatile*)ctx, (short)desired, *((short*)expected)));
        if (*expected != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool operator()(
        T volatile* ctx, T* expected, T desired) const noexcept {
        auto prev = (T)(_InterlockedCompareExchange32(
            (long volatile*)ctx, (long)desired, *((long*)expected)));
        if (*expected != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool operator()(
        T volatile* ctx, T* expected, T desired) const noexcept {
        auto prev = (T)(_InterlockedCompareExchange64(
            (__int64 volatile*)ctx, (__int64)desired, *((__int64*)expected)));
        if (*expected != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool operator()(
        T volatile* ctx, T* expected, T desired) const noexcept {
        alignas(16) __int64 buf[2];
        ::memcpy(buf, &desired, 16);
        return _InterlockedCompareExchange128(
            (__int64 volatile*)ctx, buf[1], buf[0], (__int64*)expected);
    }
};
#endif

template <memory_order s, memory_order f>
struct compare_exchange_t {

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool operator()(
        T volatile* ctx, T* expected, T desired) const noexcept {
        auto prev = (T)(_InterlockedCompareExchange8(
            (__int8 volatile*)ctx, (__int8)desired, *((__int8*)expected)));
        if (*expected != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool operator()(
        T volatile* ctx, T* expected, T desired) const noexcept {
        auto prev = (T)(_InterlockedCompareExchange8(
            (__int8 volatile*)ctx, (__int8)desired, *((__int8*)expected)));
        if (*expected != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool operator()(
        T volatile* ctx, T* expected, T desired) const noexcept {
        auto prev = (T)(_InterlockedCompareExchange8(
            (__int8 volatile*)ctx, (__int8)desired, *((__int8*)expected)));
        if (*expected != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool operator()(
        T volatile* ctx, T* expected, T desired) const noexcept {
        auto prev = (T)(_InterlockedCompareExchange64(
            (__int64 volatile*)ctx, (__int64)desired, *((__int64*)expected)));
        if (*expected != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool operator()(
        T volatile* ctx, T* expected, T desired) const noexcept {
        alignas(16) __int64 buf[2];
        ::memcpy(buf, &desired, 16);
        return _InterlockedCompareExchange128(
            (__int64 volatile*)ctx, buf[1], buf[0], (__int64*)expected);
    }
};

template <memory_order s, memory_order f, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline bool compare_exchange_strong(
    T* volatile* ctx, T** expected, T* desired) noexcept {
    auto prev = reinterpret_cast<T*>(
        _InterlockedCompareExchangePointer((void* volatile*)ctx, desired, *expected));
    if (*expected != prev) {
        *expected = prev;
        return false;
    }

    return true;
}

template <memory_order s, memory_order f, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline auto compare_exchange_weak(
    T* volatile* ctx, T** expected, T* desired) noexcept -> T* {
    return compare_exchange_strong<s, f>(ctx, expected, desired);
}

} // namespace atomics
} // namespace details
} // namespace platform
#undef __UTL_launder
#undef UTL_TRAIT_is_atomic_accessible

UTL_NAMESPACE_END
