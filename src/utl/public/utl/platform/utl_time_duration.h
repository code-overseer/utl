// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/chrono/utl_chrono_fwd.h"

#include "utl/concepts/utl_same_as.h"
#include "utl/platform/utl_time_duration.h"
#include "utl/type_traits/utl_is_same.h"

struct timespec;

UTL_NAMESPACE_BEGIN

namespace platform {
class time_duration {
    static constexpr size_t nano_divisor = size_t(1000000000);
    struct construct_t {};

    template <typename R, typename P>
    time_duration from_chrono(::std::chrono::duration<R, P> const& t) noexcept {
        using nano_duration = ::std::chrono::duration<long long, ::std::ratio<1, 1000000000>>;
        auto const time = ::std::chrono::duration_cast<nano_duration>(t).count();
        using time_type = decltype(time);
        static constexpr time_type divisor = time_type(1000000000);
        auto const seconds = time / divisor;
        return time_duration{construct_t{}, seconds, time - seconds * divisor};
    }

    constexpr time_duration adjust(size_t s, size_t ns) noexcept {
        auto const extra = ns / nano_divisor;
        return time_duration{construct_t{}, s + extra, ns - extra * nano_divisor};
    }

    constexpr time_duration(construct_t, size_t seconds, size_t nanoseconds) noexcept
        : seconds_(seconds)
        , nanoseconds_(nanoseconds) {}

public:
    template <typename R, typename P>
    explicit time_duration(::std::chrono::duration<R, P> const& t) noexcept
        : time_duration(from_chrono(t)) {}
    constexpr explicit time_duration(size_t seconds, size_t nanoseconds = 0) noexcept
        : time_duration(adjust(seconds, nanoseconds)) {}
    constexpr explicit time_duration() noexcept : seconds_(0), nanoseconds_(0) {}

    template <UTL_CONCEPT_CXX20(same_as<::timespec>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(T, ::timespec))>
    constexpr operator T() const noexcept {
        return {(decltype(T::tv_sec))seconds_, (decltype(T::tv_nsec))nanoseconds_};
    }

    UTL_ATTRIBUTES(PURE, ALWAYS_INLINE) size_t seconds() const { return seconds_; }
    UTL_ATTRIBUTES(PURE, ALWAYS_INLINE) size_t nanoseconds() const { return nanoseconds_; }

private:
    size_t seconds_;
    size_t nanoseconds_;
};
} // namespace platform

template <UTL_CONCEPT_CXX20(same_as<::timespec>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(T, ::timespec))>
UTL_ATTRIBUTES(CONST, ALWAYS_INLINE) bool in_range(platform::time_duration d) noexcept {
    return in_range<decltype(T::tv_sec)>(d.seconds()) &&
        in_range<decltype(T::tv_nsec)>(d.nanoseconds());
}

UTL_NAMESPACE_END
