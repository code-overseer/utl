// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/tempus/utl_clock_fwd.h"

#include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

namespace tempus {

/**
 * @class hardware_ticks
 * @brief Represents the ticks between hardware counters.
 *
 * The `hardware_ticks` class is used to represent the number of ticks between hardware counters.
 * Depending on the hardware and platform, the accuracy of these ticks may vary. This class provides
 * methods to initialize, retrieve, and check the validity of hardware ticks.
 */
class __UTL_ABI_PUBLIC hardware_ticks {
public:
    /**
     * @brief Checks if ticks can be converted to a duration.
     *
     * Determines whether the ticks can be converted to a time duration based on the
     * hardware and platform.
     *
     * @return `true` if ticks can be converted to a duration, `false` otherwise.
     */
    UTL_ATTRIBUTES(_ABI_PUBLIC, PURE) static bool invariant_frequency() noexcept;

    /**
     * @brief Returns an invalid `hardware_ticks` object.
     *
     * This static method returns a `hardware_ticks` object that represents an invalid or unusable
     * state. The returned object has a tick value of -1.
     *
     * @return A `hardware_ticks` object with a value of -1, indicating an invalid state.
     */
    static constexpr hardware_ticks invalid() noexcept { return hardware_ticks(-1); }

    /**
     * @brief Default constructor for `hardware_ticks`.
     *
     * Initializes a `hardware_ticks` object with a default value of 0.
     */
    constexpr explicit hardware_ticks() noexcept : ticks_(0) {}

    /**
     * @brief Constructor for `hardware_ticks` with a specific tick value.
     *
     * Initializes a `hardware_ticks` object with the specified tick value.
     *
     * @param t The tick value to initialize the `hardware_ticks` object with.
     */
    constexpr explicit hardware_ticks(long long t) noexcept : ticks_(t) {}

    /**
     * @brief Retrieves the tick value.
     *
     * Returns the current tick value stored in the `hardware_ticks` object.
     *
     * @return The tick value as a `long long`.
     */
    __UTL_HIDE_FROM_ABI constexpr long long value() const noexcept { return ticks_; }

    UTL_ATTRIBUTES(PURE, ALWAYS_INLINE) explicit constexpr operator bool() const noexcept {
        return ticks_ >= 0;
    }

    constexpr bool operator==(hardware_ticks const& other) const noexcept {
        return ticks_ == other.ticks_ && ticks_ >= 0;
    }

    constexpr bool operator!=(hardware_ticks const& other) const noexcept {
        return *this && other && !(*this == other);
    }

    constexpr bool operator<(hardware_ticks const& other) const noexcept {
        return *this && other && ticks_ < other.ticks_;
    }

    constexpr bool operator>(hardware_ticks const& other) const noexcept { return other < *this; }

    constexpr bool operator<=(hardware_ticks const& other) const noexcept {
        return *this && other && !(other < *this);
    }

    constexpr bool operator>=(hardware_ticks const& other) const noexcept {
        return *this && other && !(*this < other);
    }

#if UTL_CXX20
    template <same_as<hardware_ticks> T,
        same_as<::std::partial_ordering> R = ::std::partial_ordering>
    constexpr R operator<=>(T const& other) const noexcept {
        return *this && other ? ticks_ <=> other.ticks_ : R::unordered;
    }
#endif

private:
    /** Stores the tick value. */
    long long ticks_;
};

/**
 * @brief Converts `hardware_ticks` to `time_duration`.
 *
 * Converts the given `hardware_ticks` object to a `time_duration`. The conversion depends on the
 * specific implementation and the hardware's timekeeping capabilities. This function may terminate
 * the program on unsupported platforms; use `invariant_frequency` to check for support.
 *
 * @param ht The `hardware_ticks` object to convert.
 * @return The corresponding `time_duration`.
 */
UTL_ATTRIBUTES(_ABI_PUBLIC) time_duration to_time_duration(hardware_ticks) noexcept;
} // namespace tempus

UTL_NAMESPACE_END
