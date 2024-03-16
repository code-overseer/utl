// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_pragma.h"
#include "utl/preprocessor/utl_standard.h"

#if defined(UTL_CXX23) && defined(UTL_USE_STD_exchange) && UTL_USE_STD_exchange

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::exchange

    UTL_NAMESPACE_END

#else // defined(UTL_CXX23) && defined(UTL_USE_STD_exchange) && UTL_USE_STD_exchange

#  if defined(UTL_USE_STD_exchange) && UTL_USE_STD_exchange
UTL_PRAGMA_WARN(
    "The current standard does not implement constexpr exchange, `UTL_USE_STD_exchange` ignored")
#    undef UTL_USE_STD_exchange
#  endif // defined(UTL_USE_STD_exchange) && UTL_USE_STD_exchange

#  include "utl/type_traits/assignment_traits/utl_is_assignable.h"
#  include "utl/type_traits/assignment_traits/utl_is_nothrow_assignable.h"
#  include "utl/type_traits/constructor_traits/utl_is_move_constructible.h"
#  include "utl/type_traits/constructor_traits/utl_is_nothrow_move_constructible.h"
#  include "utl/utility/utl_forward.h"
#  include "utl/utility/utl_move.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename U = T>
constexpr enable_if_t<conjunction<is_move_constructible<T>, is_assignable<T&, U>>::value, T>
exchange(T& obj, U&& new_value) noexcept(
    is_nothrow_move_constructible<T>::value && is_nothrow_assignable<T&, U>::value) {
    T previous(move(obj));
    obj = forward<U>(new_value);
    return previous;
}

UTL_NAMESPACE_END

#endif // defined(UTL_CXX23) && defined(UTL_USE_STD_exchange) && UTL_USE_STD_exchange
