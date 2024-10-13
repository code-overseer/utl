// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/platform/utl_clock_fwd.h"

#include "utl/algorithm/utl_min.h"
#include "utl/atomics/utl_atomics.h"
#include "utl/platform/utl_time_duration.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_explicit_constructible.h"
#include "utl/type_traits/utl_is_has_unique_object_representations.h"
#include "utl/type_traits/utl_is_inequality_comparable.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_trivially_copyable.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"

#if UTL_TARGET_APPLE
#  include "apple/utl_futex.h"
#elif UTL_TARGET_LINUX
#  include "linux/utl_futex.h"
#elif UTL_TARGET_MICROSOFT
#  include "winapi/utl_futex.h"
#else
#  error Unsupported target
#endif

UTL_NAMESPACE_BEGIN

namespace platform {

/**
 * A waitable object that allows a single consumer to wait on a value of type T.
 * The type T must meet certain requirements, including being trivially copyable
 * or having a unique object representation, being an object type, and being
 * inequality comparable.
 *
 * @tparam T The type of the value to be stored in the waitable object.
 *
 * @note This class is not copyable or movable.
 */
template <typename T>
class waitable_object {
#if UTL_TRAIT_SUPPORTED_has_unique_object_representations
    static_assert(UTL_TRAIT_has_unique_object_representations(T), "Invalid type");
#else
    static_assert(UTL_TRAIT_is_trivially_copyable(T), "Invalid type");
#endif
    static_assert(UTL_TRAIT_is_object(T), "Invalid type");
    static_assert(UTL_TRAIT_is_inequality_comparable(T), "Invalid type");

public:
#if UTL_CXX20
    /**
     * Constructs a waitable object with the default constructor of T.
     * This constructor is explicit if T is explicitly constructible.
     */
    constexpr explicit(UTL_TRAIT_is_explicit_constructible(T))
        waitable_object() noexcept(UTL_TRAIT_is_nothrow_default_constructible(T)) = default;

    /**
     * Constructs a waitable object with the provided arguments, which are forwarded to T's
     * constructor. This constructor is explicit if T is explicitly constructible with the given
     * arguments.
     *
     * @tparam Args The types of the arguments to forward to T's constructor.
     * @param args The arguments to forward to T's constructor.
     */
    template <typename... Args>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(T, Args...))
    constexpr explicit(UTL_TRAIT_is_explicit_constructible(T, Args...))
        waitable_object(Args&&... args) noexcept(UTL_TRAIT_is_nothrow_constructible(T, Args...))
        : data_{__UTL forward<Args>(args)} {}
#else
    /**
     * Constructs a waitable object with the default constructor of T.
     * This constructor is explicit if T is explicitly constructible.
     */
    template <bool Object = __UTL always_true<T>()
                  UTL_CONSTRAINT_CXX11(Object && UTL_TRAIT_is_explicit_constructible(T))>
    constexpr explicit waitable_object() noexcept(UTL_TRAIT_is_nothrow_default_constructible(T)) {}
    template <bool Object = __UTL always_true<T>()
                  UTL_CONSTRAINT_CXX11(Object && !UTL_TRAIT_is_explicit_constructible(T))>
    constexpr waitable_object() noexcept(UTL_TRAIT_is_nothrow_default_constructible(T)) {}

    /**
     * Constructs a waitable object with the provided arguments, which are forwarded to T's
     * constructor. This constructor is explicit if T is explicitly constructible with the given
     * arguments.
     *
     * @tparam Args The types of the arguments to forward to T's constructor.
     * @param args The arguments to forward to T's constructor.
     */
    template <typename... Args UTL_CONSTRAINT_CXX11( UTL_TRAIT_is_constructible(T, Args...) &&
            UTL_TRAIT_is_explicit_constructible(T, Args...))>
    constexpr explicit waitable_object(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...))
        : data_{__UTL forward<Args>(args)} {}
    template <typename... Args UTL_CONSTRAINT_CXX11( UTL_TRAIT_is_constructible(T, Args...) &&
            !UTL_TRAIT_is_explicit_constructible(T, Args...))>
    constexpr waitable_object(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...))
        : data_{__UTL forward<Args>(args)} {}

#endif
    waitable_object(waitable_object const&) = delete;
    waitable_object(waitable_object&&) = delete;
    waitable_object& operator=(waitable_object const&) = delete;
    waitable_object& operator=(waitable_object&&) = delete;
    UTL_CONSTEXPR_CXX20 ~waitable_object() noexcept = default;

    /**
     * Waits for the value of the object to change from the given old value.
     * The wait is performed with a timeout and a specified memory order.
     *
     * @tparam R The representation of the duration.
     * @tparam P The period of the duration.
     * @tparam O The memory order to use for the operation.
     *
     * @param old The value to compare against the current value of the object.
     * @param timeout The maximum time to wait before returning.
     */
    template <typename R, typename P, memory_order O>
    void wait(value_type old, ::std::chrono::duration<R, P> timeout) const noexcept {
        wait<O>(old, time_duration{timeout});
    }
    template <memory_order O>
    void wait(value_type old, time_duration timeout) const noexcept;

    /**
     * Waits for the value of the object to change from the given old value.
     * The wait is performed with a specified memory order.
     *
     * @tparam O The memory order to use for the operation.
     * @param old The value to compare against the current value of the object.
     */
    template <memory_order O>
    void wait(value_type old) const noexcept;

    /**
     * Notifies a single waiting thread that the object has changed.
     */
    void notify_one() noexcept;

    /**
     * Deleted. Notifies all waiting threads. This operation is not allowed for this class.
     */
    void notify_all() noexcept = delete;

    /**
     * Gets a reference to the stored value.
     *
     * @return A reference to the stored value.
     */
    UTL_ATTRIBUTE(ALWAYS_INLINE) T& value() & noexcept {
        return data_;
    }
    UTL_ATTRIBUTE(ALWAYS_INLINE) T&& value() && noexcept { return __UTL move(data_); }
    UTL_ATTRIBUTE(ALWAYS_INLINE) T const& value() const& noexcept { return data_; }
    UTL_ATTRIBUTE(ALWAYS_INLINE) T const&& value() const&& noexcept { return __UTL move(data_); }

