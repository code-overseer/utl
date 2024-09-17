// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#include "utl/concept/utl_implicit_lifetime.h"
#include "utl/numeric/utl_sized_integral.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_copy_cv.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_make_unsigned.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_underlying_type.h"
#include "utl/utility/utl_to_underlying.h"

#if !UTL_COMPILER_MSVC
#  error Invalid compiler
#endif

UTL_NAMESPACE_BEGIN

namespace atomics {
#if UTL_CXX20
template <typename T>
struct atomic_traits;
#else
template <typename T, typename = void>
struct atomic_traits;
#endif

#if UTL_CXX20
template <sized_integral<1> T>
struct atomic_traits<T> {
#else
template <typename T>
struct atomic_traits<T, enable_if_t<UTL_TRAIT_is_sized_integral(1, T)>> {
#endif
    using value_type = remove_cv_t<T>;
    using pointer = value_type*;
    using volatile_pointer = value_type volatile*;
    using const_pointer = value_type const*;
    using const_volatile_pointer = value_type const volatile*;
    using interlocked_type = char;

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type to_interlocked(value_type value) noexcept {
        return (interlocked_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type const volatile* to_interlocked(
        const_volatile_pointer value) noexcept {
        return value;
    }
    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr value_type to_value(interlocked_type value) noexcept {
        return (value_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr bool equal(value_type l, value_type r) noexcept { return l == r; }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr void assign(pointer dst, value_type src) noexcept { *dst = src; }
};

#if UTL_CXX20
template <sized_integral<2> T>
struct atomic_traits<T> {
#else
template <typename T>
struct atomic_traits<T, enable_if_t<UTL_TRAIT_is_sized_integral(2, T)>> {
#endif
    using value_type = remove_cv_t<T>;
    using pointer = value_type*;
    using volatile_pointer = value_type volatile*;
    using const_pointer = value_type const*;
    using const_volatile_pointer = value_type const volatile*;
    using interlocked_type = short;

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type to_interlocked(value_type value) noexcept {
        return (interlocked_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type const volatile* to_interlocked(
        const_volatile_pointer value) noexcept {
        return value;
    }
    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr value_type to_value(interlocked_type value) noexcept {
        return (value_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr bool equal(value_type l, value_type r) noexcept { return l == r; }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr void assign(pointer dst, value_type src) noexcept { *dst = src; }
};

#if UTL_CXX20
template <sized_integral<4> T>
struct atomic_traits<T> {
#else
template <typename T>
struct atomic_traits<T, enable_if_t<UTL_TRAIT_is_sized_integral(4, T)>> {
#endif
    using value_type = remove_cv_t<T>;
    using pointer = value_type*;
    using volatile_pointer = value_type volatile*;
    using const_pointer = value_type const*;
    using const_volatile_pointer = value_type const volatile*;
    using interlocked_type = long;

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type to_interlocked(value_type value) noexcept {
        return (interlocked_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type const volatile* to_interlocked(
        const_volatile_pointer value) noexcept {
        return value;
    }
    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr value_type to_value(interlocked_type value) noexcept {
        return (value_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr bool equal(value_type l, value_type r) noexcept { return l == r; }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr void assign(pointer dst, value_type src) noexcept { *dst = src; }
};

#if UTL_CXX20
template <sized_integral<8> T>
struct atomic_traits<T> {
#else
template <typename T>
struct atomic_traits<T, enable_if_t<UTL_TRAIT_is_sized_integral(8, T)>> {
#endif
    using value_type = remove_cv_t<T>;
    using pointer = value_type*;
    using volatile_pointer = value_type volatile*;
    using const_pointer = value_type const*;
    using const_volatile_pointer = value_type const volatile*;
    using interlocked_type = __int64;

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type to_interlocked(value_type value) noexcept {
        return (interlocked_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type const volatile* to_interlocked(
        const_volatile_pointer value) noexcept {
        return value;
    }
    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr value_type to_value(interlocked_type value) noexcept {
        return (value_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr bool equal(value_type l, value_type r) noexcept { return l == r; }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr void assign(pointer dst, value_type src) noexcept { *dst = src; }
};

#if UTL_CXX20
template <typename T>
requires is_pointer_v<T>
struct atomic_traits<T> {
#else
template <typename T>
struct atomic_traits<T, enable_if_t<UTL_TRAIT_is_pointer(T)>> {
#endif
    using value_type = remove_cv_t<T>;
    using pointer = value_type*;
    using volatile_pointer = value_type volatile*;
    using const_pointer = value_type const*;
    using const_volatile_pointer = value_type const volatile*;
    using interlocked_type = void*;

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type to_interlocked(value_type value) noexcept {
        return (interlocked_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type const volatile* to_interlocked(
        const_volatile_pointer value) noexcept {
        return value;
    }
    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr value_type to_value(interlocked_type value) noexcept {
        return (value_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr bool equal(value_type l, value_type r) noexcept { return l == r; }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr void assign(pointer dst, value_type src) noexcept { *dst = src; }
};

#if UTL_CXX20
template <enum_type T>
struct atomic_traits<T> : private atomic_traits<underlying_type_t<T>> {
#else
template <typename T>
struct atomic_traits<T, enable_if_t<UTL_TRAIT_is_enum(T)>> :
    private atomic_traits<underlying_type_t<T>> {
#endif
private:
    using base_type = atomic_traits<underlying_type_t<T>>;

public:
    using value_type = remove_cv_t<T>;
    using pointer = value_type*;
    using volatile_pointer = value_type volatile*;
    using const_pointer = value_type const*;
    using const_volatile_pointer = value_type const volatile*;
    using typename base_type::interlocked_type;

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type to_interlocked(value_type value) noexcept {
        return base_type::to_interlocked(to_underlying(value));
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type const volatile* to_interlocked(
        const_volatile_pointer value) noexcept {
        return base_type::to_interlocked(to_underlying(value));
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr value_type to_value(interlocked_type value) noexcept {
        return (value_type)base_type::to_value(value);
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr bool equal(value_type l, value_type r) noexcept {
        return base_type::equal(to_underlying(l), to_underlying(r));
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr void assign(pointer dst, value_type src) noexcept { *dst = src; }
};

struct fence_traits {
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

struct load_traits {
private:
    using relaxed_type = memory_order_type<memory_order::relaxed>;
    using seq_cst_type = memory_order_type<memory_order::seq_cst>;

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T const* ctx, relaxed_type) noexcept {
        return __iso_volatile_load8((__int8 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T const* ctx, relaxed_type) noexcept {
        return __iso_volatile_load16((__int16 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T const* ctx, relaxed_type) noexcept {
        return __iso_volatile_load32((__int32 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T const* ctx, relaxed_type) noexcept {
        return __iso_volatile_load64((__int64 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T const* ctx, seq_cst_type) noexcept {
        return (T)__ldar8((unsigned __int8 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T const* ctx, seq_cst_type) noexcept {
        return (T)__ldar16((unsigned __int16 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T const* ctx, seq_cst_type) noexcept {
        return (T)__ldar32((unsigned __int32 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(T const* ctx, seq_cst_type) noexcept {
        return (T)__ldar64((unsigned __int64 const volatile*)ctx);
    }
#ifdef UTL_ENABLE_RCPC_LOAD
#  define __UTL_LOAD_ACQ(WIDTH, TARGET) \
      __ldapr##WIDTH((unsigned __int##WIDTH const volatile*)TARGET)
#else
#  define __UTL_LOAD_ACQ(WIDTH, TARGET) __ldar##WIDTH((unsigned __int##WIDTH const volatile*)TARGET)
#endif
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(
        T const* ctx, memory_order_type<O>) noexcept {
        return (T)__UTL_LOAD_ACQ(8, ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(
        T const* ctx, memory_order_type<O>) noexcept {
        return (T)__UTL_LOAD_ACQ(16, ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(
        T const* ctx, memory_order_type<O>) noexcept {
        return (T)__UTL_LOAD_ACQ(32, ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void load(
        T const* ctx, memory_order_type<O>) noexcept {
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

struct store_traits {
private:
    template <typename T>
    using value_type = typename atomic_traits<T>::value_type;
    using relaxed_type = value_constant<memory_order, memory_order::relaxed>;
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        __iso_volatile_store8((__int8 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        __iso_volatile_store16((__int16 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        __iso_volatile_store32((__int32 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        __iso_volatile_store64((__int64 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, value_constant<memory_order, O>) noexcept {
        __stlr8((unsigned __int8 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, value_constant<memory_order, O>) noexcept {
        __stlr16((unsigned __int16 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, value_constant<memory_order, O>) noexcept {
        __stlr32((unsigned __int32 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, value_constant<memory_order, O>) noexcept {
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
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
            T* ctx, value_type<T> value) noexcept {
            store(ctx, value, this_order);
        }

        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
            T* ctx, value_type<T> value) noexcept {
            store((underlying_type_t<T>*)ctx, (underlying_type_t<T>)value);
        }

        template <UTL_CONCEPT_CXX20(boolean_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_boolean(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
            T* ctx, value_type<T> value) noexcept {
            store(ctx, value, this_order);
        }
    };
};

template <memory_order O>
using fence_operations = fence_traits::operations<O>;
template <memory_order O>
using load_operations = load_traits::operations<O>;
template <memory_order O>
using store_operations = store_traits::operations<O>;

struct exchange_traits {
private:
    template <typename T>
    using value_type = typename atomic_traits<T>::value_type;
    using relaxed_type = memory_order_type<memory_order::relaxed>;
    using acquire_type = memory_order_type<memory_order::acquire>;
    using release_type = memory_order_type<memory_order::release>;
    using consume_type = memory_order_type<memory_order::consume>;
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedExchange8_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange16_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange32_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange64_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangePointer_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange8_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange16_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange32_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange64_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangePointer_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange8_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange16_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange32_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchange64_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangePointer_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedExchange8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedExchange16(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedExchange32(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedExchange64(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangePointer(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd8_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd16_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd32_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd64_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd8_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd16_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd32_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd64_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd8_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd16_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd32_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd64_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedExchangeAdd8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd16(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd32(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(_InterlockedExchangeAdd64(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(integral) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(
        T* ctx, value_type<T> value, memory_order_type<O> order) noexcept {
        return fetch_add(ctx, (T)(0u - (make_unsigned_t<T>)value), order);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd8_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd16_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd32_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd64_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd8_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd16_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd32_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd64_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd8_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd16_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd32_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd64_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd16(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd32(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedAnd64(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr8_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr16_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr32_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr64_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr8_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr16_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr32_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr64_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr8_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr16_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr32_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr64_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr16(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr32(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedOr64(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor8_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor16_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor32_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor64_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor8_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor16_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor32_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor64_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor8_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor16_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor32_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor64_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor16(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor32(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor = atomic_traits<T>;
        return adaptor::to_value(
            _InterlockedXor64(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

public:
    template <memory_order O>
    struct operations {
    protected:
        static constexpr value_constant<memory_order, O> this_order{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        template <typename T UTL_CONSTRAINT_CXX11(
            UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(integral<T> || is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
            T* ctx, value_type<T> value) noexcept {
            return exchange(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T exchange(
            T* ctx, value_type<T> value) noexcept {
            using type = copy_cv_t<T, underlying_type_t<T>>;
            return (T)exchange((type*)ctx, (underlying_type_t<T>)value);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
            T* ctx, value_type<T> value) noexcept {
            return fetch_add(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
            T* ctx, underlying_type_t<T> value) noexcept {
            using type = copy_cv_t<T, underlying_type_t<T>>;
            return (T)fetch_add((type*)ctx, value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_add(
            T* ctx, ptrdiff_t value) noexcept {
            static constexpr intptr_t stride = sizeof(T);
            using type = copy_cv_t<T, intptr_t>;
            return (T)fetch_add((type*)ctx, (intptr_t)value * stride);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(
            T* ctx, value_type<T> value) noexcept {
            return fetch_sub(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(
            T* ctx, underlying_type_t<T> value) noexcept {
            using type = copy_cv_t<T, underlying_type_t<T>>;
            return (T)fetch_sub((type*)ctx, value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_sub(
            T* ctx, ptrdiff_t value) noexcept {
            static constexpr intptr_t stride = sizeof(T);
            using type = copy_cv_t<T, intptr_t>;
            return (T)fetch_sub((type*)ctx, (intptr_t)value * stride);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
            T* ctx, value_type<T> value) noexcept {
            return fetch_and(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
            T* ctx, underlying_type_t<T> value) noexcept {
            using type = copy_cv_t<T, underlying_type_t<T>>;
            return (T)fetch_and((type*)ctx, value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_and(
            T* ctx, uintptr_t value) noexcept {
            using type = copy_cv_t<T, uintptr_t>;
            return (T)fetch_and((type*)ctx, value);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
            T* ctx, value_type<T> value) noexcept {
            return fetch_or(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
            T* ctx, underlying_type_t<T> value) noexcept {
            using type = copy_cv_t<T, underlying_type_t<T>>;
            return (T)fetch_or((type*)ctx, value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_or(
            T* ctx, uintptr_t value) noexcept {
            using type = copy_cv_t<T, uintptr_t>;
            return (T)fetch_or((type*)ctx, value);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
            T* ctx, value_type<T> value) noexcept {
            return fetch_xor(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
            T* ctx, underlying_type_t<T> value) noexcept {
            using type = copy_cv_t<T, underlying_type_t<T>>;
            return (T)fetch_xor((type*)ctx, value);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline T fetch_xor(
            T* ctx, uintptr_t value) noexcept {
            using type = copy_cv_t<T, uintptr_t>;
            return (T)fetch_xor((type*)ctx, value);
        }
    };
};

template <>
struct exchange_traits::operations<memory_order::consume> :
    exchange_traits::operations<memory_order::acquire> {};

template <memory_order O>
using exchange_operations = exchange_traits::operations<O>;

struct compare_exchange_traits {
private:
    template <typename T>
    using value_type = typename atomic_traits<T>::value_type;
    template <typename T>
    using pointer = typename atomic_traits<T>::pointer;
    using relaxed_type = memory_order_type<memory_order::relaxed>;
    using acquire_type = memory_order_type<memory_order::acquire>;
    using release_type = memory_order_type<memory_order::release>;
    using consume_type = memory_order_type<memory_order::consume>;
    using seq_cst_type = memory_order_type<memory_order::seq_cst>;
    using acq_rel_type = memory_order_type<memory_order::acq_rel>;
    static constexpr release_type release_v{};
    static constexpr acquire_type acquire_v{};

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, relaxed_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev = adaptor::to_value(_InterlockedCompareExchange8_nf(adaptor::to_interlocked(ctx),
            adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, relaxed_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev = adaptor::to_value(_InterlockedCompareExchange16_nf(adaptor::to_interlocked(ctx),
            adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, relaxed_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev = adaptor::to_value(_InterlockedCompareExchange32_nf(adaptor::to_interlocked(ctx),
            adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, relaxed_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev = adaptor::to_value(_InterlockedCompareExchange64_nf(adaptor::to_interlocked(ctx),
            adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, relaxed_type, relaxed_type) noexcept {
        auto comp = *expected;
        auto prev =
            adaptor::to_value(_InterlockedCompareExchangePointer_nf(adaptor::to_interlocked(ctx),
                adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, relaxed_type, relaxed_type) noexcept {
        alignas(16) __int64 buf[2];
        ::memcpy(buf, &desired, 16);
        return _InterlockedCompareExchange128_nf(
            (__int64 volatile*)ctx, buf[1], buf[0], (__int64*)expected);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev = adaptor::to_value(_InterlockedCompareExchange8_rel(adaptor::to_interlocked(ctx),
            adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev =
            adaptor::to_value(_InterlockedCompareExchange16_rel(adaptor::to_interlocked(ctx),
                adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev =
            adaptor::to_value(_InterlockedCompareExchange32_rel(adaptor::to_interlocked(ctx),
                adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev =
            adaptor::to_value(_InterlockedCompareExchange64_rel(adaptor::to_interlocked(ctx),
                adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev =
            adaptor::to_value(_InterlockedCompareExchangePointer_rel(adaptor::to_interlocked(ctx),
                adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, relaxed_type) noexcept {
        alignas(16) __int64 buf[2];
        ::memcpy(buf, &desired, 16);
        return _InterlockedCompareExchange128_rel(
            (__int64 volatile*)ctx, buf[1], buf[0], (__int64*)expected);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, acquire_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev = adaptor::to_value(_InterlockedCompareExchange8_acq(adaptor::to_interlocked(ctx),
            adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, acquire_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev =
            adaptor::to_value(_InterlockedCompareExchange16_acq(adaptor::to_interlocked(ctx),
                adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, acquire_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev =
            adaptor::to_value(_InterlockedCompareExchange32_acq(adaptor::to_interlocked(ctx),
                adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, acquire_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev =
            adaptor::to_value(_InterlockedCompareExchange64_acq(adaptor::to_interlocked(ctx),
                adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, acquire_type, relaxed_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev =
            adaptor::to_value(_InterlockedCompareExchangePointer_acq(adaptor::to_interlocked(ctx),
                adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, acquire_type, relaxed_type) noexcept {
        alignas(16) __int64 buf[2];
        ::memcpy(buf, &desired, 16);
        return _InterlockedCompareExchange128_acq(
            (__int64 volatile*)ctx, buf[1], buf[0], (__int64*)expected);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev = adaptor::to_value(_InterlockedCompareExchange8(adaptor::to_interlocked(ctx),
            adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev = adaptor::to_value(_InterlockedCompareExchange16(adaptor::to_interlocked(ctx),
            adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev = adaptor::to_value(_InterlockedCompareExchange32(adaptor::to_interlocked(ctx),
            adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev = adaptor::to_value(_InterlockedCompareExchange64(adaptor::to_interlocked(ctx),
            adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, acquire_type) noexcept {
        using adaptor = atomic_traits<T>;
        auto comp = *expected;
        auto prev =
            adaptor::to_value(_InterlockedCompareExchangePointer(adaptor::to_interlocked(ctx),
                adaptor::to_interlocked(desired), adaptor::to_interlocked(comp)));
        if (!adaptor::equal(comp, prev)) {
            adaptor::assign(expected, prev);
            return false;
        }
        return true;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, release_type, acquire_type) noexcept {
        alignas(16) __int64 buf[2];
        ::memcpy(buf, &desired, 16);
        return _InterlockedCompareExchange128(
            (__int64 volatile*)ctx, buf[1], buf[0], (__int64*)expected);
    }

    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(integral<T> || is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(T* ctx,
        pointer<T> expected, value_type<T> desired, acq_rel_type, memory_order_type<O>) noexcept {
        return compare_exchange(ctx, expected, desired, release_v, acquire_v);
    }

    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(integral<T> || is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(T* ctx,
        pointer<T> expected, value_type<T> desired, seq_cst_type, memory_order_type<O>) noexcept {
        return compare_exchange(ctx, expected, desired, release_v, acquire_v);
    }

    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(integral<T> || is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(T* ctx,
        pointer<T> expected, value_type<T> desired, consume_type, memory_order_type<O> f) noexcept {
        return compare_exchange(ctx, expected, desired, acquire_v, f);
    }

    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(integral<T> || is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(T* ctx,
        pointer<T> expected, value_type<T> desired, memory_order_type<O> s, consume_type) noexcept {
        return compare_exchange(ctx, expected, desired, s, acquire_v);
    }

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(integral<T> || is_pointer_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, consume_type, consume_type) noexcept {
        return compare_exchange(ctx, expected, desired, acquire_v, acquire_v);
    }

public:
    template <memory_order S, memory_order F>
    struct operations {
    protected:
        static constexpr memory_order_type<S> success_v{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        template <typename T UTL_CONSTRAINT_CXX11(
            UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(integral<T> || is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_strong(T* ctx,
            pointer<T> expected, value_type<T> desired, compare_exchange_failure<F> f) noexcept {
            return compare_exchange(ctx, expected, desired, success_v, f);
        }

        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_strong(T* ctx,
            pointer<T> expected, value_type<T> desired, compare_exchange_failure<F> f) noexcept {
            using type = copy_cv_t<T, underlying_type_t<T>>;
            return compare_exchange((type*)ctx, (underlying_type_t<T>*)expected,
                (underlying_type_t<T>)desired, success_v, f);
        }

        template <typename T UTL_CONSTRAINT_CXX11(
            UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(integral<T> || is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak(T* ctx,
            pointer<T> expected, value_type<T> desired, compare_exchange_failure<F> f) noexcept {
            // Only < ARMv8.1-a has no dedicated CAS instruction
            // But it requires exclusive load/store intrinsics to be exposed by MSVC to be supported
            return compare_exchange(ctx, expected, desired, success_v, f);
        }

        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak(T* ctx,
            pointer<T> expected, value_type<T> desired, compare_exchange_failure<F> f) noexcept {
            using type = copy_cv_t<T, underlying_type_t<T>>;
            return compare_exchange((type*)ctx, (underlying_type_t<T>*)expected,
                (underlying_type_t<T>)desired, success_v, f);
        }
    };
};

template <memory_order S, memory_order F>
using compare_exchange_operations = compare_exchange_traits::operations<S, F>;

} // namespace atomics

UTL_NAMESPACE_END
