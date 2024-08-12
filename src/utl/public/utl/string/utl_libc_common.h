// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/string/utl_is_string_char.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_empty.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_trivially_copyable.h"
#include "utl/type_traits/utl_is_unsigned.h"
#include "utl/type_traits/utl_remove_cv.h"

#include <cstring>

UTL_NAMESPACE_BEGIN
#define __UTL_ATTRIBUTE_LIBC_API (CONST)(NODISCARD)(ALWAYS_INLINE)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_LIBC_API
namespace libc {

enum class element_count_t : size_t {
};

template <typename T>
UTL_ATTRIBUTE(LIBC_API) constexpr size_t byte_count(element_count_t c) noexcept {
    return sizeof(T) * size_t(c);
}

template <typename T>
UTL_ATTRIBUTE(LIBC_API) constexpr bool is_pointer_in_range(
    T const* begin, T const* end, T const* ptr) noexcept {
    return UTL_CONSTANT_P(begin <= end && ptr < end) || (!(ptr < begin) && ptr < end);
}

UTL_ATTRIBUTE(LIBC_API) constexpr element_count_t operator-(element_count_t val, size_t op) noexcept {
    return (element_count_t)((size_t)val - op);
}

UTL_ATTRIBUTE(LIBC_API) constexpr bool operator==(element_count_t val, size_t op) noexcept {
    return (size_t)val == op;
}

template <typename T>
UTL_ATTRIBUTE(LIBC_API) constexpr T* operator+(T* ptr, element_count_t offset) noexcept {
    return ptr + (size_t)offset;
}

template <typename T, typename U>
struct is_trivially_lexicographically_comparable :
    bool_constant<UTL_TRAIT_is_same(remove_cv_t<T>, remove_cv_t<U>) && sizeof(T) == 1 &&
        UTL_TRAIT_is_unsigned(T)> {};

#if UTL_CXX20
template <typename T, size_t N>
concept exact_size = !is_empty_v<T> && (sizeof(T) == N);
template <typename T>
concept trivially_copyable = __UTL is_trivially_copyable_v<T>;
#else
template <typename T, size_t N>
using exact_size UTL_NODEBUG = bool_constant<!UTL_TRAIT_is_empty(T) && (sizeof(T) == N)>;
#endif

template <UTL_CONCEPT_CXX20(exact_size<1>) T UTL_REQUIRES_CXX11(exact_size<T, 1>::value)>
UTL_ATTRIBUTE(LIBC_API) unsigned char as_byte(T val) noexcept {
    return *((unsigned char const*)&val);
}
} // namespace libc

#undef __UTL_ATTRIBUTE_LIBC_API
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_LIBC_API

UTL_NAMESPACE_END
