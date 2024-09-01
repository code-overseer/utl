// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/ratio/utl_ratio_fwd.h"

UTL_STD_NAMESPACE_BEGIN
namespace chrono {
template <typename Rep, typename Period = ::std::ratio<1>>
class duration;

template <typename ToDuration, typename Rep, typename Period>
constexpr ToDuration duration_cast(duration<Rep, Period> const&);
} // namespace chrono
UTL_STD_NAMESPACE_END
