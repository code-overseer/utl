// Copyright 2023-2024 Bryan Wong

#include "utl/tempus/utl_clock.h"

#if UTL_TARGET_MICROSOFT

#  define NOMINMAX
#  define NODRAWTEXT
#  define NOGDI
#  define NOBITMAP
#  define NOMCX
#  define NOSERVICE
#  define NOHELP
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif

#  include <Windows.h>
#  include <intrin.h>
#  include <processthreadsapi.h>
#  include <string.h>
#  include <sysinfoapi.h>

#  if UTL_COMPILER_MSVC
#    pragma intrinsic(memcpy)
#  endif

UTL_NAMESPACE_BEGIN
namespace tempus {

auto time_point<system_clock_t>::get_time() noexcept -> value_type {
    static_assert(sizeof(FILETIME) == sizeof(value_type), "Invalid implementation");
    value_type out_value;
    FILETIME f;
    GetSystemTimeAsFileTime(&f);
    ::memcpy(&out_value, f, 8);
    return out_value;
}

auto time_point<thread_clock_t>::get_time() noexcept -> value_type {
    static_assert(sizeof(FILETIME) == sizeof(value_type), "Invalid implementation");
    FILETIME kernel;
    FILETIME user;
    if (GetThreadTimes(GetCurrentThread(), nullptr, nullptr, &kernel, &user)) {
        value_type k, u;
        ::memcpy(&k, f, 8);
        ::memcpy(&u, f, 8);
        return k + u;
    }

    return static_cast<value_type>(-1);
}

auto time_point<process_clock_t>::get_time() noexcept -> value_type {
    static_assert(sizeof(FILETIME) == sizeof(value_type), "Invalid implementation");
    FILETIME kernel;
    FILETIME user;
    if (GetProcessTimes(GetCurrentProcess(), nullptr, nullptr, &kernel, &user)) {
        value_type k, u;
        ::memcpy(&k, f, 8);
        ::memcpy(&u, f, 8);
        return k + u;
    }

    return static_cast<value_type>(-1);
}

auto time_point<steady_clock_t>::get_time() noexcept -> value_type {
    return GetTickCount64();
}

auto time_point<high_resolution_clock_t>::get_time() noexcept -> value_type {
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    return ticks.QuadPart;
}

auto time_point<high_resolution_clock_t>::difference(value_type l, value_type r) noexcept
    -> duration {
    static value_type const frequency = []() {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f.QuadPart;
    }();
    static value_type const ns_per_tick = value_type(1000000000) / frequency;
    auto const diff = l - r;
    return duration{0, diff * ns_per_tick};
}

} // namespace tempus

UTL_NAMESPACE_END

#endif // UTL_TARGET_MICROSOFT
