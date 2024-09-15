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
struct fence_type {
private:
    using relaxed_type = memory_order_type<memory_order::relaxed>;
    using acquire_type = memory_order_type<memory_order::acquire>;
    using consume_type = memory_order_type<memory_order::consume>;
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence(relaxed_type) noexcept {}
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void signal_fence(relaxed_type) noexcept {}
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence(acquire_type) noexcept {
        __UTL_ACQUIRE_BARRIER();
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence(consume_type) noexcept {
        __UTL_ACQUIRE_BARRIER();
    }
    template <memory_order O>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence(
        memory_order_type<O>) noexcept {
        __UTL_MEMORY_BARRIER();
    }
    template <memory_order O>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void signal_fence(
        memory_order_type<O>) noexcept {
        __UTL_COMPILER_BARRIER();
    }

public:
    template <memory_order O>
    struct operations {
    protected:
        static constexpr memory_order_type<O> this_order{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence() noexcept {
            thread_fence(this_order);
        }
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void signal_fence() noexcept {
            signal_fence(this_order);
        }
    };
};

struct load_type {
private:
    using relaxed_type = memory_order_type<memory_order::relaxed>;
    using seq_cst_type = memory_order_type<memory_order::seq_cst>;

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T* ctx, relaxed_type) noexcept {
        return __iso_volatile_load8((__int8 volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T* ctx, relaxed_type) noexcept {
        return __iso_volatile_load16((__int16 volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T* ctx, relaxed_type) noexcept {
        return __iso_volatile_load32((__int32 volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T* ctx, relaxed_type) noexcept {
        return __iso_volatile_load64((__int64 volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T* ctx, seq_cst_type) noexcept {
        return (T)__ldar8((unsigned __int8 volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T* ctx, seq_cst_type) noexcept {
        return (T)__ldar16((unsigned __int16 volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T* ctx, seq_cst_type) noexcept {
        return (T)__ldar32((unsigned __int32 volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T* ctx, seq_cst_type) noexcept {
        return (T)__ldar64((unsigned __int64 volatile*)ctx);
    }
#ifdef UTL_ENABLE_RCPC_LOAD
#  define __UTL_LOAD_ACQ(WIDTH, TARGET) \
      __ldapr##WIDTH((unsigned __int##WIDTH const volatile*)TARGET)
#else
#  define __UTL_LOAD_ACQ(WIDTH, TARGET) __ldar##WIDTH((unsigned __int##WIDTH const volatile*)TARGET)
#endif
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(
        T* ctx, memory_order_type<O>) noexcept {
        return (T)__UTL_LOAD_ACQ(8, ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(
        T* ctx, memory_order_type<O>) noexcept {
        return (T)__UTL_LOAD_ACQ(16, ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(
        T* ctx, memory_order_type<O>) noexcept {
        return (T)__UTL_LOAD_ACQ(32, ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(
        T* ctx, memory_order_type<O>) noexcept {
        return (T)__UTL_LOAD_ACQ(64, ctx);
    }
#undef __UTL_LOAD_ACQ
public:
    template <memory_order O>
    struct operations {
    protected:
        static_assert(is_load_order<O>(), "Invalid order");
        static constexpr memory_order_type<O> this_order{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T* ctx) noexcept {
            load(ctx, value, this_order);
        }

        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T* ctx) noexcept {
            load((underlying_type_t<T>*)ctx);
        }

        template <UTL_CONCEPT_CXX20(boolean_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_boolean(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T load(T* ctx) noexcept {
            load(ctx, this_order);
        }
    };
};

struct store_type {
private:
    using relaxed_type = value_constant<memory_order, memory_order::relaxed>;
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, T value, relaxed_type) noexcept {
        __iso_volatile_store8((__int8 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, T value, relaxed_type) noexcept {
        __iso_volatile_store16((__int16 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, T value, relaxed_type) noexcept {
        __iso_volatile_store32((__int32 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, T value, relaxed_type) noexcept {
        __iso_volatile_store64((__int64 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, T value, value_constant<memory_order, O>) noexcept {
        __stlr8((unsigned __int8 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, T value, value_constant<memory_order, O>) noexcept {
        __stlr16((unsigned __int16 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, T value, value_constant<memory_order, O>) noexcept {
        __stlr32((unsigned __int32 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, T value, value_constant<memory_order, O>) noexcept {
        __stlr64((unsigned __int64 volatile*)ctx, value);
    }

public:
    template <memory_order O>
    struct operations {
    protected:
        static_assert(is_store_order<O>(), "Invalid order");
        static constexpr value_constant<memory_order, O> this_order{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(T* ctx, T value) noexcept {
            store(ctx, value, this_order);
        }

        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(T* ctx, T value) noexcept {
            store((underlying_type_t<T>*)ctx, (underlying_type_t<T>)value);
        }

        template <UTL_CONCEPT_CXX20(boolean_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_boolean(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(T* ctx, T value) noexcept {
            store(ctx, value, this_order);
        }
    };
};

template <memory_order O>
using fence_operations = fence_type::operations<O>;
template <memory_order O>
using load_operations = load_type::operations<O>;
template <memory_order O>
using store_operations = store_type::operations<O>;

struct exchange_type {
private:
    using relaxed_type = memory_order_type<memory_order::relaxed>;
    using acquire_type = memory_order_type<memory_order::acquire>;
    using release_type = memory_order_type<memory_order::release>;
    using consume_type = memory_order_type<memory_order::consume>;
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, relaxed_type) noexcept {
        return (T)_InterlockedExchange8_nf((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, relaxed_type) noexcept {
        return (T)_InterlockedExchange16_nf((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, relaxed_type) noexcept {
        return (T)_InterlockedExchange32_nf((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, relaxed_type) noexcept {
        return (T)_InterlockedExchange64_nf((__int64 volatile*)ctx, value);
    }
    template <typename T, memory_order O>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* volatile*, T value, relaxed_type) noexcept {
        return (T*)_InterlockedExchangePointer_nf((void* volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, acquire_type) noexcept {
        return (T)_InterlockedExchange8_acq((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, acquire_type) noexcept {
        return (T)_InterlockedExchange16_acq((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, acquire_type) noexcept {
        return (T)_InterlockedExchange32_acq((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, acquire_type) noexcept {
        return (T)_InterlockedExchange64_acq((__int64 volatile*)ctx, value);
    }
    template <typename T, memory_order O>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* volatile*, T value, acquire_type) noexcept {
        return (T*)_InterlockedExchangePointer_acq((void* volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, release_type) noexcept {
        return (T)_InterlockedExchange8_rel((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, release_type) noexcept {
        return (T)_InterlockedExchange16_rel((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, release_type) noexcept {
        return (T)_InterlockedExchange32_rel((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, release_type) noexcept {
        return (T)_InterlockedExchange64_rel((__int64 volatile*)ctx, value);
    }
    template <typename T, memory_order O>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* volatile*, T value, release_type) noexcept {
        return (T*)_InterlockedExchangePointer_rel((void* volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedExchange8((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedExchange16((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedExchange32((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedExchange64((__int64 volatile*)ctx, value);
    }
    template <typename T, memory_order O>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* volatile*, T* value, memory_order_type<O>) noexcept {
        return (T*)_InterlockedExchangePointer((void* volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, relaxed_type) noexcept {
        return (T)_InterlockedExchangeAdd8_nf((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, relaxed_type) noexcept {
        return (T)_InterlockedExchangeAdd16_nf((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, relaxed_type) noexcept {
        return (T)_InterlockedExchangeAdd32_nf((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, relaxed_type) noexcept {
        return (T)_InterlockedExchangeAdd64_nf((__int64 volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, acquire_type) noexcept {
        return (T)_InterlockedExchangeAdd8_acq((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, acquire_type) noexcept {
        return (T)_InterlockedExchangeAdd16_acq((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, acquire_type) noexcept {
        return (T)_InterlockedExchangeAdd32_acq((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, acquire_type) noexcept {
        return (T)_InterlockedExchangeAdd64_acq((__int64 volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, release_type) noexcept {
        return (T)_InterlockedExchangeAdd8_rel((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, release_type) noexcept {
        return (T)_InterlockedExchangeAdd16_rel((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, release_type) noexcept {
        return (T)_InterlockedExchangeAdd32_rel((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, release_type) noexcept {
        return (T)_InterlockedExchangeAdd64_rel((__int64 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedExchangeAdd8((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedExchangeAdd16((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedExchangeAdd32((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedExchangeAdd64((__int64 volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(integral) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(
        T volatile*, T value, memory_order_type<O> order) noexcept {
        return fetch_add(ctx, (T)(0u - (make_unsigned_t<T>)value), order);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedAnd8((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedAnd16((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedAnd32((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedAnd64((__int64 volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedOr8((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedOr16((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedOr32((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedOr64((__int64 volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedXor8((char volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedXor16((short volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedXor32((long volatile*)ctx, value);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T volatile*, T value, memory_order_type<O>) noexcept {
        return (T)_InterlockedXor64((__int64 volatile*)ctx, value);
    }

public:
    template <memory_order O>
    struct operations {
    protected:
        static constexpr value_constant<memory_order, O> this_order{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(T* ctx, T value) noexcept {
            return (T)exchange(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(T* ctx, T value) noexcept {
            return (T)exchange((underlying_type_t<T>*)ctx, (underlying_type_t<T>)value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(T* ctx, T value) noexcept {
            return (T)exchange((intptr_t*)ctx, (intptr_t)value);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(T* ctx, T value) noexcept {
            return (T)fetch_add(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
            T* ctx, underlying_type_t<T> value) noexcept {
            return (T)fetch_add((underlying_type_t<T>*)ctx, value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
            T* ctx, ptrdiff_t value) noexcept {
            static constexpr intptr_t stride = sizeof(T);
            return (T)fetch_add((intptr_t*)ctx, (intptr_t)value * stride);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(T* ctx, T value) noexcept {
            return (T)fetch_sub(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(
            T* ctx, underlying_type_t<T> value) noexcept {
            return (T)fetch_sub((underlying_type_t<T>*)ctx, value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(
            T* ctx, ptrdiff_t value) noexcept {
            static constexpr intptr_t stride = sizeof(T);
            return (T)fetch_sub((intptr_t*)ctx, (intptr_t)value * stride);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(T* ctx, T value) noexcept {
            return (T)fetch_and(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
            T* ctx, underlying_type_t<T> value) noexcept {
            return (T)fetch_and((underlying_type_t<T>*)ctx, value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
            T* ctx, uintptr_t value) noexcept {
            return (T)fetch_and((uintptr_t*)ctx, value);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(T* ctx, T value) noexcept {
            return (T)fetch_or(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
            T* ctx, underlying_type_t<T> value) noexcept {
            return (T)fetch_or((underlying_type_t<T>*)ctx, value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
            T* ctx, uintptr_t value) noexcept {
            return (T)fetch_or((uintptr_t*)ctx, value);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(T* ctx, T value) noexcept {
            return (T)fetch_xor(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
            T* ctx, underlying_type_t<T> value) noexcept {
            return (T)fetch_xor((underlying_type_t<T>*)ctx, value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
            T* ctx, uintptr_t value) noexcept {
            return (T)fetch_xor((uintptr_t*)ctx, value);
        }
    };
};

template <>
struct exchange_type::operations<memory_order::consume> :
    exchange_type::operations<memory_order::acquire> {};

template <memory_order O>
using exchange_operations = exchange_type::operations<O>;

struct compare_exchange_type {
private:
    using relaxed_type = memory_order_type<memory_order::relaxed>;
    using acquire_type = memory_order_type<memory_order::acquire>;
    using release_type = memory_order_type<memory_order::release>;
    using consume_type = memory_order_type<memory_order::consume>;
    using seq_cst_type = memory_order_type<memory_order::seq_cst>;
    using acq_rel_type = memory_order_type<memory_order::acq_rel>;

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, relaxed_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev =
            (T)(_InterlockedCompareExchange8_nf((char volatile*)ctx, (char)desired, (char)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, relaxed_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev = (T)(_InterlockedCompareExchange16_nf(
            (short volatile*)ctx, (short)desired, (short)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, relaxed_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev =
            (T)(_InterlockedCompareExchange32_nf((long volatile*)ctx, (long)desired, (long)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, relaxed_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev = (T)(_InterlockedCompareExchange64_nf(
            (__int64 volatile*)ctx, (__int64)desired, (__int64)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, relaxed_type, relaxed_type) noexcept {
        auto ex = *expected;
        auto prev = (T)(_InterlockedCompareExchangePointer_nf((void* volatile*)ctx, desired, ex));
        if (ex != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, relaxed_type, relaxed_type) noexcept {
        alignas(16) __int64 buf[2];
        ::memcpy(buf, &desired, 16);
        return _InterlockedCompareExchange128_nf(
            (__int64 volatile*)ctx, buf[1], buf[0], (__int64*)expected);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev =
            (T)(_InterlockedCompareExchange8_rel((char volatile*)ctx, (char)desired, (char)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev = (T)(_InterlockedCompareExchange16_rel(
            (short volatile*)ctx, (short)desired, (short)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev =
            (T)(_InterlockedCompareExchange32_rel((long volatile*)ctx, (long)desired, (long)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev = (T)(_InterlockedCompareExchange64_rel(
            (__int64 volatile*)ctx, (__int64)desired, (__int64)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, relaxed_type) noexcept {
        auto ex = *expected;
        auto prev = (T)(_InterlockedCompareExchangePointer_rel((void* volatile*)ctx, desired, ex));
        if (ex != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, relaxed_type) noexcept {
        alignas(16) __int64 buf[2];
        ::memcpy(buf, &desired, 16);
        return _InterlockedCompareExchange128_rel(
            (__int64 volatile*)ctx, buf[1], buf[0], (__int64*)expected);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, acquire_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev =
            (T)(_InterlockedCompareExchange8_rel((char volatile*)ctx, (char)desired, (char)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, acquire_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev = (T)(_InterlockedCompareExchange16_rel(
            (short volatile*)ctx, (short)desired, (short)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, acquire_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev =
            (T)(_InterlockedCompareExchange32_rel((long volatile*)ctx, (long)desired, (long)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, acquire_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev = (T)(_InterlockedCompareExchange64_rel(
            (__int64 volatile*)ctx, (__int64)desired, (__int64)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, acquire_type, relaxed_type) noexcept {
        auto ex = *expected;
        auto prev = (T)(_InterlockedCompareExchangePointer_rel((void* volatile*)ctx, desired, ex));
        if (ex != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, acquire_type, relaxed_type) noexcept {
        alignas(16) __int64 buf[2];
        ::memcpy(buf, &desired, 16);
        return _InterlockedCompareExchange128_rel(
            (__int64 volatile*)ctx, buf[1], buf[0], (__int64*)expected);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, acquire_type) noexcept {
        auto comp = *expected;
        auto prev =
            (T)(_InterlockedCompareExchange8((char volatile*)ctx, (char)desired, (char)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, acquire_type) noexcept {
        auto comp = *expected;
        auto prev =
            (T)(_InterlockedCompareExchange16((short volatile*)ctx, (short)desired, (short)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, acquire_type) noexcept {
        auto comp = *expected;
        auto prev =
            (T)(_InterlockedCompareExchange32((long volatile*)ctx, (long)desired, (long)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, acquire_type) noexcept {
        auto comp = *expected;
        auto prev = (T)(_InterlockedCompareExchange64(
            (__int64 volatile*)ctx, (__int64)desired, (__int64)comp));
        if (comp != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, acquire_type) noexcept {
        auto ex = *expected;
        auto prev = (T)(_InterlockedCompareExchangePointer((void* volatile*)ctx, desired, ex));
        if (ex != prev) {
            *expected = prev;
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, release_type, acquire_type) noexcept {
        alignas(16) __int64 buf[2];
        ::memcpy(buf, &desired, 16);
        return _InterlockedCompareExchange128(
            (__int64 volatile*)ctx, buf[1], buf[0], (__int64*)expected);
    }

    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(integral<T> || UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, acq_rel_type, memory_order_type<O>) noexcept {
        return compare_exchange(ctx, expected, desired, release_type{}, acquire_type{});
    }

    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(integral<T> || UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, seq_cst_type, memory_order_type<O>) noexcept {
        return compare_exchange(ctx, expected, desired, release_type{}, acquire_type{});
    }

    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(integral<T> || UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, consume_type, memory_order_type<O> f) noexcept {
        return compare_exchange(ctx, expected, desired, acquire_type{}, f);
    }

    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(integral<T> || UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, T* expected, T desired, memory_order_type<O> s, consume_type) noexcept {
        return compare_exchange(ctx, expected, desired, s, acquire_type{});
    }

public:
    template <memory_order S, memory_order F>
    struct operations {
    protected:
        static constexpr value_constant<memory_order, S> success_v{};
        static constexpr value_constant<memory_order, F> fail_v{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        template <typename T UTL_CONSTRAINT_CXX11(
            UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(integral<T> || UTL_TRAIT_is_pointer(T))
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_strong(
            T* ctx, T* expected, T desired) noexcept {
            return compare_exchange(ctx, expected, desired, success_v, fail_v);
        }

        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_strong(
            T* ctx, T* expected, T desired) noexcept {
            return compare_exchange((underlying_type_t<T>*)ctx, (underlying_type_t<T>*)expected,
                (underlying_type_t<T>)desired, success_v, fail_v);
        }

        template <typename T UTL_CONSTRAINT_CXX11(
            UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(integral<T> || UTL_TRAIT_is_pointer(T))
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak(
            T* ctx, T* expected, T desired) noexcept {
            return compare_exchange(ctx, expected, desired, success_v, fail_v);
        }

        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak(
            T* ctx, T* expected, T desired) noexcept {
            return compare_exchange((underlying_type_t<T>*)ctx, (underlying_type_t<T>*)expected,
                (underlying_type_t<T>)desired, success_v, fail_v);
        }
    };
};

template <memory_order S, memory_order F>
using compare_exchange_operations = compare_exchange_type::operations<S, F>;

} // namespace atomics
} // namespace platform

UTL_NAMESPACE_END
