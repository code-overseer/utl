// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/source_location/utl_source_location.h"

UTL_NAMESPACE_BEGIN

namespace exceptions {
struct message_format {
    constexpr message_format&& forward(message_format&& src) noexcept {
        return static_cast<message_format&&>(src);
    }
#if UTL_COMPILER_SUPPORTS_SOURCE_LOCATION
    template <size_t N>
    message_format(char const (&fmt)[N] UTL_ATTRIBUTE(LIFETIMEBOUND),
        UTL_SCOPE source_location src = UTL_SCOPE source_location::current()) noexcept
        : format(fmt)
        , location(src) {}
#else
    template <size_t N>
    message_format(char const (&fmt)[N] UTL_ATTRIBUTE(LIFETIMEBOUND), UTL_SCOPE source_location src) noexcept
        : format(fmt)
        , location(src) {}
#endif
    char const* format;
    UTL_SCOPE source_location location;
};
} // namespace exceptions

UTL_NAMESPACE_END

#define UTL_MESSAGE_FORMAT(FORMAT) \
    UTL_SCOPE exceptions::message_format::forward({FORMAT, UTL_SOURCE_LOCATION()})
