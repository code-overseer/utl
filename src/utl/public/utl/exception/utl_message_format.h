// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/source_location/utl_source_location.h"

UTL_NAMESPACE_BEGIN

namespace exceptions {
/**
 * @brief Intermediate converter for message formatting and source location tracking.
 *
 * This struct facilitates the handling of message formatting with integrated support for
 * source location information. On compilers that fully support source location intrinsics,
 * string literals can be implicitly converted to a message_vformat, with the default source
 * location argument providing the precise location of the conversion site. For compilers
 * that do not fully support these intrinsics, the `UTL_MESSAGE_FORMAT` macro is available,
 * which uses preprocessor tokens to construct a source location.
 *
 * Use `UTL_COMPILER_SUPPORTS_SOURCE_LOCATION` to check for source location intrinsic support
 */
struct __UTL_ABI_PUBLIC message_vformat {
    /**
     * Internal use only to prevent assignment
     */
    __UTL_HIDE_FROM_ABI static constexpr message_vformat&& forward(
        message_vformat&& src UTL_LIFETIMEBOUND) noexcept {
        return static_cast<message_vformat&&>(src);
    }
#if UTL_COMPILER_SUPPORTS_SOURCE_LOCATION
    /**
     * @brief Constructs a message_vformat object with format string and source location.
     *
     * This constructor initializes the message_vformat with a format string and
     * optionally with source location information if supported by the compiler.
     *
     * @tparam N - The size of the byte array containing the format string.
     * @param fmt - The format string.
     * @param src - The source location information, defaulting to the current location.
     */
    template <size_t N>
    __UTL_HIDE_FROM_ABI message_vformat(char const (&fmt)[N] UTL_LIFETIMEBOUND,
        __UTL source_location src = UTL_SOURCE_LOCATION()) noexcept
        : format(fmt)
        , location(src) {}
#else
    /**
     * @brief Constructs a message_vformat object with format string and source location.
     *
     * This constructor initializes the message_vformat with a format string and
     * source location information.
     *
     * @tparam N - The size of the byte array containing the format string.
     * @param fmt - The format string.
     * @param src - The source location information.
     */
    template <size_t N>
    __UTL_HIDE_FROM_ABI message_vformat(
        char const (&fmt)[N] UTL_LIFETIMEBOUND, __UTL source_location src) noexcept
        : format(fmt)
        , location(src) {}
#endif
    char const* format;
    __UTL source_location location;
};
} // namespace exceptions

UTL_NAMESPACE_END

#define UTL_MESSAGE_FORMAT(FORMAT) \
    __UTL exceptions::message_vformat::forward({FORMAT, UTL_SOURCE_LOCATION()})
