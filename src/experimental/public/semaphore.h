#pragma once
#include "gcc/atomics.h"
#include "posix/futex.h"
#include "utl_config.h"

#include <atomic>

namespace utl::experimental {

class semaphore {
    static [[noreturn]] void throw_system_error(posix::result r) {
        char error_msg[1024];
        sprintf(error_msg, "Internal semaphore error: %s", posix::to_string(r));
        throw std::system_error(error_msg);
    }

public:
    explicit semaphore(int32_t n) : current_(n) {}
    semaphore(semaphore const&) = delete;
    semaphore& operator=(semaphore const&) = delete;

    template <typename R, typename P>
    bool wait_for(std::chrono::duration<R, P> d) {
        uint32_t val = gcc::atomic_load(remaining_, std::memory_order_relaxed);
        if (val > 0 && try_decrease(&val)) {
            return true;
        }

        do {
            auto const begin = std::chrono::high_resolution_clock::now();

            UTL_ON_SCOPE_EXIT {
                using clock_duration = decltype(std::chrono::high_resolution_clock::now() - begin);
                auto const min_val = std::min<std::common_type_t<clock_duration, decltype(d)>>(
                    (std::chrono::high_resolution_clock::now() - begin), d);

                d -= min_val;
            };

            auto const r = posix::futex_wait(current_, &val, d);
            if (r == posix::result::success) {
                if (try_decrease(&val)) {
                    return true;
                }

                continue;
            }

            if (r == posix::result::timeout) {
                return false;
            }
            if (r == posix::result::interrupted) {
                continue;
            }

            throw_system_error(r);
        } while (val == 0);
    }

    void wait() {
        int32_t val = 0;
        while (val == 0) {
            auto const r = posix::futex_wait(current_, &val, d);
            if (r == posix::result::success) {
                if (try_decrease(&val)) {
                    return;
                }

                continue;
            }

            if (r == posix::result::interrupted) {
                continue;
            }

            throw_system_error(r);
        }
    }

    void signal() {
        gcc::atomic_fetch_add(current_, 1, std::memory_order_acq_rel);
        posix::futex_notify_one(current_);
    }

private:
    bool try_decrease(uint32_t* current_val) {
        auto& val = *current_val;
        auto new_val = val - 1;
        while (!gcc::compare_exchange(
            current_, &val, new_val, std::memory_order_release, std::memory_order_relaxed)) {

            if (val == 0) {
                return false;
            }

            new_val = val - 1;
        }

        gcc::atomic_thread_fence(std::memory_order_acquire);
        return true;
    }

    uint32_t current_;
};

} // namespace utl::experimental
