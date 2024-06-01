// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_copy_constructible.h"
#include "utl/concepts/utl_equality_comparable.h"
#include "utl/concepts/utl_move_constructible.h"
#include "utl/concepts/utl_swappable.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/utility/utl_move.h"
#include "utl/utility/utl_swap.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

namespace details {
namespace allocator {

template <typename T>
concept comparable = requires {
    typename T::is_always_equal;
    requires T::is_always_equal::value;
} || (UTL_SCOPE equality_comparable<T> && requires(T const& l, T const& r) {
    { l == r } noexcept;
    { l != r } noexcept;
});
}
} // namespace details

template <typename T>
concept allocator_type =
    copy_constructible<T> && move_constructible<T> && swappable<T> && requires(T t) {
        { T(t) } noexcept;
        { T(UTL_SCOPE move(t)) } noexcept;
    } && is_nothrow_swappable_v<T> && details::allocator::comparable<T>;

UTL_NAMESPACE_END
#endif
