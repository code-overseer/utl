// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/utility/utl_forward.h"
#  include "utl/utility/utl_swap.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept swappable = requires(T& l, T& r) { UTL_SCOPE utility::swap(l, r); };

template <typename T, typename U>
concept swappable_with = requires(T&& t, U&& u) {
    UTL_SCOPE utility::swap(UTL_SCOPE forward<T>(t), UTL_SCOPE forward<T>(t));
    UTL_SCOPE utility::swap(UTL_SCOPE forward<U>(u), UTL_SCOPE forward<U>(u));
    UTL_SCOPE utility::swap(UTL_SCOPE forward<T>(t), UTL_SCOPE forward<U>(u));
    UTL_SCOPE utility::swap(UTL_SCOPE forward<U>(u), UTL_SCOPE forward<T>(t));
};

UTL_NAMESPACE_END

#endif
