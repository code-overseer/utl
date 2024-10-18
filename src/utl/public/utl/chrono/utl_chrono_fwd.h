// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/ratio/utl_ratio_fwd.h"

#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_complete.h"

UTL_STD_NAMESPACE_BEGIN
namespace chrono {
template <typename Rep, typename Period>
class duration;

template <typename ToDuration, typename Rep, typename Period>
constexpr __UTL enable_if_t<!__UTL is_complete<duration<Rep, Period>>::value, ToDuration>
duration_cast(duration<Rep, Period> const&);
} // namespace chrono
UTL_STD_NAMESPACE_END
