// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/chrono/utl_chrono_fwd.h"
#include "utl/compare/utl_compare_fwd.h"

#include "utl/assert/utl_assert.h"
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

/**
 * @class duration
 * @brief A class that encapsulates a time duration represented by seconds and nanoseconds
 *
 * This class provides a compact representation of time using a fixed bit-width for nanoseconds and
 * seconds. It supports arithmetic operations, comparison, and conversion to and from
 * `std::chrono::duration` and `timespec` types.
 *
 * Duration values must always be positive, any arithmetic/construction that results in negative
 * time will return an invalid value.
 *
 * Simular to std::chrono, a set of literals is defined in utl::tempus_literals using the same
 * convention.
 *
 * ### Bit-Width Details:
 * - 30 bits for storing nanoseconds.
 * - 34 bits for storing seconds, allowing durations of ~544 years
 */
class __UTL_ABI_PUBLIC duration {
    static constexpr auto nanoseconds_bitwidth = 30;
    static constexpr auto seconds_bitwidth = 34;
    static constexpr auto nano_multiple = 1000000000ull;
    static constexpr auto invalid_value = static_cast<uint64_t>(-1);
    struct direct_tag {};
    struct invalid_tag {};

    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) static constexpr uint64_t reinterpret(
        duration const& d) noexcept {
        return (d.seconds_ << 30) | d.nanoseconds_;
    }

    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) static constexpr duration adjust_ns(
        int64_t total_ns) noexcept {
        return total_ns >= 0 ? duration{UTL_TRAIT_default_constant(direct_tag),
                                   total_ns / nano_multiple, total_ns % nano_multiple}
                             : invalid();
    }

    UTL_ATTRIBUTES(ALWAYS_INLINE, _HIDE_FROM_ABI) static constexpr duration adjust(
        int64_t s, int64_t ns) noexcept {
        return adjust_ns(s * nano_multiple + ns);
    }

    template <typename R, typename P>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) static UTL_CONSTEXPR_CXX14 duration from_chrono(
        ::std::chrono::duration<R, P> const& t) noexcept {
        using nano_ratio = ::std::ratio<1, nano_multiple>;
        using nano_duration = ::std::chrono::duration<int64_t, nano_ratio>;
        auto const time = ::std::chrono::duration_cast<nano_duration>(t).count();
        return adjust(0, time);
    }

    __UTL_HIDE_FROM_ABI constexpr duration(
        direct_tag, uint64_t seconds, uint64_t nanoseconds) noexcept
        : nanoseconds_(nanoseconds)
        , seconds_(seconds) {
        UTL_ASSERT(seconds < (1 << (seconds_bitwidth - 1)));
    }

    __UTL_HIDE_FROM_ABI constexpr duration(invalid_tag) noexcept
        : nanoseconds_((1ull << nanoseconds_bitwidth) - 1)
        , seconds_((1ull << seconds_bitwidth) - 1) {}

public:
    __UTL_HIDE_FROM_ABI static constexpr duration invalid() noexcept {
        return {UTL_TRAIT_default_constant(invalid_tag)};
    }

    template <typename R, typename P>
    __UTL_HIDE_FROM_ABI explicit UTL_CONSTEXPR_CXX14 duration(::std::chrono::duration<R, P> const& t) noexcept
        : duration(from_chrono(t)) {}
    __UTL_HIDE_FROM_ABI constexpr explicit duration(
        int64_t seconds, int64_t nanoseconds = 0) noexcept
        : duration(adjust(seconds, nanoseconds)) {}
    __UTL_HIDE_FROM_ABI constexpr explicit duration() noexcept : nanoseconds_(0), seconds_(0) {}

    template <typename R, typename P>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) explicit UTL_CONSTEXPR_CXX14
    operator ::std::chrono::duration<R, P>() const noexcept {
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
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr operator T() const noexcept {
        return {(decltype(T::tv_sec))seconds_, (decltype(T::tv_nsec))nanoseconds_};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr uint64_t seconds() const {
        return seconds_;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr uint32_t nanoseconds() const {
        return nanoseconds_;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline explicit constexpr
    operator bool() const noexcept {
        return reinterpret(*this) != invalid_value;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr duration operator-(
        duration const& other) const noexcept {
        return *this && other
            ? duration{0,
                  static_cast<int64_t>(__UTL sub_sat(seconds_ * nano_multiple + nanoseconds_,
                      other.seconds_ * nano_multiple + other.nanoseconds_))}
            : invalid();
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr duration operator+(
        duration const& other) const noexcept {
        return *this && other
            ? duration{0,
                  static_cast<int64_t>(__UTL add_sat(seconds_ * nano_multiple + nanoseconds_,
                      other.seconds_ * nano_multiple + other.nanoseconds_))}
            : invalid();
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator==(duration const& other) const noexcept {
        return reinterpret(*this) == reinterpret(other) && *this;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator!=(duration const& other) const noexcept {
        return *this && other && reinterpret(*this) != reinterpret(other);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator<(duration const& other) const noexcept {
        return *this && other && reinterpret(*this) < reinterpret(other);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator>(duration const& other) const noexcept {
        return *this && other && reinterpret(*this) > reinterpret(other);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator<=(duration const& other) const noexcept {
        return *this && other && reinterpret(*this) <= reinterpret(other);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr bool operator>=(duration const& other) const noexcept {
        return *this && other && reinterpret(*this) >= reinterpret(other);
    }

#if UTL_CXX20

    template <same_as<duration> D, same_as<::std::partial_ordering> R = ::std::partial_ordering>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr R operator<=>(D const& other) const noexcept {
        if (!*this || !other) {
            return R::unordered;
        }

        return reinterpret(*this) <=> reinterpret(other);
    }

#endif

private:
    using value_type = uint64_t;
    value_type nanoseconds_ : nanoseconds_bitwidth;
    value_type seconds_ : seconds_bitwidth;
};
static_assert(sizeof(duration) == sizeof(uint64_t), "Invalid implementation");
} // namespace tempus

template <UTL_CONCEPT_CXX20(same_as<::timespec>) T UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(T, ::timespec))>
UTL_ATTRIBUTES(CONST, ALWAYS_INLINE, _HIDE_FROM_ABI) bool in_range(tempus::duration d) noexcept {
    return in_range<decltype(T::tv_sec)>(d.seconds());
}

UTL_NAMESPACE_END
