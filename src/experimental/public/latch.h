#pragma once
#include "gcc/atomics.h"
#include "posix/futex.h"
#include "utl_config.h"

#include <atomic>

namespace utl::experimental {

class latch {
    static [[noreturn]] void throw_system_error(posix::result r) {
        char error_msg[1024];
        sprintf(error_msg, "Internal latch error: %s", posix::to_string(r));
        throw std::system_error(error_msg);
    }

public:
    explicit latch(int32_t n) : remaining_(n) {}
    latch(latch const&) = delete;
    latch& operator=(latch const&) = delete;

    template <typename R, typename P>
    bool wait_for(std::chrono::duration<R, P> d) {
        int32_t val = gcc::atomic_load(remaining_, std::memory_order_relaxed);
        while (val > 0) {
            auto const begin = std::chrono::high_resolution_clock::now();
            auto const r = posix::futex_wait(remaining_, &val, d);
            if (r == posix::result::success || r == posix::result::timeout) {
                return r == posix::result::success;
            }

            if (r == posix::result::interrupted) {
                using clock_duration = decltype(std::chrono::high_resolution_clock::now() - begin);
                auto const min_val = std::min<std::common_type_t<clock_duration, decltype(d)>>(
                    (std::chrono::high_resolution_clock::now() - begin), d);

                d -= min_val;
                continue;
            }

            throw_system_error(r);
        }
    }

    void wait() {
        int32_t val = gcc::atomic_load(remaining_, std::memory_order_relaxed);
        while (val > 0) {
            auto const r = posix::futex_wait(remaining_, &val);
            if (r == posix::result::success) {
                return;
            }
            if (r == posix::result::interrupted) {
                continue;
            }

            throw_system_error(r);
        }
    }

    void count_down(uint32_t n = 1) {
        int32_t val = gcc::atomic_fetch_sub(remaining_, n, std::memory_order_acq_rel);
        if (val <= n) {
            posix::futex_notify_all(remaining_);
        }
    }

    bool try_wait() const noexcept {
        return gcc::atomic_load(&remaining_, std::memory_order_relaxed) <= 0;
    }

private:
    uint32_t remaining_;
};

} // namespace utl::experimental
