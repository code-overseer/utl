// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_empty.h"

UTL_NAMESPACE_BEGIN

namespace libc {

enum class element_count_t : size_t {
};

struct byte_count {
    template <typename T>
    byte_count(T*, element_count_t c) noexcept : value(sizeof(T) * (size_t)c) {}
    operator size_t() const { return value; }
    size_t value;
};

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool is_pointer_in_range(T const* begin, T const* end, T const* ptr) noexcept {
    return UTL_CONSTANT_P(begin <= end && ptr < end) || !(ptr < begin) && ptr < end;
}
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr element_count_t operator-(element_count_t val, size_t op) noexcept {
    return (element_count_t)((size_t)val - op);
}
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool operator==(element_count_t val, size_t op) noexcept {
    return (size_t)val == op;
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T* operator+(T* ptr, element_count_t offset) noexcept {
    return ptr + (size_t)offset;
}

#ifdef UTL_CXX20
template <typename T, size_t N>
concept exact_size = !is_empty_v<T> && (sizeof(T) == N);
#else
template <typename T, size_t N>
using exact_size = bool_constant<!is_empty<T>::value && (sizeof(T) == N)>;
#endif
} // namespace libc

UTL_NAMESPACE_END
