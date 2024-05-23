// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX14 && UTL_USE_STD_forward

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::forward;

UTL_NAMESPACE_END

#else // UTL_CXX14 && UTL_USE_STD_forward

#  if UTL_USE_STD_forward
UTL_PRAGMA_WARN(
    "The current standard does not implement a constexpr forward, `UTL_USE_STD_forward` ignored")
#  endif // UTL_USE_STD_forward

#  include "utl/type_traits/utl_is_lvalue_reference.h"
#  include "utl/type_traits/utl_remove_reference.h"

UTL_NAMESPACE_BEGIN

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC)
constexpr T&& forward(remove_reference_t<T>& t UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return static_cast<T&&>(t);
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC)
constexpr T&& forward(remove_reference_t<T>&& t UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    static_assert(!is_lvalue_reference<T>::value, "cannot forward an rvalue as an lvalue");
    return static_cast<T&&>(t);
}

UTL_NAMESPACE_END

#endif // UTL_CXX14 && UTL_USE_STD_forward

#if UTL_CXX23 && UTL_USE_STD_forward_like

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::forward_like;

UTL_NAMESPACE_END

#else // UTL_CXX23 && UTL_USE_STD_forward_like

#  include "utl/type_traits/utl_add_rvalue_reference.h"
#  include "utl/type_traits/utl_copy_cvref.h"
#  include "utl/type_traits/utl_remove_reference.h"

#  if UTL_USE_STD_forward_like
UTL_PRAGMA_WARN(
    "The current standard does not implement forward_like, `UTL_USE_STD_forward_like` ignored")
#  endif // UTL_USE_STD_forward_like

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC)
constexpr auto forward_like(U&& u UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept
    -> add_rvalue_reference_t<copy_cvref_t<T, remove_reference_t<U>>> {
    return static_cast<add_rvalue_reference_t<copy_cvref_t<T, remove_reference_t<U>>>>(u);
}

UTL_NAMESPACE_END
#endif   // UTL_CXX23 && UTL_USE_STD_forward_like
