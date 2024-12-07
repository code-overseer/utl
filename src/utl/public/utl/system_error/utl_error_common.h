// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace error_code {

template <typename T>
class category_compare {
protected:
    UTL_CONSTEXPR_CXX20 ~category_compare() = default;
#if UTL_CXX20
    __UTL_HIDE_FROM_ABI inline constexpr auto operator<=>(T const& rhs) const noexcept {
        return this <=> __UTL addressof(rhs);
    }
#endif
};

template <typename T>
class error_compare {
protected:
    UTL_CONSTEXPR_CXX20 ~error_compare() = default;
#if UTL_CXX20
    __UTL_HIDE_FROM_ABI friend inline constexpr auto operator<=>(
        T const& lhs, T const& rhs) noexcept {
        if (auto result = lhs.category() <=> rhs.category(); result != 0)
            return result;
        return lhs.value() <=> rhs.value();
    }
#endif
};

} // namespace error_code
} // namespace details

UTL_NAMESPACE_END
