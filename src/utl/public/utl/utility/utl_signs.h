// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_integral.h"
#include "utl/type_traits/utl_make_signed.h"
#include "utl/type_traits/utl_make_unsigned.h"

#if !UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_integral<T>::value, make_unsigned_t<T>> to_unsigned(T val) noexcept {
    return (make_unsigned_t<T>)val;
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_integral<T>::value, make_signed_t<T>> to_signed(T val) noexcept {
    return (make_signed_t<T>)val;
}

UTL_NAMESPACE_END

#else // if !UTL_CXX20

#  include "utl/concepts.h"

UTL_NAMESPACE_BEGIN

template <integral T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr make_unsigned_t<T> to_unsigned(T val) noexcept {
    return (make_unsigned_t<T>)val;
}

template <integral T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr make_unsigned_t<T> to_signed(T val) noexcept {
    return (make_signed_t<T>)val;
}

UTL_NAMESPACE_END

#endif
