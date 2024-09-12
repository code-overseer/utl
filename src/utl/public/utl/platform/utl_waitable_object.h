// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/platform/utl_clock_fwd.h"

#include "utl/algorithm/utl_min.h"
#include "utl/platform/utl_time_duration.h"
#include "utl/type_traits/utl_is_function.h"
#include "utl/type_traits/utl_is_has_unique_object_representations.h"
#include "utl/type_traits/utl_is_reference.h"
#include "utl/type_traits/utl_is_trivially_copyable.h"
#include "utl/type_traits/utl_remove_pointer.h"

#if UTL_TARGET_APPLE
#  include "darwin/utl_futex.h"
#endif

UTL_NAMESPACE_BEGIN

namespace platform {
template <typename T>
class waitable_obect;
template <typename T>
class single_waitable_obect;

template <typename T>
class waitable_obect {
#if UTL_TRAIT_SUPPORTED_has_unique_object_representations
    static_assert(UTL_TRAIT_has_unique_object_representations(T), "Invalid type");
#else
    static_assert(UTL_TRAIT_is_trivially_copyable(T), "Invalid type");
#endif
    static_assert(UTL_TRAIT_is_object(T), "Invalid type");
    static_assert(details::futex::is_waitable<T>::value, "Invalid type");

public:
    constexpr explicit waitable_obect(T& t) : address(__UTL addressof(t)) {}
    waitable_obect(waitable_obect const&) = delete;
    waitable_obect(waitable_obect&&) = delete;
    waitable_obect& operator=(waitable_obect const&) = delete;
    waitable_obect& operator=(waitable_obect&&) = delete;
    UTL_CONSTEXPR_CXX20 ~waitable_obect() noexcept = default;

    template <typename R, typename P>
    void wait(value_type old, ::std::chrono::duration<R, P> timeout,
        __UTL memory_order o = __UTL memory_order_seq_cst) const noexcept {
        wait(old, time_duration{timeout}, o);
    }

    void wait(value_type old, __UTL memory_order o = __UTL memory_order_seq_cst) const noexcept {
        return wait(old, time_duration::invalid(), o);
    }

    void wait(value_type old, time_duration,
        __UTL memory_order o = __UTL memory_order_seq_cst) const noexcept;

    void notify_one() noexcept;

