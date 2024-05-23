// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX14 && UTL_USE_STD_move
#  include <utility>

UTL_NAMESPACE_BEGIN

using std::move;
using std::move_if_noexcept;

UTL_NAMESPACE_END

#else // UTL_CXX14 && defined(UTL_USE_STD_move)

#  if UTL_USE_STD_move
UTL_PRAGMA_WARN(
    "The current standard does not implement a constexpr move, `UTL_USE_STD_move` ignored")
#  endif

UTL_NAMESPACE_BEGIN

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC)
constexpr remove_reference_t<T>&& move(T&& t UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return static_cast<remove_reference_t<T>&&>(t);
}

namespace details {
namespace utility {
template <typename T>
using move_if_noexcept_result_t =
    conditional_t<!is_nothrow_move_constructible<T>::value && is_copy_constructible<T>::value,
        T const&, T&&>;

} // namespace utility
} // namespace details

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC)
constexpr details::utility::move_if_noexcept_result_t<T> move_if_noexcept(
    T& t UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return move(t);
}

UTL_NAMESPACE_END

#endif // UTL_CXX14 && UTL_USE_STD_move
