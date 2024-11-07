// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_is_same.h"
namespace {
using size_t = decltype(sizeof(0));
template <typename T, size_t N>
struct array_size {
    static constexpr size_t value = N;
};
template <typename T, size_t N>
array_size<size_t, N> ArraySize(T (&)[N]) noexcept;
} // namespace

#define ARRAY_SIZE(A) decltype(ArraySize(A))::value
#define ASSERT_NOEXCEPT(...) static_assert(noexcept(__VA_ARGS__), "Operation must be noexcept")
#define ASSERT_SAME_TYPE(...) static_assert(utl::is_same<__VA_ARGS__>::value, "Types must be equal")