    void notify_all() noexcept;

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    T load(__UTL memory_order o = __UTL memory_order_seq_cst) const noexcept {
        return __UTL atomic_ref<value_type>(*address).load(o);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    void store(T value, __UTL memory_order o = __UTL memory_order_seq_cst) noexcept {
        return __UTL atomic_ref<value_type>(*address).store(value, o);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    T exchange(T value, __UTL memory_order o = __UTL memory_order_seq_cst) noexcept {
        return __UTL atomic_ref<value_type>(*address).exchange(value, o);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    T fetch_add(T value, __UTL memory_order o = __UTL memory_order_seq_cst) noexcept {
        return __UTL atomic_ref<value_type>(*address).fetch_add(value, o);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    T fetch_sub(T value, __UTL memory_order o = __UTL memory_order_seq_cst) noexcept {
        return __UTL atomic_ref<value_type>(*address).fetch_sub(value, o);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    T fetch_and(T value, __UTL memory_order o = __UTL memory_order_seq_cst) noexcept {
        return __UTL atomic_ref<value_type>(*address).fetch_and(value, o);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    T fetch_or(T value, __UTL memory_order o = __UTL memory_order_seq_cst) noexcept {
        return __UTL atomic_ref<value_type>(*address).fetch_or(value, o);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    T fetch_xor(T value, __UTL memory_order o = __UTL memory_order_seq_cst) noexcept {
        return __UTL atomic_ref<value_type>(*address).fetch_xor(value, o);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    bool comapre_exchange_strong(
        T& expected, T desired, __UTL memory_order o = __UTL memory_order_seq_cst) noexcept {
        return __UTL atomic_ref<value_type>(*address).comapre_exchange_strong(expected, desired, o);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    bool comapre_exchange_strong(
        T& expected, T desired, __UTL memory_order success, __UTL memory_order failure) noexcept {
        return __UTL atomic_ref<value_type>(*address).comapre_exchange_strong(
            expected, desired, success, failure);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    bool comapre_exchange_weak(
        T& expected, T desired, __UTL memory_order o = __UTL memory_order_seq_cst) noexcept {
        return __UTL atomic_ref<value_type>(*address).comapre_exchange_weak(expected, desired, o);
    }

    UTL_ATRRIBUTE(ALWAYS_INLINE)
    bool comapre_exchange_weak(
        T& expected, T desired, __UTL memory_order success, __UTL memory_order failure) noexcept {
        return __UTL atomic_ref<value_type>(*address).comapre_exchange_weak(
            expected, desired, success, failure);
    }

private:
    T* address;
    unsigned int wait_count = 0;
    signed int notify_flag = 0;
};

template <typename T>
void waitable_obect<T>::wait(
    value_type const old, time_duration const t, __UTL memory_order const o) const noexcept {
    static int const _ = []() {
        // Anything post-Nehalem
        UTL_ASSERT(hardware_ticks::invariant_frequency());
        return 0;
    }();

    auto const has_changed = [&]() { return __UTL platform::atomic_load(address, o) == old; };
    auto const elapsed_tick = [start = get_time(hardware_clock, __UTL memory_order_relaxed)]() {
        return get_time(__UTL platform::hardware_clock, __UTL memory_order_relaxed) - start;
    };
    auto const on_complete = [&]() {
        __UTL platform::thread_fence(__UTL memory_order_release);
        auto waits = __UTL platform::atomic_fetch_sub(&wait_count, 1, __UTL memory_order_relaxed);
        if (waits == 1) {
            __UTL platform::atomic_store(&notify_flag, 0, __UTL memory_order_relaxed);
        }
    };
    auto const on_notified = [&]() {
        __UTL platform::atomic_fetch_sub(&notify_flag, 1, __UTL memory_order_relaxed);
        on_complete();
    };
    auto const is_spin_timeout = [&]() {
        static constexpr size_t spin_cycles = 1000;
        auto const elapsed = elapsed_tick();
        return to_time_duration(elapsed) > t && elapsed.value() > spin_cycles;
    };

    __UTL platform::atomic_fetch_add(&wait_count, 1, __UTL memory_order_release);
    do {
        if (has_changed()) {
            on_complete();
            return;
        }

        __UTL platform::pause();
    } while (!is_spin_timeout());

    auto const should_poll = [&]() {
        return !has_changed() && to_time_duration(elapsed_tick()) < t;
    };

    while (should_poll()) {
        auto const notified = __UTL platform::atomic_load(&notify_flag, __UTL memory_order_acquire);
        if (notified < 0) {
            on_complete();
            return;
        }
        if (notified != 0) {
            on_notified();
            return;
        }

        static constexpr time_duration sleep_slice(0, 64000);
        auto const remaining = t - to_time_duration(elapsed_tick());
        // We cannot put a permanent sleep here, otherwise we cannot guarantee
        // a notify call will wake the futex, setting a max sleep will ensure
        // that the maximum latency will be ~64us.
        details::futex::wait(address, old, __UTL min(remaining, sleep_slice));
    }

    on_complete();
}

template <typename T>
T waitable_obect::notify_one() const noexcept {
    if (__UTL platform::atomic_load(&wait_count, __UTL memory_order_acquire) > 0) {
        __UTL platform::atomic_fetch_add(&notify_flag, 1, __UTL memory_order_relaxed);
        details::futex::notify_one(address);
    }
}

template <typename T>
T waitable_obect::notify_all() const noexcept {
    if (__UTL platform::atomic_load(&wait_count, __UTL memory_order_acquire) > 0) {
        __UTL platform::atomic_fetch_or(&notify_flag, 1 << 31, __UTL memory_order_relaxed);
        details::futex::notify_all(address);
    }
}
} // namespace platform

UTL_NAMESPACE_END
