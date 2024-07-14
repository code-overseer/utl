// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20 && UTL_USE_STD_cmp_utility

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::cmp_equal;
using std::cmp_greater;
using std::cmp_greater_equal;
using std::cmp_less;
using std::cmp_less_equal;
using std::cmp_not_equal;
using std::in_range;

UTL_NAMESPACE_END

#else // UTL_CXX20 && UTL_USE_STD_cmp_utility

#  include "utl/numeric/utl_limits.h"
#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_integral.h"
#  include "utl/type_traits/utl_is_signed.h"
#  include "utl/type_traits/utl_is_unsigned.h"
#  include "utl/utility/utl_signs.h"

#  if UTL_USE_STD_cmp_utility
UTL_PRAGMA_WARN(
    "The current standard does not implement cmp_x functions, `UTL_USE_STD_cmp_utility` ignored")
#    undef UTL_USE_STD_cmp_utility
#  endif // UTL_USE_STD_cmp_utility

#  if !UTL_CXX17

UTL_NAMESPACE_BEGIN

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_signed<L>::value == is_signed<R>::value, bool> cmp_equal(
    L l, R r) noexcept {
    return l == r;
}

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_signed<L>::value && is_unsigned<R>::value, bool> cmp_equal(
    L l, R r) noexcept {
    return l >= 0 && to_unsigned(l) == r;
}

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_unsigned<L>::value && is_signed<R>::value, bool> cmp_equal(
    L l, R r) noexcept {
    return r >= 0 && to_unsigned(r) == l;
}

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_signed<L>::value == is_signed<R>::value, bool> cmp_less(
    L l, R r) noexcept {
    return l < r;
}

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_signed<L>::value && is_unsigned<R>::value, bool> cmp_less(
    L l, R r) noexcept {
    return l < 0 || to_unsigned(l) < r;
}

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_unsigned<L>::value && is_signed<R>::value, bool> cmp_less(
    L l, R r) noexcept {
    return r >= 0 && l < to_unsigned(r);
}

UTL_NAMESPACE_END

#  elif !UTL_CXX20 // if !UTL_CXX17

UTL_NAMESPACE_BEGIN

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_integral<L>::value && is_integral<R>::value, bool> cmp_equal(
    L l, R r) noexcept {
    if constexpr (is_signed<L>::value == is_signed<R>::value) {
        return l == r;
    } else if constexpr (is_signed<L>::value) {
        return l >= 0 && to_unsigned(l) == r;
    } else {
        return r >= 0 && to_unsigned(r) == l;
    }
}

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_integral<L>::value && is_integral<R>::value, bool> cmp_less(
    L l, R r) noexcept {
    if constexpr (is_signed<L>::value == is_signed<R>::value) {
        return l < r;
    } else if constexpr (is_signed<L>::value) {
        return l < 0 || to_unsigned(l) < r;
    } else {
        return r >= 0 && l < to_unsigned(r);
    }
}

UTL_NAMESPACE_END

#  else // elif !UTL_CXX20

#    include "utl/concepts/utl_integral.h"

UTL_NAMESPACE_BEGIN

template <integral L, integral R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool cmp_equal(L l, R r) noexcept {
    if constexpr (is_signed<L>::value == is_signed<R>::value) {
        return l == r;
    } else if constexpr (is_signed<L>::value) {
        return l >= 0 && to_unsigned(l) == r;
    } else {
        return r >= 0 && to_unsigned(r) == l;
    }
}

template <integral L, integral R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool cmp_less(L l, R r) noexcept {
    if constexpr (is_signed<L>::value == is_signed<R>::value) {
        return l < r;
    } else if constexpr (is_signed<L>::value) {
        return l < 0 || to_unsigned(l) < r;
    } else {
        return r >= 0 && l < to_unsigned(r);
    }
}

UTL_NAMESPACE_END

#  endif // if !UTL_CXX17

#  if !UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_integral<L>::value && is_integral<R>::value, bool> cmp_not_equal(
    L l, R r) noexcept {
    return !cmp_equal(l, r);
}

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_integral<L>::value && is_integral<R>::value, bool> cmp_greater(
    L l, R r) noexcept {
    return cmp_less(r, l);
}

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_integral<L>::value && is_integral<R>::value, bool> cmp_less_equal(
    L l, R r) noexcept {
    return !cmp_less(r, l);
}

template <typename L, typename R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_integral<L>::value && is_integral<R>::value, bool> cmp_greater_equal(
    L l, R r) noexcept {
    return !cmp_less(l, r);
}

template <typename R, typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_integral<T>::value && is_integral<R>::value, bool> in_range(T t) noexcept {
    return cmp_greater_equal(t, UTL_NUMERIC_minimum(R)) &&
        cmp_less_equal(t, UTL_NUMERIC_maximum(R));
}

UTL_NAMESPACE_END

#  else // UTL_CXX20

#    include "utl/concepts/utl_integral.h"

UTL_NAMESPACE_BEGIN

template <integral L, integral R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool cmp_not_equal(L l, R r) noexcept {
    return !cmp_equal(l, r);
}

template <integral L, integral R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool cmp_greater(L l, R r) noexcept {
    return cmp_less(r, l);
}

template <integral L, integral R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool cmp_less_equal(L l, R r) noexcept {
    return !cmp_less(r, l);
}

template <integral L, integral R>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool cmp_greater_equal(L l, R r) noexcept {
    return !cmp_less(l, r);
}

template <integral R, integral T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool in_range(T t) noexcept {
    return cmp_greater_equal(t, UTL_NUMERIC_minimum(R)) &&
        cmp_less_equal(t, UTL_NUMERIC_maximum(R));
}

UTL_NAMESPACE_END

#  endif // UTL_CXX20

#endif // UTL_CXX20 && UTL_USE_STD_cmp_utility
