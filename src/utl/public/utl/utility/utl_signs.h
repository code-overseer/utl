// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_pragma.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/type_traits/utl_std_traits.h"

#ifndef UTL_CXX20

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

#else // ifndef UTL_CXX20

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
