// Copyright 2023-2024 Bryan Wong

#include "utl/tempus/utl_clock.h"

#if UTL_TARGET_APPLE
#  include "utl/scope/utl_scope_exit.h"

#  include <mach/clock.h>
#  include <mach/mach.h>
#  include <mach/mach_time.h>
#  include <sys/time.h>

UTL_NAMESPACE_BEGIN

namespace tempus {

auto clock_traits<steady_clock_t>::difference(value_type l, value_type r) noexcept
    -> duration_type {
    static mach_timebase_info_data_t const timebase = []() {
        mach_timebase_info_data_t t;
        mach_timebase_info(&t);
        return t;
    }();

    auto const diff = (l - r) * timebase.numer / timebase.denom;
    using diff_type = long long;
    return duration_type{0, diff_type(diff)};
}

auto clock_traits<thread_clock_t>::get_time() noexcept -> value_type {
    mach_port_t thread = mach_thread_self();
    UTL_ON_SCOPE_EXIT {
        mach_port_deallocate(mach_task_self(), thread);
    };

    mach_msg_type_number_t count = THREAD_BASIC_INFO_COUNT;
    thread_basic_info_data_t info;
    auto const result = thread_info(thread, THREAD_BASIC_INFO, (thread_info_t)&info, &count);

    if (result != KERN_SUCCESS) {
        return -1;
    }

    ::timeval const user{info.user_time.seconds, info.user_time.microseconds};
    ::timeval const kernel{info.system_time.seconds, info.system_time.microseconds};
    auto const user_time = details::timeval_to_usec(user);
    auto const kernel_time = details::timeval_to_usec(kernel);
    return user_time + kernel_time;
}

} // namespace tempus

UTL_NAMESPACE_END

#endif