private:
    enum class notification : unsigned int {
        none = 0,
        notify = 1,
        deep_sleep = 2,
    };
    static constexpr time_duration sleep_slices[]{
        {0,  500},
        {0, 1000},
        {0, 2000}
    };
    static constexpr size_t spin_cycles = 1000;
    static constexpr hardware_ticks elapsed_ticks(time_duration start) noexcept {
        return get_time(__UTL platform::hardware_clock, __UTL memory_order_relaxed) - start;
    }

    constexpr bool is_notified() const noexcept {
        auto const flag = __UTL atomic_relaxed::load(&flag_);
        return ((unsigned int)flag & (unsigned int)notification::notify) > 0;
    }

    static void ensure_hardware() noexcept {
        static int const _ = []() {
            // Anything post-Nehalem
            UTL_ASSERT(hardware_ticks::invariant_frequency());
            return 0;
        }();
        (void)_;
    }

    static constexpr bool has_spin_timeout(hardware_ticks start, time_duration timeout) noexcept {
        auto const elapsed = elapsed_ticks(start);
        return timeout < to_time_duration(elapsed) && spin_cycles < elapsed.value();
    }

    static constexpr bool has_spin_timeout(hardware_ticks start) noexcept {
        return spin_cycles < elapsed_ticks(start).value();
    }

    T data_;
    notification flag_ = notification::none;

    static_assert(details::futex::is_waitable<notification>::value, "Invalid type");
};

template <typename T>
void waitable_object<T>::notify_one() noexcept {
    auto const flag = atomic_release::exchange(&flag_, notification::notify);
    if (flag == notification::deep_sleep) {
        details::futex::notify_one(address);
    }
}

template <typename T>
template <memory_order O>
void waitable_object<T>::wait(value_type old) const noexcept {
    ensure_hardware();
    UTL_ATTRIBUTE(MAYBE_UNUSED) auto const _ = atomic_acquire::exchange(&flag_, notification::none);
    if (__UTL atomic_operations<O>::load(&data_) != old) {
        return;
    }

    auto const start_tick = get_time(__UTL platform::hardware_clock, __UTL memory_order_relaxed);
    UTL_COMPILER_BARRIER();
    do {
        if (is_notified()) {
            goto complete;
        }

        __UTL platform::pause();
    } while (!has_spin_timeout(start_tick));

    for (auto slice : sleep_slices) {
        if (is_notified()) {
            goto complete;
        }

        platform::sleep(slice);
    }
    auto const flag = atomic_relaxed::exchange(&flag_, notification::deep_sleep);
    if (((unsigned int)flag & (unsigned int)notification::notify) > 0) {
        goto complete;
    }

    while (true) {
        details::futex::wait(&flag_, notification::deep_sleep, time_duration::invalid());
        if (is_notified()) {
            goto complete;
        }
    }
complete:
    auto const volatile final_flag = atomic_acquire::exchange(&flag_, notification::none);
    UTL_ATTRIBUTE(MAYBE_UNUSED) auto const result =
        ((unsigned int)final_flag & (unsigned int)notification::notify) != 0;
    UTL_BUILTIN_assume(result);
    UTL_ASSERT(result);
}

template <typename T>
template <memory_order O>
void waitable_object<T>::wait(
    value_type const old, time_duration const timeout, memory_order_type<O>) const noexcept {
    ensure_hardware();
    if (__UTL atomic_operations<O>::load(&data_) != old) {
        return;
    }

    auto const start_tick = get_time(__UTL platform::hardware_clock, __UTL memory_order_relaxed);
    atomic_relaxed::store(&flag_, notification::none);
    UTL_COMPILER_BARRIER();
    do {
        if (is_notified()) {
            goto complete;
        }

        __UTL platform::pause();
    } while (!has_spin_timeout(start_tick, timeout));

    for (auto slice : sleep_slices) {
        if (is_notified()) {
            goto complete;
        }

        auto const duration = to_time_duration(elapsed_ticks(start_tick));
        if (timeout < duration) {
            return;
        }

        auto const remaining = timeout - duration + time_duration{0, 10};
        platform::sleep(__UTL min(slice, remaining));
    }
    auto const flag = atomic_relaxed::exchange(&flag_, notification::deep_sleep);
    if (((unsigned int)flag & (unsigned int)notification::notify) > 0) {
        goto complete;
    }

    while (true) {
        auto const duration = to_time_duration(elapsed_ticks(start_tick));
        if (timeout < duration) {
            return;
        }

        auto const remaining = timeout - duration + time_duration{0, 10};
        details::futex::wait(&flag_, notification::deep_sleep, remaining);
        if (is_notified()) {
            goto complete;
        }
    }
complete:
    auto const volatile final_flag = atomic_acquire::exchange(&flag_, notification::none);
    auto const result = ((unsigned int)final_flag & (unsigned int)notification::notify) != 0;
    UTL_ASSERT(result);
}

} // namespace platform

UTL_NAMESPACE_END
