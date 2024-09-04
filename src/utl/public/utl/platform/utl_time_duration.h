// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/chrono/utl_chrono_fwd.h"

#include "utl/compare/utl_partial_ordering.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/platform/utl_time_duration.h"
#include "utl/type_traits/utl_is_same.h"

struct timespec;

UTL_NAMESPACE_BEGIN

namespace platform {
class time_duration {
    static constexpr long long nano_divisor = 1000000000ll;
    struct construct_t {};

    template <typename R, typename P>
    __UTL_HIDE_FROM_ABI static constexpr time_duration from_chrono(
        ::std::chrono::duration<R, P> const& t) noexcept {
        using nano_ratio = ::std::ratio<1, nano_divisor>;
        using nano_duration = ::std::chrono::duration<long long, nano_ratio>;
        auto const time = ::std::chrono::duration_cast<nano_duration>(t).count();
        auto const divisor = 1000000000ll;
        auto const seconds = time / divisor;
        return {construct_t{}, seconds, time - seconds * divisor};
    }

    static constexpr time_duration adjust(long long s, long long ns) noexcept {
        auto const extra = ns / nano_divisor;
        return {construct_t{}, s + extra, ns - extra * nano_divisor};
    }

    constexpr time_duration(construct_t, long long seconds, long long nanoseconds) noexcept
        : seconds_(seconds)
        , nanoseconds_(nanoseconds) {}

public:
    static constexpr time_duration invalid() noexcept { return {construct_t{}, -1, -1}; }

    template <typename R, typename P>
    __UTL_HIDE_FROM_ABI explicit time_duration(::std::chrono::duration<R, P> const& t) noexcept
        : time_duration(from_chrono(t)) {}
    constexpr explicit time_duration(long long seconds, long long nanoseconds = 0) noexcept
        : time_duration(adjust(seconds, nanoseconds)) {}
    constexpr explicit time_duration() noexcept : seconds_(0), nanoseconds_(0) {}

    template <UTL_CONCEPT_CXX20(same_as<::timespec>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(T, ::timespec))>
    constexpr operator T() const noexcept {
        return {(decltype(T::tv_sec))seconds_, (decltype(T::tv_nsec))nanoseconds_};
    }

    UTL_ATTRIBUTES(PURE, ALWAYS_INLINE) constexpr long long seconds() const { return seconds_; }
    UTL_ATTRIBUTES(PURE, ALWAYS_INLINE) constexpr long long nanoseconds() const { return nanoseconds_; }

    UTL_ATTRIBUTES(PURE, ALWAYS_INLINE) explicit constexpr operator bool() const noexcept {
        return seconds_ < 0 && (seconds_ == nanoseconds_);
    }

    constexpr bool operator==(time_duration const& other) const noexcept {
        return *this && other && seconds_ == other.seconds_ && nanoseconds_ == other.nanoseconds_;
    }

#if UTL_CXX20

    constexpr __UTL partial_ordering operator<=>(time_duration const& other) const noexcept {
        if (!*this || !other) {
            return __UTL partial_ordering::unordered;
        }

        auto const s = seconds_ <=> other.seconds_;
        return s != 0 ? s : nanoseconds_ <=> other.nanoseconds_;
    }

#else

    constexpr bool operator!=(time_duration const& other) const noexcept {
        return *this && other && !(*this == other);
    }

    constexpr bool operator<(time_duration const& other) const noexcept {
        return *this && other &&
            (seconds_ < other.seconds_ ||
                (seconds_ == other.seconds_ && nanoseconds_ < other.nanoseconds_));
    }

    constexpr bool operator>(time_duration const& other) const noexcept { return other < *this; }

    constexpr bool operator<=(time_duration const& other) const noexcept {
        return *this && other && !(other < *this);
    }

    constexpr bool operator>=(time_duration const& other) const noexcept {
        return *this && other && !(*this < other);
    }

#endif

private:
    long long seconds_;
    long long nanoseconds_;
};
} // namespace platform

template <UTL_CONCEPT_CXX20(same_as<::timespec>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(T, ::timespec))>
UTL_ATTRIBUTES(CONST, ALWAYS_INLINE) bool in_range(platform::time_duration d) noexcept {
    return in_range<decltype(T::tv_sec)>(d.seconds()) &&
        in_range<decltype(T::tv_nsec)>(d.nanoseconds());
}

UTL_NAMESPACE_END
