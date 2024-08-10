// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX23 && UTL_USE_STD_exchange

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::exchange;

UTL_NAMESPACE_END

#else // UTL_CXX23 && UTL_USE_STD_exchange

#  if UTL_USE_STD_exchange
UTL_PRAGMA_WARN(
    "The current standard does not implement constexpr exchange, `UTL_USE_STD_exchange` ignored")
#    undef UTL_USE_STD_exchange
#  endif // UTL_USE_STD_exchange

#  include "utl/concepts/utl_assignable_to.h"
#  include "utl/concepts/utl_move_constructible.h"
#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_assignable.h"
#  include "utl/type_traits/utl_is_move_constructible.h"
#  include "utl/type_traits/utl_is_nothrow_assignable.h"
#  include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#  include "utl/utility/utl_forward.h"
#  include "utl/utility/utl_move.h"

UTL_NAMESPACE_BEGIN

#  if UTL_COMPILER_CLANG_AT_LEAST(18, 0, 0)
template <UTL_CONCEPT_CXX20(move_constructible) T, UTL_CONCEPT_CXX20(assignable_to<T&>) U = T>
UTL_HIDE_FROM_ABI constexpr UTL_ENABLE_IF_CXX11(T, UTL_TRAIT_is_move_constructible(T) && UTL_TRAIT_is_assignable(T&, U)) exchange(T& obj, U&& new_value) noexcept(
    UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_assignable(T&, U)) {
    T previous(UTL_SCOPE move(obj));
    obj = UTL_SCOPE forward<U>(new_value);
    return previous;
}
#  else
template <UTL_CONCEPT_CXX20(move_constructible) T, UTL_CONCEPT_CXX20(assignable_to<T&>) U = T>
UTL_HIDE_FROM_ABI constexpr UTL_ENABLE_IF_CXX11(
    T, UTL_TRAIT_is_move_constructible(T) && UTL_SCOPE is_assignable<T&, U>::value)
    exchange(T& obj, U&& new_value) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_assignable(T&, U)) {
    T previous(UTL_SCOPE move(obj));
    obj = UTL_SCOPE forward<U>(new_value);
    return previous;
}
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX23 && UTL_USE_STD_exchange
