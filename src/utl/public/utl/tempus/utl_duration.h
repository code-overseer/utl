// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/chrono/utl_chrono_fwd.h"
#include "utl/compare/utl_compare_fwd.h"

#include "utl/concepts/utl_same_as.h"
#include "utl/numeric/utl_add_sat.h"
#include "utl/numeric/utl_sub_sat.h"
#include "utl/tempus/utl_duration.h"
#include "utl/type_traits/utl_default_constant.h"
#include "utl/type_traits/utl_is_same.h"

#include <stdint.h>

struct timespec;

UTL_NAMESPACE_BEGIN

namespace tempus {
class __UTL_ABI_PUBLIC duration {
    static constexpr auto nano_multiple = 1000000000ull;
    static constexpr auto invalid_value = static_cast<uint64_t>(-1);
    struct direct_tag {};
    static constexpr uint64_t reinterpret(duration const& d) noexcept {
        return (d.seconds_ << 30) | d.nanoseconds_;
    }

    static constexpr duration adjust_ns(int64_t total_ns) noexcept {
        return total_ns >= 0 ? duration{UTL_TRAIT_default_constant(direct_tag),
                                   total_ns / nano_multiple, total_ns % nano_multiple}
                             : invalid();
    }

    static constexpr duration adjust(int64_t s, int64_t ns) noexcept {
        return adjust_ns(s * nano_multiple + ns);
    }

    template <typename R, typename P>
    __UTL_HIDE_FROM_ABI static UTL_CONSTEXPR_CXX14 duration from_chrono(
        ::std::chrono::duration<R, P> const& t) noexcept {
        using nano_ratio = ::std::ratio<1, nano_multiple>;
        using nano_duration = ::std::chrono::duration<int64_t, nano_ratio>;
        auto const time = ::std::chrono::duration_cast<nano_duration>(t).count();
        return adjust(0, time);
    }

    constexpr duration(direct_tag, uint64_t seconds, uint64_t nanoseconds) noexcept
        : nanoseconds_(nanoseconds)
        , seconds_(seconds) {}

public:
    static constexpr duration invalid() noexcept {
        return {UTL_TRAIT_default_constant(direct_tag), invalid_value, invalid_value};
    }

    template <typename R, typename P>
    __UTL_HIDE_FROM_ABI explicit UTL_CONSTEXPR_CXX14 duration(::std::chrono::duration<R, P> const& t) noexcept
        : duration(from_chrono(t)) {}
    constexpr explicit duration(int64_t seconds, int64_t nanoseconds = 0) noexcept
        : duration(adjust(seconds, nanoseconds)) {}
    constexpr explicit duration() noexcept : nanoseconds_(0), seconds_(0) {}

    template <typename R, typename P>
    __UTL_HIDE_FROM_ABI explicit UTL_CONSTEXPR_CXX14 operator ::std::chrono::duration<R, P>() const noexcept {
        using result_type = ::std::chrono::duration<R, P>;
        using nano_ratio = ::std::ratio<1, nano_multiple>;
        // defer instantiation
        using nano_duration = enable_if_t<__UTL always_true<result_type>(),
            ::std::chrono::duration<int64_t, nano_ratio>>;
        return ::std::chrono::duration_cast<result_type>(
            nano_duration(seconds_ * nano_multiple + nanoseconds_));
    }

    template <UTL_CONCEPT_CXX20(same_as<::timespec>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(T, ::timespec))>
    __UTL_HIDE_FROM_ABI constexpr operator T() const noexcept {
        return {(decltype(T::tv_sec))seconds_, (decltype(T::tv_nsec))nanoseconds_};
    }

    UTL_ATTRIBUTES(ALWAYS_INLINE) constexpr uint64_t seconds() const { return seconds_; }
    UTL_ATTRIBUTES(ALWAYS_INLINE) constexpr uint32_t nanoseconds() const { return nanoseconds_; }

    UTL_ATTRIBUTES(ALWAYS_INLINE) explicit constexpr operator bool() const noexcept {
        return reinterpret(*this) != invalid_value;
    }

    constexpr duration operator-(duration const& other) const noexcept {
        return *this && other
            ? duration{0,
                  static_cast<int64_t>(__UTL sub_sat(seconds_ * nano_multiple + nanoseconds_,
                      other.seconds_ * nano_multiple + other.nanoseconds_))}
            : invalid();
    }

    constexpr duration operator+(duration const& other) const noexcept {
        return *this && other
            ? duration{0,
                  static_cast<int64_t>(__UTL add_sat(seconds_ * nano_multiple + nanoseconds_,
                      other.seconds_ * nano_multiple + other.nanoseconds_))}
            : invalid();
    }

    constexpr bool operator==(duration const& other) const noexcept {
        return reinterpret(*this) == reinterpret(other) && *this;
    }

    constexpr bool operator!=(duration const& other) const noexcept {
        return *this && other && reinterpret(*this) != reinterpret(other);
    }

    constexpr bool operator<(duration const& other) const noexcept {
        return *this && other && reinterpret(*this) < reinterpret(other);
    }

    constexpr bool operator>(duration const& other) const noexcept {
        return *this && other && reinterpret(*this) > reinterpret(other);
    }

    constexpr bool operator<=(duration const& other) const noexcept {
        return *this && other && reinterpret(*this) <= reinterpret(other);
    }

    constexpr bool operator>=(duration const& other) const noexcept {
        return *this && other && reinterpret(*this) >= reinterpret(other);
    }

#if UTL_CXX20

    template <same_as<duration> D, same_as<::std::partial_ordering> R = ::std::partial_ordering>
    __UTL_HIDE_FROM_ABI constexpr R operator<=>(D const& other) const noexcept {
        if (!*this || !other) {
            return R::unordered;
        }

        return reinterpret(*this) <=> reinterpret(other);
    }

#endif

private:
    using value_type = uint64_t;
    value_type nanoseconds_ : 30;
    value_type seconds_ : 34;
};
static_assert(sizeof(duration) == sizeof(uint64_t), "Invalid implementation");
} // namespace tempus

template <UTL_CONCEPT_CXX20(same_as<::timespec>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(T, ::timespec))>
UTL_ATTRIBUTES(CONST, ALWAYS_INLINE) bool in_range(tempus::duration d) noexcept {
    return in_range<decltype(T::tv_sec)>(d.seconds());
}

UTL_NAMESPACE_END