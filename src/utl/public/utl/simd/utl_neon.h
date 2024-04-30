// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_type_identity.h"

#ifdef UTL_SIMD_ARM_NEON
#  include <arm_neon.h>

#  define UTL_NEON_BINARY_FUNC(NAME, TYPE, INTRINSIC)                                     \
      UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)                                     \
      inline TYPE NAME(TYPE left, TYPE right) noexcept { return INTRINSIC(left, right); } \
      static_assert(true, "semi-colon required after this macro")
#  define UTL_NEON_UNARY_FUNC(NAME, TYPE, INTRINSIC)                 \
      UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)                \
      inline TYPE NAME(TYPE arg) noexcept { return INTRINSIC(arg); } \
      static_assert(true, "semi-colon required after this macro")

UTL_NAMESPACE_BEGIN
namespace neon {

#  ifdef UTL_COMPILER_MSVC

struct alignas(2) float16_t {
    __n16 val;
    float16_t() noexcept = default;
    float16_t(__n16 val) noexcept : val(val) {}
};

struct alignas(1) poly8_t {
    ::poly8_t val;
    poly8_t() noexcept = default;
    poly8_t(::poly8_t val) noexcept : val(val) {}
};
struct alignas(2) poly16_t {
    ::poly16_t val;
    poly16_t() noexcept = default;
    poly16_t(::poly16_t val) noexcept : val(val) {}
};
struct alignas(8) poly64_t {
    ::poly64_t val;
    poly64_t() noexcept = default;
    poly64_t(::poly64_t val) noexcept : val(val) {}
};

#  endif // ifdef UTL_COMPILER_MSVC

namespace traits {

template <typename T>
struct is_element : false_type {};
template <>
struct is_element<int8_t> : true_type {};
template <>
struct is_element<int16_t> : true_type {};
template <>
struct is_element<int32_t> : true_type {};
template <>
struct is_element<int64_t> : true_type {};
template <>
struct is_element<uint8_t> : true_type {};
template <>
struct is_element<uint16_t> : true_type {};
template <>
struct is_element<uint32_t> : true_type {};
template <>
struct is_element<uint64_t> : true_type {};
template <>
struct is_element<float16_t> : true_type {};
template <>
struct is_element<float32_t> : true_type {};
template <>
struct is_element<float64_t> : true_type {};
template <>
struct is_element<poly8_t> : true_type {};
template <>
struct is_element<poly16_t> : true_type {};
template <>
struct is_element<poly64_t> : true_type {};

UTL_CONSTEVAL bool is_valid_size(size_t n) noexcept { return n == 8 || n == 16; }

template <typename T, size_t N>
struct is_valid : bool_constant<is_element<T>::value && is_valid_size(sizeof(T) * N)> {};
template <typename From, typename To>
using is_loadable = bool_constant<is_element<To>::value &&
    (sizeof(From) >= sizeof(To) && alignof(From) >= alignof(To))>;
template <typename From, typename To>
struct is_reinterpretable : bool_constant<is_element<From>::value && is_element<To>::value> {};

#  ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_element_v = is_element<T>::value;
template <typename T, size_t N>
UTL_INLINE_CXX17 constexpr bool is_valid_v = is_valid<T, N>::value;
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_loadable_v = is_loadable<From, To>::value;
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_reinterpretable_v = is_reinterpretable<From, To>::value;
#  endif

#  ifdef UTL_CXX20
template <typename T>
concept element = is_element_v<T>;
template <typename T, size_t N>
concept valid = element<T> && is_valid_size(sizeof(T) * N);
template <typename From, typename To>
concept loadable_as = element<To> && (sizeof(From) >= sizeof(To) && alignof(From) >= alignof(To));
template <typename To, typename From>
concept loadable_from = loadable_as<From, To>;
template <typename From, typename To>
concept reinterpretable_as = element<From> && element<To>;
template <typename To, typename From>
concept reinterpretable_from = reinterpretable_as<From, To>;
#  endif

} // namespace traits

#  ifdef UTL_COMPILER_MSVC

template <typename T, size_t N>
struct vector;

namespace details {
template <size_t N>
struct underlying_type;
template <>
struct underlying_type<8> {
    using type = __n64;
};
template <>
struct underlying_type<16> {
    using type = __n128;
};

template <typename T, size_t N>
using vector_type_t = typename underlying_type<is_valid<T, N>::value * sizeof(T) * N>::type;

using qreg_u8 = vector<uint8_t, 16>;
using qreg_u16 = vector<uint16_t, 8>;
using qreg_u32 = vector<uint32_t, 4>;
using qreg_u64 = vector<uint64_t, 2>;
using qreg_s8 = vector<int8_t, 16>;
using qreg_s16 = vector<int16_t, 8>;
using qreg_s32 = vector<int32_t, 4>;
using qreg_s64 = vector<int64_t, 2>;
using qreg_f16 = vector<float16_t, 8>;
using qreg_f32 = vector<float32_t, 4>;
using qreg_f64 = vector<float64_t, 2>;
using qreg_p8 = vector<poly8_t, 16>;
using qreg_p16 = vector<poly16_t, 8>;
using qreg_p64 = vector<poly64_t, 2>;
using dreg_u8 = vector<uint8_t, 8>;
using dreg_u16 = vector<uint16_t, 4>;
using dreg_u32 = vector<uint32_t, 2>;
using dreg_u64 = vector<uint64_t, 1>;
using dreg_s8 = vector<int8_t, 8>;
using dreg_s16 = vector<int16_t, 4>;
using dreg_s32 = vector<int32_t, 2>;
using dreg_s64 = vector<int64_t, 1>;
using dreg_f16 = vector<float16_t, 4>;
using dreg_f32 = vector<float32_t, 2>;
using dreg_f64 = vector<float64_t, 1>;
using dreg_p8 = vector<poly8_t, 8>;
using dreg_p16 = vector<poly16_t, 4>;
using dreg_p64 = vector<poly64_t, 1>;
} // namespace details

template <typename T, size_t N>
struct vector {
    vector(details::vector_type_t<T, N> val) noexcept : val(val) {}

    UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
    operator details::vector_type_t<T, N> const&() const noexcept { return val; }

    details::vector_type_t<T, N> val;
};

UTL_NEON_BINARY_FUNC(operator==, details::dreg_u8, vceqq_u8);
UTL_NEON_BINARY_FUNC(operator==, details::dreg_u16, vceqq_u16);
UTL_NEON_BINARY_FUNC(operator==, details::dreg_u32, vceqq_u32);
UTL_NEON_BINARY_FUNC(operator==, details::dreg_u64, vceqq_u64);

UTL_NEON_BINARY_FUNC(operator!=, details::dreg_u8, vcne_u8);
UTL_NEON_BINARY_FUNC(operator!=, details::dreg_u16, vcne_u16);
UTL_NEON_BINARY_FUNC(operator!=, details::dreg_u32, vcne_u32);
UTL_NEON_BINARY_FUNC(operator!=, details::dreg_u64, vcne_u64);

UTL_NEON_BINARY_FUNC(operator<, details::dreg_u8, vclt_u8);
UTL_NEON_BINARY_FUNC(operator<, details::dreg_u16, vclt_u16);
UTL_NEON_BINARY_FUNC(operator<, details::dreg_u32, vclt_u32);
UTL_NEON_BINARY_FUNC(operator<, details::dreg_u64, vclt_u64);

UTL_NEON_BINARY_FUNC(operator<=, details::dreg_u8, vcle_u8);
UTL_NEON_BINARY_FUNC(operator<=, details::dreg_u16, vcle_u16);
UTL_NEON_BINARY_FUNC(operator<=, details::dreg_u32, vcle_u32);
UTL_NEON_BINARY_FUNC(operator<=, details::dreg_u64, vcle_u64);

UTL_NEON_BINARY_FUNC(operator>, details::dreg_u8, vcgt_u8);
UTL_NEON_BINARY_FUNC(operator>, details::dreg_u16, vcgt_u16);
UTL_NEON_BINARY_FUNC(operator>, details::dreg_u32, vcgt_u32);
UTL_NEON_BINARY_FUNC(operator>, details::dreg_u64, vcgt_u64);

UTL_NEON_BINARY_FUNC(operator>=, details::dreg_u8, vcge_u8);
UTL_NEON_BINARY_FUNC(operator>=, details::dreg_u16, vcge_u16);
UTL_NEON_BINARY_FUNC(operator>=, details::dreg_u32, vcge_u32);
UTL_NEON_BINARY_FUNC(operator>=, details::dreg_u64, vcge_u64);

UTL_NEON_BINARY_FUNC(operator==, details::dreg_s8, vceqq_s8);
UTL_NEON_BINARY_FUNC(operator==, details::dreg_s16, vceqq_s16);
UTL_NEON_BINARY_FUNC(operator==, details::dreg_s32, vceqq_s32);
UTL_NEON_BINARY_FUNC(operator==, details::dreg_s64, vceqq_s64);

UTL_NEON_BINARY_FUNC(operator!=, details::dreg_s8, vcne_s8);
UTL_NEON_BINARY_FUNC(operator!=, details::dreg_s16, vcne_s16);
UTL_NEON_BINARY_FUNC(operator!=, details::dreg_s32, vcne_s32);
UTL_NEON_BINARY_FUNC(operator!=, details::dreg_s64, vcne_s64);

UTL_NEON_BINARY_FUNC(operator<, details::dreg_s8, vclt_s8);
UTL_NEON_BINARY_FUNC(operator<, details::dreg_s16, vclt_s16);
UTL_NEON_BINARY_FUNC(operator<, details::dreg_s32, vclt_s32);
UTL_NEON_BINARY_FUNC(operator<, details::dreg_s64, vclt_s64);

UTL_NEON_BINARY_FUNC(operator<=, details::dreg_s8, vcle_s8);
UTL_NEON_BINARY_FUNC(operator<=, details::dreg_s16, vcle_s16);
UTL_NEON_BINARY_FUNC(operator<=, details::dreg_s32, vcle_s32);
UTL_NEON_BINARY_FUNC(operator<=, details::dreg_s64, vcle_s64);

UTL_NEON_BINARY_FUNC(operator>, details::dreg_s8, vcgt_s8);
UTL_NEON_BINARY_FUNC(operator>, details::dreg_s16, vcgt_s16);
UTL_NEON_BINARY_FUNC(operator>, details::dreg_s32, vcgt_s32);
UTL_NEON_BINARY_FUNC(operator>, details::dreg_s64, vcgt_s64);

UTL_NEON_BINARY_FUNC(operator>=, details::dreg_s8, vcge_s8);
UTL_NEON_BINARY_FUNC(operator>=, details::dreg_s16, vcge_s16);
UTL_NEON_BINARY_FUNC(operator>=, details::dreg_s32, vcge_s32);
UTL_NEON_BINARY_FUNC(operator>=, details::dreg_s64, vcge_s64);

UTL_NEON_BINARY_FUNC(operator==, details::dreg_f16, vceqq_f16);
UTL_NEON_BINARY_FUNC(operator==, details::dreg_f32, vceqq_f32);
UTL_NEON_BINARY_FUNC(operator==, details::dreg_f64, vceqq_f64);

UTL_NEON_BINARY_FUNC(operator!=, details::dreg_f16, vcne_f16);
UTL_NEON_BINARY_FUNC(operator!=, details::dreg_f32, vcne_f32);
UTL_NEON_BINARY_FUNC(operator!=, details::dreg_f64, vcne_f64);

UTL_NEON_BINARY_FUNC(operator<, details::dreg_f16, vclt_f16);
UTL_NEON_BINARY_FUNC(operator<, details::dreg_f32, vclt_f32);
UTL_NEON_BINARY_FUNC(operator<, details::dreg_f64, vclt_f64);

UTL_NEON_BINARY_FUNC(operator<=, details::dreg_f16, vcle_f16);
UTL_NEON_BINARY_FUNC(operator<=, details::dreg_f32, vcle_f32);
UTL_NEON_BINARY_FUNC(operator<=, details::dreg_f64, vcle_f64);

UTL_NEON_BINARY_FUNC(operator>, details::dreg_f16, vcgt_f16);
UTL_NEON_BINARY_FUNC(operator>, details::dreg_f32, vcgt_f32);
UTL_NEON_BINARY_FUNC(operator>, details::dreg_f64, vcgt_f64);

UTL_NEON_BINARY_FUNC(operator>=, details::dreg_f16, vcge_f16);
UTL_NEON_BINARY_FUNC(operator>=, details::dreg_f32, vcge_f32);
UTL_NEON_BINARY_FUNC(operator>=, details::dreg_f64, vcge_f64);

UTL_NEON_BINARY_FUNC(operator+, details::dreg_u8, vadd_u8);
UTL_NEON_BINARY_FUNC(operator+, details::dreg_u16, vadd_u16);
UTL_NEON_BINARY_FUNC(operator+, details::dreg_u32, vadd_u32);
UTL_NEON_BINARY_FUNC(operator+, details::dreg_u64, vadd_u64);

UTL_NEON_BINARY_FUNC(operator+, details::dreg_s8, vadd_s8);
UTL_NEON_BINARY_FUNC(operator+, details::dreg_s16, vadd_s16);
UTL_NEON_BINARY_FUNC(operator+, details::dreg_s32, vadd_s32);
UTL_NEON_BINARY_FUNC(operator+, details::dreg_s64, vadd_s64);

UTL_NEON_BINARY_FUNC(operator+, details::dreg_f16, vadd_f16);
UTL_NEON_BINARY_FUNC(operator+, details::dreg_f32, vadd_f32);
UTL_NEON_BINARY_FUNC(operator+, details::dreg_f64, vadd_f64);

UTL_NEON_BINARY_FUNC(operator-, details::dreg_u8, vsub_u8);
UTL_NEON_BINARY_FUNC(operator-, details::dreg_u16, vsub_u16);
UTL_NEON_BINARY_FUNC(operator-, details::dreg_u32, vsub_u32);
UTL_NEON_BINARY_FUNC(operator-, details::dreg_u64, vsub_u64);

UTL_NEON_BINARY_FUNC(operator-, details::dreg_s8, vsub_s8);
UTL_NEON_BINARY_FUNC(operator-, details::dreg_s16, vsub_s16);
UTL_NEON_BINARY_FUNC(operator-, details::dreg_s32, vsub_s32);
UTL_NEON_BINARY_FUNC(operator-, details::dreg_s64, vsub_s64);

UTL_NEON_BINARY_FUNC(operator-, details::dreg_f16, vsub_f16);
UTL_NEON_BINARY_FUNC(operator-, details::dreg_f32, vsub_f32);
UTL_NEON_BINARY_FUNC(operator-, details::dreg_f64, vsub_f64);

UTL_NEON_BINARY_FUNC(operator*, details::dreg_u8, vmul_u8);
UTL_NEON_BINARY_FUNC(operator*, details::dreg_u16, vmul_u16);
UTL_NEON_BINARY_FUNC(operator*, details::dreg_u32, vmul_u32);
UTL_NEON_BINARY_FUNC(operator*, details::dreg_u64, vmul_u64);

UTL_NEON_BINARY_FUNC(operator*, details::dreg_s8, vmul_s8);
UTL_NEON_BINARY_FUNC(operator*, details::dreg_s16, vmul_s16);
UTL_NEON_BINARY_FUNC(operator*, details::dreg_s32, vmul_s32);
UTL_NEON_BINARY_FUNC(operator*, details::dreg_s64, vmul_s64);

UTL_NEON_BINARY_FUNC(operator*, details::dreg_f16, vmul_f16);
UTL_NEON_BINARY_FUNC(operator*, details::dreg_f32, vmul_f32);
UTL_NEON_BINARY_FUNC(operator*, details::dreg_f64, vmul_f64);

UTL_NEON_BINARY_FUNC(operator/, details::dreg_f16, vdiv_f16);
UTL_NEON_BINARY_FUNC(operator/, details::dreg_f32, vdiv_f32);
UTL_NEON_BINARY_FUNC(operator/, details::dreg_f64, vdiv_f64);

UTL_NEON_BINARY_FUNC(operator&, details::dreg_u8, vand_u8);
UTL_NEON_BINARY_FUNC(operator&, details::dreg_u16, vand_u16);
UTL_NEON_BINARY_FUNC(operator&, details::dreg_u32, vand_u32);
UTL_NEON_BINARY_FUNC(operator&, details::dreg_u64, vand_u64);

UTL_NEON_BINARY_FUNC(operator&, details::dreg_s8, vand_s8);
UTL_NEON_BINARY_FUNC(operator&, details::dreg_s16, vand_s16);
UTL_NEON_BINARY_FUNC(operator&, details::dreg_s32, vand_s32);
UTL_NEON_BINARY_FUNC(operator&, details::dreg_s64, vand_s64);

UTL_NEON_BINARY_FUNC(operator|, details::dreg_u8, vorr_u8);
UTL_NEON_BINARY_FUNC(operator|, details::dreg_u16, vorr_u16);
UTL_NEON_BINARY_FUNC(operator|, details::dreg_u32, vorr_u32);
UTL_NEON_BINARY_FUNC(operator|, details::dreg_u64, vorr_u64);

UTL_NEON_BINARY_FUNC(operator|, details::dreg_s8, vorr_s8);
UTL_NEON_BINARY_FUNC(operator|, details::dreg_s16, vorr_s16);
UTL_NEON_BINARY_FUNC(operator|, details::dreg_s32, vorr_s32);
UTL_NEON_BINARY_FUNC(operator|, details::dreg_s64, vorr_s64);

UTL_NEON_BINARY_FUNC(operator^, details::dreg_u8, veor_u8);
UTL_NEON_BINARY_FUNC(operator^, details::dreg_u16, veor_u16);
UTL_NEON_BINARY_FUNC(operator^, details::dreg_u32, veor_u32);
UTL_NEON_BINARY_FUNC(operator^, details::dreg_u64, veor_u64);

UTL_NEON_BINARY_FUNC(operator^, details::dreg_s8, veor_s8);
UTL_NEON_BINARY_FUNC(operator^, details::dreg_s16, veor_s16);
UTL_NEON_BINARY_FUNC(operator^, details::dreg_s32, veor_s32);
UTL_NEON_BINARY_FUNC(operator^, details::dreg_s64, veor_s64);

UTL_NEON_UNARY_FUNC(operator~, dreg_s8, vneg_s8);
UTL_NEON_UNARY_FUNC(operator~, dreg_s16, vneg_s16);
UTL_NEON_UNARY_FUNC(operator~, dreg_s32, vneg_s32);
UTL_NEON_UNARY_FUNC(operator~, dreg_s64, vneg_s64);

UTL_NEON_BINARY_FUNC(operator==, details::qreg_u8, vceqq_u8);
UTL_NEON_BINARY_FUNC(operator==, details::qreg_u16, vceqq_u16);
UTL_NEON_BINARY_FUNC(operator==, details::qreg_u32, vceqq_u32);
UTL_NEON_BINARY_FUNC(operator==, details::qreg_u64, vceqq_u64);

UTL_NEON_BINARY_FUNC(operator!=, details::qreg_u8, vcneq_u8);
UTL_NEON_BINARY_FUNC(operator!=, details::qreg_u16, vcneq_u16);
UTL_NEON_BINARY_FUNC(operator!=, details::qreg_u32, vcneq_u32);
UTL_NEON_BINARY_FUNC(operator!=, details::qreg_u64, vcneq_u64);

UTL_NEON_BINARY_FUNC(operator<, details::qreg_u8, vcltq_u8);
UTL_NEON_BINARY_FUNC(operator<, details::qreg_u16, vcltq_u16);
UTL_NEON_BINARY_FUNC(operator<, details::qreg_u32, vcltq_u32);
UTL_NEON_BINARY_FUNC(operator<, details::qreg_u64, vcltq_u64);

UTL_NEON_BINARY_FUNC(operator<=, details::qreg_u8, vcleq_u8);
UTL_NEON_BINARY_FUNC(operator<=, details::qreg_u16, vcleq_u16);
UTL_NEON_BINARY_FUNC(operator<=, details::qreg_u32, vcleq_u32);
UTL_NEON_BINARY_FUNC(operator<=, details::qreg_u64, vcleq_u64);

UTL_NEON_BINARY_FUNC(operator>, details::qreg_u8, vcgtq_u8);
UTL_NEON_BINARY_FUNC(operator>, details::qreg_u16, vcgtq_u16);
UTL_NEON_BINARY_FUNC(operator>, details::qreg_u32, vcgtq_u32);
UTL_NEON_BINARY_FUNC(operator>, details::qreg_u64, vcgtq_u64);

UTL_NEON_BINARY_FUNC(operator>=, details::qreg_u8, vcgeq_u8);
UTL_NEON_BINARY_FUNC(operator>=, details::qreg_u16, vcgeq_u16);
UTL_NEON_BINARY_FUNC(operator>=, details::qreg_u32, vcgeq_u32);
UTL_NEON_BINARY_FUNC(operator>=, details::qreg_u64, vcgeq_u64);

UTL_NEON_BINARY_FUNC(operator==, details::qreg_s8, vceqq_s8);
UTL_NEON_BINARY_FUNC(operator==, details::qreg_s16, vceqq_s16);
UTL_NEON_BINARY_FUNC(operator==, details::qreg_s32, vceqq_s32);
UTL_NEON_BINARY_FUNC(operator==, details::qreg_s64, vceqq_s64);

UTL_NEON_BINARY_FUNC(operator!=, details::qreg_s8, vcneq_s8);
UTL_NEON_BINARY_FUNC(operator!=, details::qreg_s16, vcneq_s16);
UTL_NEON_BINARY_FUNC(operator!=, details::qreg_s32, vcneq_s32);
UTL_NEON_BINARY_FUNC(operator!=, details::qreg_s64, vcneq_s64);

UTL_NEON_BINARY_FUNC(operator<, details::qreg_s8, vcltq_s8);
UTL_NEON_BINARY_FUNC(operator<, details::qreg_s16, vcltq_s16);
UTL_NEON_BINARY_FUNC(operator<, details::qreg_s32, vcltq_s32);
UTL_NEON_BINARY_FUNC(operator<, details::qreg_s64, vcltq_s64);

UTL_NEON_BINARY_FUNC(operator<=, details::qreg_s8, vcleq_s8);
UTL_NEON_BINARY_FUNC(operator<=, details::qreg_s16, vcleq_s16);
UTL_NEON_BINARY_FUNC(operator<=, details::qreg_s32, vcleq_s32);
UTL_NEON_BINARY_FUNC(operator<=, details::qreg_s64, vcleq_s64);

UTL_NEON_BINARY_FUNC(operator>, details::qreg_s8, vcgtq_s8);
UTL_NEON_BINARY_FUNC(operator>, details::qreg_s16, vcgtq_s16);
UTL_NEON_BINARY_FUNC(operator>, details::qreg_s32, vcgtq_s32);
UTL_NEON_BINARY_FUNC(operator>, details::qreg_s64, vcgtq_s64);

UTL_NEON_BINARY_FUNC(operator>=, details::qreg_s8, vcgeq_s8);
UTL_NEON_BINARY_FUNC(operator>=, details::qreg_s16, vcgeq_s16);
UTL_NEON_BINARY_FUNC(operator>=, details::qreg_s32, vcgeq_s32);
UTL_NEON_BINARY_FUNC(operator>=, details::qreg_s64, vcgeq_s64);

UTL_NEON_BINARY_FUNC(operator==, details::qreg_f16, vceqq_f16);
UTL_NEON_BINARY_FUNC(operator==, details::qreg_f32, vceqq_f32);
UTL_NEON_BINARY_FUNC(operator==, details::qreg_f64, vceqq_f64);

UTL_NEON_BINARY_FUNC(operator!=, details::qreg_f16, vcneq_f16);
UTL_NEON_BINARY_FUNC(operator!=, details::qreg_f32, vcneq_f32);
UTL_NEON_BINARY_FUNC(operator!=, details::qreg_f64, vcneq_f64);

UTL_NEON_BINARY_FUNC(operator<, details::qreg_f16, vcltq_f16);
UTL_NEON_BINARY_FUNC(operator<, details::qreg_f32, vcltq_f32);
UTL_NEON_BINARY_FUNC(operator<, details::qreg_f64, vcltq_f64);

UTL_NEON_BINARY_FUNC(operator<=, details::qreg_f16, vcleq_f16);
UTL_NEON_BINARY_FUNC(operator<=, details::qreg_f32, vcleq_f32);
UTL_NEON_BINARY_FUNC(operator<=, details::qreg_f64, vcleq_f64);

UTL_NEON_BINARY_FUNC(operator>, details::qreg_f16, vcgtq_f16);
UTL_NEON_BINARY_FUNC(operator>, details::qreg_f32, vcgtq_f32);
UTL_NEON_BINARY_FUNC(operator>, details::qreg_f64, vcgtq_f64);

UTL_NEON_BINARY_FUNC(operator>=, details::qreg_f16, vcgeq_f16);
UTL_NEON_BINARY_FUNC(operator>=, details::qreg_f32, vcgeq_f32);
UTL_NEON_BINARY_FUNC(operator>=, details::qreg_f64, vcgeq_f64);

UTL_NEON_BINARY_FUNC(operator+, details::qreg_u8, vaddq_u8);
UTL_NEON_BINARY_FUNC(operator+, details::qreg_u16, vaddq_u16);
UTL_NEON_BINARY_FUNC(operator+, details::qreg_u32, vaddq_u32);
UTL_NEON_BINARY_FUNC(operator+, details::qreg_u64, vaddq_u64);

UTL_NEON_BINARY_FUNC(operator+, details::qreg_s8, vaddq_s8);
UTL_NEON_BINARY_FUNC(operator+, details::qreg_s16, vaddq_s16);
UTL_NEON_BINARY_FUNC(operator+, details::qreg_s32, vaddq_s32);
UTL_NEON_BINARY_FUNC(operator+, details::qreg_s64, vaddq_s64);

UTL_NEON_BINARY_FUNC(operator+, details::qreg_f16, vaddq_f16);
UTL_NEON_BINARY_FUNC(operator+, details::qreg_f32, vaddq_f32);
UTL_NEON_BINARY_FUNC(operator+, details::qreg_f64, vaddq_f64);

UTL_NEON_BINARY_FUNC(operator-, details::qreg_u8, vsubq_u8);
UTL_NEON_BINARY_FUNC(operator-, details::qreg_u16, vsubq_u16);
UTL_NEON_BINARY_FUNC(operator-, details::qreg_u32, vsubq_u32);
UTL_NEON_BINARY_FUNC(operator-, details::qreg_u64, vsubq_u64);

UTL_NEON_BINARY_FUNC(operator-, details::qreg_s8, vsubq_s8);
UTL_NEON_BINARY_FUNC(operator-, details::qreg_s16, vsubq_s16);
UTL_NEON_BINARY_FUNC(operator-, details::qreg_s32, vsubq_s32);
UTL_NEON_BINARY_FUNC(operator-, details::qreg_s64, vsubq_s64);

UTL_NEON_BINARY_FUNC(operator-, details::qreg_f16, vsubq_f16);
UTL_NEON_BINARY_FUNC(operator-, details::qreg_f32, vsubq_f32);
UTL_NEON_BINARY_FUNC(operator-, details::qreg_f64, vsubq_f64);

UTL_NEON_BINARY_FUNC(operator*, details::qreg_u8, vmulq_u8);
UTL_NEON_BINARY_FUNC(operator*, details::qreg_u16, vmulq_u16);
UTL_NEON_BINARY_FUNC(operator*, details::qreg_u32, vmulq_u32);
UTL_NEON_BINARY_FUNC(operator*, details::qreg_u64, vmulq_u64);

UTL_NEON_BINARY_FUNC(operator*, details::qreg_s8, vmulq_s8);
UTL_NEON_BINARY_FUNC(operator*, details::qreg_s16, vmulq_s16);
UTL_NEON_BINARY_FUNC(operator*, details::qreg_s32, vmulq_s32);
UTL_NEON_BINARY_FUNC(operator*, details::qreg_s64, vmulq_s64);

UTL_NEON_BINARY_FUNC(operator*, details::qreg_f16, vmulq_f16);
UTL_NEON_BINARY_FUNC(operator*, details::qreg_f32, vmulq_f32);
UTL_NEON_BINARY_FUNC(operator*, details::qreg_f64, vmulq_f64);

UTL_NEON_BINARY_FUNC(operator/, details::qreg_f16, vdivq_f16);
UTL_NEON_BINARY_FUNC(operator/, details::qreg_f32, vdivq_f32);
UTL_NEON_BINARY_FUNC(operator/, details::qreg_f64, vdivq_f64);

UTL_NEON_BINARY_FUNC(operator&, details::qreg_u8, vandq_u8);
UTL_NEON_BINARY_FUNC(operator&, details::qreg_u16, vandq_u16);
UTL_NEON_BINARY_FUNC(operator&, details::qreg_u32, vandq_u32);
UTL_NEON_BINARY_FUNC(operator&, details::qreg_u64, vandq_u64);

UTL_NEON_BINARY_FUNC(operator&, details::qreg_s8, vandq_s8);
UTL_NEON_BINARY_FUNC(operator&, details::qreg_s16, vandq_s16);
UTL_NEON_BINARY_FUNC(operator&, details::qreg_s32, vandq_s32);
UTL_NEON_BINARY_FUNC(operator&, details::qreg_s64, vandq_s64);

UTL_NEON_BINARY_FUNC(operator|, details::qreg_u8, vorrq_u8);
UTL_NEON_BINARY_FUNC(operator|, details::qreg_u16, vorrq_u16);
UTL_NEON_BINARY_FUNC(operator|, details::qreg_u32, vorrq_u32);
UTL_NEON_BINARY_FUNC(operator|, details::qreg_u64, vorrq_u64);

UTL_NEON_BINARY_FUNC(operator|, details::qreg_s8, vorrq_s8);
UTL_NEON_BINARY_FUNC(operator|, details::qreg_s16, vorrq_s16);
UTL_NEON_BINARY_FUNC(operator|, details::qreg_s32, vorrq_s32);
UTL_NEON_BINARY_FUNC(operator|, details::qreg_s64, vorrq_s64);

UTL_NEON_BINARY_FUNC(operator^, details::qreg_u8, veorq_u8);
UTL_NEON_BINARY_FUNC(operator^, details::qreg_u16, veorq_u16);
UTL_NEON_BINARY_FUNC(operator^, details::qreg_u32, veorq_u32);
UTL_NEON_BINARY_FUNC(operator^, details::qreg_u64, veorq_u64);

UTL_NEON_BINARY_FUNC(operator^, details::qreg_s8, veorq_s8);
UTL_NEON_BINARY_FUNC(operator^, details::qreg_s16, veorq_s16);
UTL_NEON_BINARY_FUNC(operator^, details::qreg_s32, veorq_s32);
UTL_NEON_BINARY_FUNC(operator^, details::qreg_s64, veorq_s64);

UTL_NEON_UNARY_FUNC(operator~, qreg_s8, vnegq_s8);
UTL_NEON_UNARY_FUNC(operator~, qreg_s16, vnegq_s16);
UTL_NEON_UNARY_FUNC(operator~, qreg_s32, vnegq_s32);
UTL_NEON_UNARY_FUNC(operator~, qreg_s64, vnegq_s64);

#    define UTL_NEON_UNDERLYING(vec) (vec).val

#  else // ifdef UTL_COMPILER_MSVC

namespace details {
template <typename T, size_t N>
struct vector_type;
// clang-format off
template <> struct vector_type<int8_t, 8> { using type = int8x8_t; };
template <> struct vector_type<int16_t, 4> { using type = int16x4_t; };
template <> struct vector_type<int32_t, 2> { using type = int32x2_t; };
template <> struct vector_type<int64_t, 1> { using type = int64x1_t; };
template <> struct vector_type<int8_t, 16> { using type = int8x16_t; };
template <> struct vector_type<int16_t, 8> { using type = int16x8_t; };
template <> struct vector_type<int32_t, 4> { using type = int32x4_t; };
template <> struct vector_type<int64_t, 2> { using type = int64x2_t; };
template <> struct vector_type<uint8_t, 8> { using type = uint8x8_t; };
template <> struct vector_type<uint16_t, 4> { using type = uint16x4_t; };
template <> struct vector_type<uint32_t, 2> { using type = uint32x2_t; };
template <> struct vector_type<uint64_t, 1> { using type = uint64x1_t; };
template <> struct vector_type<uint8_t, 16> { using type = uint8x16_t; };
template <> struct vector_type<uint16_t, 8> { using type = uint16x8_t; };
template <> struct vector_type<uint32_t, 4> { using type = uint32x4_t; };
template <> struct vector_type<uint64_t, 2> { using type = uint64x2_t; };
template <> struct vector_type<float16_t, 4> { using type = float16x4_t; };
template <> struct vector_type<float32_t, 2> { using type = float32x2_t; };
template <> struct vector_type<float64_t, 1> { using type = float64x1_t; };
template <> struct vector_type<float16_t, 8> { using type = float16x8_t; };
template <> struct vector_type<float32_t, 4> { using type = float32x4_t; };
template <> struct vector_type<float64_t, 2> { using type = float64x2_t; };
template <> struct vector_type<poly8_t, 8> { using type = poly8x8_t; };
template <> struct vector_type<poly16_t, 4> { using type = poly16x4_t; };
template <> struct vector_type<poly64_t, 1> { using type = poly64x1_t; };
template <> struct vector_type<poly8_t, 16> { using type = poly8x16_t; };
template <> struct vector_type<poly16_t, 8> { using type = poly16x8_t; };
template <> struct vector_type<poly64_t, 2> { using type = poly64x2_t; };
template <typename T, size_t N>
using vector_type_t = typename vector_type<T, N>::type;
// clang-format on
using qreg_u8 = vector<uint8_t, 16>;
using qreg_u16 = vector<uint16_t, 8>;
using qreg_u32 = vector<uint32_t, 4>;
using qreg_u64 = vector<uint64_t, 2>;
using qreg_s8 = vector<int8_t, 16>;
using qreg_s16 = vector<int16_t, 8>;
using qreg_s32 = vector<int32_t, 4>;
using qreg_s64 = vector<int64_t, 2>;
using qreg_f16 = vector<float16_t, 8>;
using qreg_f32 = vector<float32_t, 4>;
using qreg_f64 = vector<float64_t, 2>;
using qreg_p8 = vector<poly8_t, 16>;
using qreg_p16 = vector<poly16_t, 8>;
using qreg_p64 = vector<poly64_t, 2>;
using dreg_u8 = vector<uint8_t, 8>;
using dreg_u16 = vector<uint16_t, 4>;
using dreg_u32 = vector<uint32_t, 2>;
using dreg_u64 = vector<uint64_t, 1>;
using dreg_s8 = vector<int8_t, 8>;
using dreg_s16 = vector<int16_t, 4>;
using dreg_s32 = vector<int32_t, 2>;
using dreg_s64 = vector<int64_t, 1>;
using dreg_f16 = vector<float16_t, 4>;
using dreg_f32 = vector<float32_t, 2>;
using dreg_f64 = vector<float64_t, 1>;
using dreg_p8 = vector<poly8_t, 8>;
using dreg_p16 = vector<poly16_t, 4>;
using dreg_p64 = vector<poly64_t, 1>;
} // namespace details

template <typename T, size_t N>
using vector = details::vector_type_t<T, N>;

#    define UTL_NEON_UNDERLYING(vec) (vec)

#  endif // ifdef UTL_COMPILER_MSVC

namespace details {

using u8_t = uint8_t;
using u16_t = uint16_t;
using u32_t = uint32_t;
using u64_t = uint64_t;
using s8_t = int8_t;
using s16_t = int16_t;
using s32_t = int32_t;
using s64_t = int64_t;
using f16_t = float16_t;
using f32_t = float32_t;
using f64_t = float64_t;
using p8_t = poly8_t;
using p16_t = poly16_t;
using p64_t = poly64_t;

UTL_NEON_BINARY_FUNC(orn, qreg_u8, vornq_u8);
UTL_NEON_BINARY_FUNC(orn, qreg_u16, vornq_u16);
UTL_NEON_BINARY_FUNC(orn, qreg_u32, vornq_u32);
UTL_NEON_BINARY_FUNC(orn, qreg_u64, vornq_u64);

UTL_NEON_BINARY_FUNC(orn, qreg_s8, vornq_s8);
UTL_NEON_BINARY_FUNC(orn, qreg_s16, vornq_s16);
UTL_NEON_BINARY_FUNC(orn, qreg_s32, vornq_s32);
UTL_NEON_BINARY_FUNC(orn, qreg_s64, vornq_s64);

UTL_NEON_BINARY_FUNC(bic, qreg_u8, vbicq_u8);
UTL_NEON_BINARY_FUNC(bic, qreg_u16, vbicq_u16);
UTL_NEON_BINARY_FUNC(bic, qreg_u32, vbicq_u32);
UTL_NEON_BINARY_FUNC(bic, qreg_u64, vbicq_u64);

UTL_NEON_BINARY_FUNC(bic, qreg_s8, vbicq_s8);
UTL_NEON_BINARY_FUNC(bic, qreg_s16, vbicq_s16);
UTL_NEON_BINARY_FUNC(bic, qreg_s32, vbicq_s32);
UTL_NEON_BINARY_FUNC(bic, qreg_s64, vbicq_s64);

UTL_NEON_UNARY_FUNC(neg, qreg_u8, vnegq_u8);
UTL_NEON_UNARY_FUNC(neg, qreg_u16, vnegq_u16);
UTL_NEON_UNARY_FUNC(neg, qreg_u32, vnegq_u32);
UTL_NEON_UNARY_FUNC(neg, qreg_u64, vnegq_u64);

UTL_NEON_UNARY_FUNC(neg, qreg_s8, vnegq_s8);
UTL_NEON_UNARY_FUNC(neg, qreg_s16, vnegq_s16);
UTL_NEON_UNARY_FUNC(neg, qreg_s32, vnegq_s32);
UTL_NEON_UNARY_FUNC(neg, qreg_s64, vnegq_s64);

UTL_NEON_BINARY_FUNC(orn, dreg_u8, vorn_u8);
UTL_NEON_BINARY_FUNC(orn, dreg_u16, vorn_u16);
UTL_NEON_BINARY_FUNC(orn, dreg_u32, vorn_u32);
UTL_NEON_BINARY_FUNC(orn, dreg_u64, vorn_u64);

UTL_NEON_BINARY_FUNC(orn, dreg_s8, vorn_s8);
UTL_NEON_BINARY_FUNC(orn, dreg_s16, vorn_s16);
UTL_NEON_BINARY_FUNC(orn, dreg_s32, vorn_s32);
UTL_NEON_BINARY_FUNC(orn, dreg_s64, vorn_s64);

UTL_NEON_BINARY_FUNC(bic, dreg_u8, vbic_u8);
UTL_NEON_BINARY_FUNC(bic, dreg_u16, vbic_u16);
UTL_NEON_BINARY_FUNC(bic, dreg_u32, vbic_u32);
UTL_NEON_BINARY_FUNC(bic, dreg_u64, vbic_u64);

UTL_NEON_BINARY_FUNC(bic, dreg_s8, vbic_s8);
UTL_NEON_BINARY_FUNC(bic, dreg_s16, vbic_s16);
UTL_NEON_BINARY_FUNC(bic, dreg_s32, vbic_s32);
UTL_NEON_BINARY_FUNC(bic, dreg_s64, vbic_s64);

UTL_NEON_UNARY_FUNC(neg, dreg_u8, vneg_u8);
UTL_NEON_UNARY_FUNC(neg, dreg_u16, vneg_u16);
UTL_NEON_UNARY_FUNC(neg, dreg_u32, vneg_u32);
UTL_NEON_UNARY_FUNC(neg, dreg_u64, vneg_u64);

UTL_NEON_UNARY_FUNC(neg, dreg_s8, vneg_s8);
UTL_NEON_UNARY_FUNC(neg, dreg_s16, vneg_s16);
UTL_NEON_UNARY_FUNC(neg, dreg_s32, vneg_s32);
UTL_NEON_UNARY_FUNC(neg, dreg_s64, vneg_s64);

template <typename T, size_t N>
struct negation_intermediate {
    UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
    friend vector<T, N> operator|(vector<T, N> left, negation_intermediate right) noexcept {
        return orn(left, right.val);
    }

    UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
    friend vector<T, N> operator&(vector<T, N> left, negation_intermediate right) noexcept {
        return bic(left, right.val);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
    operator vector<T, N>() const noexcept { return neg(val); }
    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
    vector<T, N> eval() const noexcept { return neg(val); }

    negation_intermediate(vector<T, N> v) noexcept : val(v) {}

private:
    vector<T, N> val;
};
} // namespace details

template <typename T, size_t N>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
details::negation_intermediate<T, N> negate(vector<T, N> v) noexcept {
    return v;
}

template <typename T, size_t N>
vector<T, N> zero() noexcept;

// clang-format off
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int8_t, 16> zero<int8_t, 16>() noexcept { return vdupq_n_s8(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int16_t, 8> zero<int16_t, 8>() noexcept { return vdupq_n_s16(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int32_t, 4> zero<int32_t, 4>() noexcept { return vdupq_n_s32(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int64_t, 2> zero<int64_t, 2>() noexcept { return vdupq_n_s64(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint8_t, 16> zero<uint8_t, 16>() noexcept { return vdupq_n_u8(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint16_t, 8> zero<uint16_t, 8>() noexcept { return vdupq_n_u16(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint32_t, 4> zero<uint32_t, 4>() noexcept { return vdupq_n_u32(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint64_t, 2> zero<uint64_t, 2>() noexcept { return vdupq_n_u64(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float32_t, 4> zero<float32_t, 4>() noexcept { return vdupq_n_f32(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float64_t, 2> zero<float64_t, 2>() noexcept { return vdupq_n_f64(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly8_t, 4> zero<poly8_t, 4>() noexcept { return vdupq_n_p8(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly16_t, 8> zero<poly16_t, 8>() noexcept { return vdupq_n_p16(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly64_t, 2> zero<poly64_t, 2>() noexcept { return vdupq_n_p64(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int8_t, 8> zero<int8_t, 8>() noexcept { return vdup_n_s8(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int16_t, 4> zero<int16_t, 4>() noexcept { return vdup_n_s16(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int32_t, 2> zero<int32_t, 2>() noexcept { return vdup_n_s32(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int64_t, 1> zero<int64_t, 1>() noexcept { return vdup_n_s64(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint8_t, 8> zero<uint8_t, 8>() noexcept { return vdup_n_u8(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint16_t, 4> zero<uint16_t, 4>() noexcept { return vdup_n_u16(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint32_t, 2> zero<uint32_t, 2>() noexcept { return vdup_n_u32(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint64_t, 1> zero<uint64_t, 1>() noexcept { return vdup_n_u64(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float32_t, 2> zero<float32_t, 2>() noexcept { return vdup_n_f32(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float64_t, 1> zero<float64_t, 1>() noexcept { return vdup_n_f64(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly8_t, 2> zero<poly8_t, 2>() noexcept { return vdup_n_p8(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly16_t, 4> zero<poly16_t, 4>() noexcept { return vdup_n_p16(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly64_t, 1> zero<poly64_t, 1>() noexcept { return vdup_n_p64(0); }
#   ifndef UTL_COMPILER_MSVC
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float16_t, 8> zero<float16_t, 8>() noexcept { return vdupq_n_f16(0); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float16_t, 4> zero<float16_t, 4>() noexcept { return vdup_n_f16(0); }
#   endif
// clang-format on

template <typename T, size_t N>
vector<T, N> load(T const* ptr) noexcept;

// clang-format off
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<int8_t, 16> load<int8_t, 16>(int8_t const* data) noexcept { return vld1q_s8(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<int16_t, 8> load<int16_t, 8>(int16_t const* data) noexcept { return vld1q_s16(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<int32_t, 4> load<int32_t, 4>(int32_t const* data) noexcept { return vld1q_s32(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<int64_t, 2> load<int64_t, 2>(int64_t const* data) noexcept { return vld1q_s64(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<uint8_t, 16> load<uint8_t, 16>(uint8_t const* data) noexcept { return vld1q_u8(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<uint16_t, 8> load<uint16_t, 8>(uint16_t const* data) noexcept { return vld1q_u16(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<uint32_t, 4> load<uint32_t, 4>(uint32_t const* data) noexcept { return vld1q_u32(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<uint64_t, 2> load<uint64_t, 2>(uint64_t const* data) noexcept { return vld1q_u64(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<float32_t, 4> load<float32_t, 4>(float32_t const* data) noexcept { return vld1q_f32(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<float64_t, 2> load<float64_t, 2>(float64_t const* data) noexcept { return vld1q_f64(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<poly8_t, 16> load<poly8_t, 16>(poly8_t const* data) noexcept { return vld1q_p8(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<poly16_t, 8> load<poly16_t, 8>(poly16_t const* data) noexcept { return vld1q_p16(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<poly64_t, 2> load<poly64_t, 2>(poly64_t const* data) noexcept { return vld1q_p64(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<int8_t, 8> load<int8_t, 8>(int8_t const* data) noexcept { return vld1_s8(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<int16_t, 4> load<int16_t, 4>(int16_t const* data) noexcept { return vld1_s16(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<int32_t, 2> load<int32_t, 2>(int32_t const* data) noexcept { return vld1_s32(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<int64_t, 1> load<int64_t, 1>(int64_t const* data) noexcept { return vld1_s64(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<uint8_t, 8> load<uint8_t, 8>(uint8_t const* data) noexcept { return vld1_u8(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<uint16_t, 4> load<uint16_t, 4>(uint16_t const* data) noexcept { return vld1_u16(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<uint32_t, 2> load<uint32_t, 2>(uint32_t const* data) noexcept { return vld1_u32(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<uint64_t, 1> load<uint64_t, 1>(uint64_t const* data) noexcept { return vld1_u64(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<float32_t, 2> load<float32_t, 2>(float32_t const* data) noexcept { return vld1_f32(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<float64_t, 1> load<float64_t, 1>(float64_t const* data) noexcept { return vld1_f64(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<poly8_t, 8> load<poly8_t, 8>(poly8_t const* data) noexcept { return vld1_p8(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<poly16_t, 4> load<poly16_t, 4>(poly16_t const* data) noexcept { return vld1_p16(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<poly64_t, 1> load<poly64_t, 1>(poly64_t const* data) noexcept { return vld1_p64(data); }
#  ifndef UTL_COMPILER_MSVC
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<float16_t, 8> load<float16_t, 8>(float16_t const* data) noexcept { return vld1q_f16(data); }
template <>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
vector<float16_t, 4> load<float16_t, 4>(float16_t const* data) noexcept { return vld1_f16(data); }
#  endif
// clang-format on
template <typename T, size_t N>
void store(T* dst, vector<T, N>) noexcept;
// clang-format off
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<int8_t, 16>(int8_t* dst, vector<int8_t, 16> src) noexcept { vst1q_s8(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<int16_t, 8>(int16_t* dst, vector<int16_t, 8> src) noexcept { vst1q_s16(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<int32_t, 4>(int32_t* dst, vector<int32_t, 4> src) noexcept { vst1q_s32(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<int64_t, 2>(int64_t* dst, vector<int64_t, 2> src) noexcept { vst1q_s64(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<uint8_t, 16>(uint8_t* dst, vector<uint8_t, 16> src) noexcept { vst1q_u8(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<uint16_t, 8>(uint16_t* dst, vector<uint16_t, 8> src) noexcept { vst1q_u16(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<uint32_t, 4>(uint32_t* dst, vector<uint32_t, 4> src) noexcept { vst1q_u32(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<uint64_t, 2>(uint64_t* dst, vector<uint64_t, 2> src) noexcept { vst1q_u64(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<float32_t, 4>(float32_t* dst, vector<float32_t, 4> src) noexcept { vst1q_f32(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<float64_t, 2>(float64_t* dst, vector<float64_t, 2> src) noexcept { vst1q_f64(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<poly8_t, 16>(poly8_t* dst, vector<poly8_t, 16> src) noexcept { vst1q_p8(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<poly16_t, 8>(poly16_t* dst, vector<poly16_t, 8> src) noexcept { vst1q_p16(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<poly64_t, 2>(poly64_t* dst, vector<poly64_t, 2> src) noexcept { vst1q_p64(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<int8_t, 8>(int8_t* dst, vector<int8_t, 8> src) noexcept { vst1_s8(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<int16_t, 4>(int16_t* dst, vector<int16_t, 4> src) noexcept { vst1_s16(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<int32_t, 2>(int32_t* dst, vector<int32_t, 2> src) noexcept { vst1_s32(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<int64_t, 1>(int64_t* dst, vector<int64_t, 1> src) noexcept { vst1_s64(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<uint8_t, 8>(uint8_t* dst, vector<uint8_t, 8> src) noexcept { vst1_u8(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<uint16_t, 4>(uint16_t* dst, vector<uint16_t, 4> src) noexcept { vst1_u16(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<uint32_t, 2>(uint32_t* dst, vector<uint32_t, 2> src) noexcept { vst1_u32(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<uint64_t, 1>(uint64_t* dst, vector<uint64_t, 1> src) noexcept { vst1_u64(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<float32_t, 2>(float32_t* dst, vector<float32_t, 2> src) noexcept { vst1_f32(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<float64_t, 1>(float64_t* dst, vector<float64_t, 1> src) noexcept { vst1_f64(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<poly8_t, 8>(poly8_t* dst, vector<poly8_t, 8> src) noexcept { vst1_p8(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<poly16_t, 4>(poly16_t* dst, vector<poly16_t, 4> src) noexcept { vst1_p16(dst, UTL_NEON_UNDERLYING(src)); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<poly64_t, 1>(poly64_t* dst, vector<poly64_t, 1> src) noexcept { vst1_p64(dst, UTL_NEON_UNDERLYING(src)); }
#  ifndef UTL_COMPILER_MSVC
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<float16_t, 8>(float16_t* dst, vector<float16_t, 8> src) noexcept { vst1q_f16(dst, src); }
template <>
UTL_ATTRIBUTES(ALWAYS_INLINE)
void store<float16_t, 4>(float16_t* dst, vector<float16_t, 4> src) noexcept { vst1_f16(dst, src); }
#  endif
// clang-format on

template <typename T, size_t N>
vector<T, N> dup(T const val) noexcept;

// clang-format off
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int8_t, 16> dup<int8_t, 16>(int8_t const val) noexcept { return vdupq_n_s8(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int16_t, 8> dup<int16_t, 8>(int16_t const val) noexcept { return vdupq_n_s16(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int32_t, 4> dup<int32_t, 4>(int32_t const val) noexcept { return vdupq_n_s32(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int64_t, 2> dup<int64_t, 2>(int64_t const val) noexcept { return vdupq_n_s64(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint8_t, 16> dup<uint8_t, 16>(uint8_t const val) noexcept { return vdupq_n_u8(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint16_t, 8> dup<uint16_t, 8>(uint16_t const val) noexcept { return vdupq_n_u16(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint32_t, 4> dup<uint32_t, 4>(uint32_t const val) noexcept { return vdupq_n_u32(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint64_t, 2> dup<uint64_t, 2>(uint64_t const val) noexcept { return vdupq_n_u64(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float32_t, 4> dup<float32_t, 4>(float32_t const val) noexcept { return vdupq_n_f32(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float64_t, 2> dup<float64_t, 2>(float64_t const val) noexcept { return vdupq_n_f64(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly8_t, 4> dup<poly8_t, 4>(poly8_t const val) noexcept { return vdupq_n_p8(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly16_t, 8> dup<poly16_t, 8>(poly16_t const val) noexcept { return vdupq_n_p16(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly64_t, 2> dup<poly64_t, 2>(poly64_t const val) noexcept { return vdupq_n_p64(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int8_t, 8> dup<int8_t, 8>(int8_t const val) noexcept { return vdup_n_s8(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int16_t, 4> dup<int16_t, 4>(int16_t const val) noexcept { return vdup_n_s16(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int32_t, 2> dup<int32_t, 2>(int32_t const val) noexcept { return vdup_n_s32(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int64_t, 1> dup<int64_t, 1>(int64_t const val) noexcept { return vdup_n_s64(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint8_t, 8> dup<uint8_t, 8>(uint8_t const val) noexcept { return vdup_n_u8(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint16_t, 4> dup<uint16_t, 4>(uint16_t const val) noexcept { return vdup_n_u16(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint32_t, 2> dup<uint32_t, 2>(uint32_t const val) noexcept { return vdup_n_u32(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint64_t, 1> dup<uint64_t, 1>(uint64_t const val) noexcept { return vdup_n_u64(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float32_t, 2> dup<float32_t, 2>(float32_t const val) noexcept { return vdup_n_f32(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float64_t, 1> dup<float64_t, 1>(float64_t const val) noexcept { return vdup_n_f64(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly8_t, 2> dup<poly8_t, 2>(poly8_t const val) noexcept { return vdup_n_p8(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly16_t, 4> dup<poly16_t, 4>(poly16_t const val) noexcept { return vdup_n_p16(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly64_t, 1> dup<poly64_t, 1>(poly64_t const val) noexcept { return vdup_n_p64(val); }
#   ifndef UTL_COMPILER_MSVC
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float16_t, 8> dup<float16_t, 8>(float16_t const val) noexcept { return vdupq_n_f16(val); }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float16_t, 4> dup<float16_t, 4>(float16_t const val) noexcept { return vdup_n_f16(val); }
#   endif
// clang-format on

template <typename To, typename From, size_t N>
vector<To, sizeof(From) * N / sizeof(To)> reinterpret(vector<From, N> val) noexcept;
// clang-format off
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int8_t, 16> reinterpret(vector<int8_t, 16> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int16_t, 8> reinterpret(vector<int16_t, 8> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int32_t, 4> reinterpret(vector<int32_t, 4> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int64_t, 2> reinterpret(vector<int64_t, 2> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint8_t, 16> reinterpret(vector<uint8_t, 16> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint16_t, 8> reinterpret(vector<uint16_t, 8> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint32_t, 4> reinterpret(vector<uint32_t, 4> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint64_t, 2> reinterpret(vector<uint64_t, 2> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly8_t, 16> reinterpret(vector<poly8_t, 16> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly16_t, 8> reinterpret(vector<poly16_t, 8> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly64_t, 2> reinterpret(vector<poly64_t, 2> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float16_t, 8> reinterpret(vector<float16_t, 8> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float32_t, 4> reinterpret(vector<float32_t, 4> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float64_t, 2> reinterpret(vector<float64_t, 2> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int8_t, 8> reinterpret(vector<int8_t, 8> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int16_t, 4> reinterpret(vector<int16_t, 4> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int32_t, 2> reinterpret(vector<int32_t, 2> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<int64_t, 1> reinterpret(vector<int64_t, 1> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint8_t, 8> reinterpret(vector<uint8_t, 8> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint16_t, 4> reinterpret(vector<uint16_t, 4> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint32_t, 2> reinterpret(vector<uint32_t, 2> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<uint64_t, 1> reinterpret(vector<uint64_t, 1> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly8_t, 8> reinterpret(vector<poly8_t, 8> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly16_t, 4> reinterpret(vector<poly16_t, 4> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<poly64_t, 1> reinterpret(vector<poly64_t, 1> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float16_t, 4> reinterpret(vector<float16_t, 4> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float32_t, 2> reinterpret(vector<float32_t, 2> val) noexcept { return val; }
template <>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
vector<float64_t, 1> reinterpret(vector<float64_t, 1> val) noexcept { return val; }
// clang-format on

#  define UTL_NEON_CASTQ(FROM, TO)                                        \
      template <>                                                         \
      UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)                     \
      details::qreg_##TO reinterpret(details::qreg_##FROM val) noexcept { \
          return vreinterpretq_##TO##_##FROM(UTL_NEON_UNDERLYING(val));   \
      }                                                                   \
      static_assert(true, "semi-colon required")

#  define UTL_NEON_CASTD(FROM, TO)                                        \
      template <>                                                         \
      UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)                     \
      details::dreg_##TO reinterpret(details::dreg_##FROM val) noexcept { \
          return vreinterpret_##TO##_##FROM(UTL_NEON_UNDERLYING(val));    \
      }                                                                   \
      static_assert(true, "semi-colon required")

// clang-format off
UTL_NEON_CASTQ(s16, s8); UTL_NEON_CASTD(s16, s8);
UTL_NEON_CASTQ(s32, s8); UTL_NEON_CASTD(s32, s8);
UTL_NEON_CASTQ(s64, s8); UTL_NEON_CASTD(s64, s8);
UTL_NEON_CASTQ(u8, s8); UTL_NEON_CASTD(u8, s8);
UTL_NEON_CASTQ(u16, s8); UTL_NEON_CASTD(u16, s8);
UTL_NEON_CASTQ(u32, s8); UTL_NEON_CASTD(u32, s8);
UTL_NEON_CASTQ(u64, s8); UTL_NEON_CASTD(u64, s8);
UTL_NEON_CASTQ(f16, s8); UTL_NEON_CASTD(f16, s8);
UTL_NEON_CASTQ(f32, s8); UTL_NEON_CASTD(f32, s8);
UTL_NEON_CASTQ(f64, s8); UTL_NEON_CASTD(f64, s8);
UTL_NEON_CASTQ(p8, s8); UTL_NEON_CASTD(p8, s8);
UTL_NEON_CASTQ(p16, s8); UTL_NEON_CASTD(p16, s8);
UTL_NEON_CASTQ(p64, s8); UTL_NEON_CASTD(p64, s8);

UTL_NEON_CASTQ(s8, s16); UTL_NEON_CASTD(s8, s16);
UTL_NEON_CASTQ(s32, s16); UTL_NEON_CASTD(s32, s16);
UTL_NEON_CASTQ(s64, s16); UTL_NEON_CASTD(s64, s16);
UTL_NEON_CASTQ(u8, s16); UTL_NEON_CASTD(u8, s16);
UTL_NEON_CASTQ(u16, s16); UTL_NEON_CASTD(u16, s16);
UTL_NEON_CASTQ(u32, s16); UTL_NEON_CASTD(u32, s16);
UTL_NEON_CASTQ(u64, s16); UTL_NEON_CASTD(u64, s16);
UTL_NEON_CASTQ(f16, s16); UTL_NEON_CASTD(f16, s16);
UTL_NEON_CASTQ(f32, s16); UTL_NEON_CASTD(f32, s16);
UTL_NEON_CASTQ(f64, s16); UTL_NEON_CASTD(f64, s16);
UTL_NEON_CASTQ(p8, s16); UTL_NEON_CASTD(p8, s16);
UTL_NEON_CASTQ(p16, s16); UTL_NEON_CASTD(p16, s16);
UTL_NEON_CASTQ(p64, s16); UTL_NEON_CASTD(p64, s16);

UTL_NEON_CASTQ(s8, s32); UTL_NEON_CASTD(s8, s32);
UTL_NEON_CASTQ(s16, s32); UTL_NEON_CASTD(s16, s32);
UTL_NEON_CASTQ(s64, s32); UTL_NEON_CASTD(s64, s32);
UTL_NEON_CASTQ(u8, s32); UTL_NEON_CASTD(u8, s32);
UTL_NEON_CASTQ(u16, s32); UTL_NEON_CASTD(u16, s32);
UTL_NEON_CASTQ(u32, s32); UTL_NEON_CASTD(u32, s32);
UTL_NEON_CASTQ(u64, s32); UTL_NEON_CASTD(u64, s32);
UTL_NEON_CASTQ(f16, s32); UTL_NEON_CASTD(f16, s32);
UTL_NEON_CASTQ(f32, s32); UTL_NEON_CASTD(f32, s32);
UTL_NEON_CASTQ(f64, s32); UTL_NEON_CASTD(f64, s32);
UTL_NEON_CASTQ(p8, s32); UTL_NEON_CASTD(p8, s32);
UTL_NEON_CASTQ(p16, s32); UTL_NEON_CASTD(p16, s32);
UTL_NEON_CASTQ(p64, s32); UTL_NEON_CASTD(p64, s32);

UTL_NEON_CASTQ(s8, s64); UTL_NEON_CASTD(s8, s64);
UTL_NEON_CASTQ(s16, s64); UTL_NEON_CASTD(s16, s64);
UTL_NEON_CASTQ(s32, s64); UTL_NEON_CASTD(s32, s64);
UTL_NEON_CASTQ(u8, s64); UTL_NEON_CASTD(u8, s64);
UTL_NEON_CASTQ(u16, s64); UTL_NEON_CASTD(u16, s64);
UTL_NEON_CASTQ(u32, s64); UTL_NEON_CASTD(u32, s64);
UTL_NEON_CASTQ(u64, s64); UTL_NEON_CASTD(u64, s64);
UTL_NEON_CASTQ(f16, s64); UTL_NEON_CASTD(f16, s64);
UTL_NEON_CASTQ(f32, s64); UTL_NEON_CASTD(f32, s64);
UTL_NEON_CASTQ(f64, s64); UTL_NEON_CASTD(f64, s64);
UTL_NEON_CASTQ(p8, s64); UTL_NEON_CASTD(p8, s64);
UTL_NEON_CASTQ(p16, s64); UTL_NEON_CASTD(p16, s64);
UTL_NEON_CASTQ(p64, s64); UTL_NEON_CASTD(p64, s64);

UTL_NEON_CASTQ(s8, u8); UTL_NEON_CASTD(s8, u8);
UTL_NEON_CASTQ(s16, u8); UTL_NEON_CASTD(s16, u8);
UTL_NEON_CASTQ(s32, u8); UTL_NEON_CASTD(s32, u8);
UTL_NEON_CASTQ(s64, u8); UTL_NEON_CASTD(s64, u8);
UTL_NEON_CASTQ(u16, u8); UTL_NEON_CASTD(u16, u8);
UTL_NEON_CASTQ(u32, u8); UTL_NEON_CASTD(u32, u8);
UTL_NEON_CASTQ(u64, u8); UTL_NEON_CASTD(u64, u8);
UTL_NEON_CASTQ(f16, u8); UTL_NEON_CASTD(f16, u8);
UTL_NEON_CASTQ(f32, u8); UTL_NEON_CASTD(f32, u8);
UTL_NEON_CASTQ(f64, u8); UTL_NEON_CASTD(f64, u8);
UTL_NEON_CASTQ(p8, u8); UTL_NEON_CASTD(p8, u8);
UTL_NEON_CASTQ(p16, u8); UTL_NEON_CASTD(p16, u8);
UTL_NEON_CASTQ(p64, u8); UTL_NEON_CASTD(p64, u8);

UTL_NEON_CASTQ(s8, u16); UTL_NEON_CASTD(s8, u16);
UTL_NEON_CASTQ(s16, u16); UTL_NEON_CASTD(s16, u16);
UTL_NEON_CASTQ(s32, u16); UTL_NEON_CASTD(s32, u16);
UTL_NEON_CASTQ(s64, u16); UTL_NEON_CASTD(s64, u16);
UTL_NEON_CASTQ(u8, u16); UTL_NEON_CASTD(u8, u16);
UTL_NEON_CASTQ(u32, u16); UTL_NEON_CASTD(u32, u16);
UTL_NEON_CASTQ(u64, u16); UTL_NEON_CASTD(u64, u16);
UTL_NEON_CASTQ(f16, u16); UTL_NEON_CASTD(f16, u16);
UTL_NEON_CASTQ(f32, u16); UTL_NEON_CASTD(f32, u16);
UTL_NEON_CASTQ(f64, u16); UTL_NEON_CASTD(f64, u16);
UTL_NEON_CASTQ(p8, u16); UTL_NEON_CASTD(p8, u16);
UTL_NEON_CASTQ(p16, u16); UTL_NEON_CASTD(p16, u16);
UTL_NEON_CASTQ(p64, u16); UTL_NEON_CASTD(p64, u16);

UTL_NEON_CASTQ(s8, u32); UTL_NEON_CASTD(s8, u32);
UTL_NEON_CASTQ(s16, u32); UTL_NEON_CASTD(s16, u32);
UTL_NEON_CASTQ(s32, u32); UTL_NEON_CASTD(s32, u32);
UTL_NEON_CASTQ(s64, u32); UTL_NEON_CASTD(s64, u32);
UTL_NEON_CASTQ(u8, u32); UTL_NEON_CASTD(u8, u32);
UTL_NEON_CASTQ(u16, u32); UTL_NEON_CASTD(u16, u32);
UTL_NEON_CASTQ(u64, u32); UTL_NEON_CASTD(u64, u32);
UTL_NEON_CASTQ(f16, u32); UTL_NEON_CASTD(f16, u32);
UTL_NEON_CASTQ(f32, u32); UTL_NEON_CASTD(f32, u32);
UTL_NEON_CASTQ(f64, u32); UTL_NEON_CASTD(f64, u32);
UTL_NEON_CASTQ(p8, u32); UTL_NEON_CASTD(p8, u32);
UTL_NEON_CASTQ(p16, u32); UTL_NEON_CASTD(p16, u32);
UTL_NEON_CASTQ(p64, u32); UTL_NEON_CASTD(p64, u32);

UTL_NEON_CASTQ(s8, u64); UTL_NEON_CASTD(s8, u64);
UTL_NEON_CASTQ(s16, u64); UTL_NEON_CASTD(s16, u64);
UTL_NEON_CASTQ(s32, u64); UTL_NEON_CASTD(s32, u64);
UTL_NEON_CASTQ(s64, u64); UTL_NEON_CASTD(s64, u64);
UTL_NEON_CASTQ(u8, u64); UTL_NEON_CASTD(u8, u64);
UTL_NEON_CASTQ(u16, u64); UTL_NEON_CASTD(u16, u64);
UTL_NEON_CASTQ(u32, u64); UTL_NEON_CASTD(u32, u64);
UTL_NEON_CASTQ(f16, u64); UTL_NEON_CASTD(f16, u64);
UTL_NEON_CASTQ(f32, u64); UTL_NEON_CASTD(f32, u64);
UTL_NEON_CASTQ(f64, u64); UTL_NEON_CASTD(f64, u64);
UTL_NEON_CASTQ(p8, u64); UTL_NEON_CASTD(p8, u64);
UTL_NEON_CASTQ(p16, u64); UTL_NEON_CASTD(p16, u64);
UTL_NEON_CASTQ(p64, u64); UTL_NEON_CASTD(p64, u64);

UTL_NEON_CASTQ(s8, f16); UTL_NEON_CASTD(s8, f16);
UTL_NEON_CASTQ(s16, f16); UTL_NEON_CASTD(s16, f16);
UTL_NEON_CASTQ(s32, f16); UTL_NEON_CASTD(s32, f16);
UTL_NEON_CASTQ(s64, f16); UTL_NEON_CASTD(s64, f16);
UTL_NEON_CASTQ(u8, f16); UTL_NEON_CASTD(u8, f16);
UTL_NEON_CASTQ(u16, f16); UTL_NEON_CASTD(u16, f16);
UTL_NEON_CASTQ(u32, f16); UTL_NEON_CASTD(u32, f16);
UTL_NEON_CASTQ(u64, f16); UTL_NEON_CASTD(u64, f16);
UTL_NEON_CASTQ(f32, f16); UTL_NEON_CASTD(f32, f16);
UTL_NEON_CASTQ(f64, f16); UTL_NEON_CASTD(f64, f16);
UTL_NEON_CASTQ(p8, f16); UTL_NEON_CASTD(p8, f16);
UTL_NEON_CASTQ(p16, f16); UTL_NEON_CASTD(p16, f16);
UTL_NEON_CASTQ(p64, f16); UTL_NEON_CASTD(p64, f16);

UTL_NEON_CASTQ(s8, f32); UTL_NEON_CASTD(s8, f32);
UTL_NEON_CASTQ(s16, f32); UTL_NEON_CASTD(s16, f32);
UTL_NEON_CASTQ(s32, f32); UTL_NEON_CASTD(s32, f32);
UTL_NEON_CASTQ(s64, f32); UTL_NEON_CASTD(s64, f32);
UTL_NEON_CASTQ(u8, f32); UTL_NEON_CASTD(u8, f32);
UTL_NEON_CASTQ(u16, f32); UTL_NEON_CASTD(u16, f32);
UTL_NEON_CASTQ(u32, f32); UTL_NEON_CASTD(u32, f32);
UTL_NEON_CASTQ(u64, f32); UTL_NEON_CASTD(u64, f32);
UTL_NEON_CASTQ(f16, f32); UTL_NEON_CASTD(f16, f32);
UTL_NEON_CASTQ(f64, f32); UTL_NEON_CASTD(f64, f32);
UTL_NEON_CASTQ(p8, f32); UTL_NEON_CASTD(p8, f32);
UTL_NEON_CASTQ(p16, f32); UTL_NEON_CASTD(p16, f32);
UTL_NEON_CASTQ(p64, f32); UTL_NEON_CASTD(p64, f32);

UTL_NEON_CASTQ(s8, f64); UTL_NEON_CASTD(s8, f64);
UTL_NEON_CASTQ(s16, f64); UTL_NEON_CASTD(s16, f64);
UTL_NEON_CASTQ(s32, f64); UTL_NEON_CASTD(s32, f64);
UTL_NEON_CASTQ(s64, f64); UTL_NEON_CASTD(s64, f64);
UTL_NEON_CASTQ(u8, f64); UTL_NEON_CASTD(u8, f64);
UTL_NEON_CASTQ(u16, f64); UTL_NEON_CASTD(u16, f64);
UTL_NEON_CASTQ(u32, f64); UTL_NEON_CASTD(u32, f64);
UTL_NEON_CASTQ(u64, f64); UTL_NEON_CASTD(u64, f64);
UTL_NEON_CASTQ(f16, f64); UTL_NEON_CASTD(f16, f64);
UTL_NEON_CASTQ(f32, f64); UTL_NEON_CASTD(f32, f64);
UTL_NEON_CASTQ(p8, f64); UTL_NEON_CASTD(p8, f64);
UTL_NEON_CASTQ(p16, f64); UTL_NEON_CASTD(p16, f64);
UTL_NEON_CASTQ(p64, f64); UTL_NEON_CASTD(p64, f64);

UTL_NEON_CASTQ(s8, p8); UTL_NEON_CASTD(s8, p8);
UTL_NEON_CASTQ(s16, p8); UTL_NEON_CASTD(s16, p8);
UTL_NEON_CASTQ(s32, p8); UTL_NEON_CASTD(s32, p8);
UTL_NEON_CASTQ(s64, p8); UTL_NEON_CASTD(s64, p8);
UTL_NEON_CASTQ(u8, p8); UTL_NEON_CASTD(u8, p8);
UTL_NEON_CASTQ(u16, p8); UTL_NEON_CASTD(u16, p8);
UTL_NEON_CASTQ(u32, p8); UTL_NEON_CASTD(u32, p8);
UTL_NEON_CASTQ(u64, p8); UTL_NEON_CASTD(u64, p8);
UTL_NEON_CASTQ(f16, p8); UTL_NEON_CASTD(f16, p8);
UTL_NEON_CASTQ(f32, p8); UTL_NEON_CASTD(f32, p8);
UTL_NEON_CASTQ(f64, p8); UTL_NEON_CASTD(f64, p8);
UTL_NEON_CASTQ(p16, p8); UTL_NEON_CASTD(p16, p8);
UTL_NEON_CASTQ(p64, p8); UTL_NEON_CASTD(p64, p8);

UTL_NEON_CASTQ(s8, p16); UTL_NEON_CASTD(s8, p16);
UTL_NEON_CASTQ(s16, p16); UTL_NEON_CASTD(s16, p16);
UTL_NEON_CASTQ(s32, p16); UTL_NEON_CASTD(s32, p16);
UTL_NEON_CASTQ(s64, p16); UTL_NEON_CASTD(s64, p16);
UTL_NEON_CASTQ(u8, p16); UTL_NEON_CASTD(u8, p16);
UTL_NEON_CASTQ(u16, p16); UTL_NEON_CASTD(u16, p16);
UTL_NEON_CASTQ(u32, p16); UTL_NEON_CASTD(u32, p16);
UTL_NEON_CASTQ(u64, p16); UTL_NEON_CASTD(u64, p16);
UTL_NEON_CASTQ(f16, p16); UTL_NEON_CASTD(f16, p16);
UTL_NEON_CASTQ(f32, p16); UTL_NEON_CASTD(f32, p16);
UTL_NEON_CASTQ(f64, p16); UTL_NEON_CASTD(f64, p16);
UTL_NEON_CASTQ(p8, p16); UTL_NEON_CASTD(p8, p16);
UTL_NEON_CASTQ(p64, p16); UTL_NEON_CASTD(p64, p16);

UTL_NEON_CASTQ(s8, p64); UTL_NEON_CASTD(s8, p64);
UTL_NEON_CASTQ(s16, p64); UTL_NEON_CASTD(s16, p64);
UTL_NEON_CASTQ(s32, p64); UTL_NEON_CASTD(s32, p64);
UTL_NEON_CASTQ(s64, p64); UTL_NEON_CASTD(s64, p64);
UTL_NEON_CASTQ(u8, p64); UTL_NEON_CASTD(u8, p64);
UTL_NEON_CASTQ(u16, p64); UTL_NEON_CASTD(u16, p64);
UTL_NEON_CASTQ(u32, p64); UTL_NEON_CASTD(u32, p64);
UTL_NEON_CASTQ(u64, p64); UTL_NEON_CASTD(u64, p64);
UTL_NEON_CASTQ(f16, p64); UTL_NEON_CASTD(f16, p64);
UTL_NEON_CASTQ(f32, p64); UTL_NEON_CASTD(f32, p64);
UTL_NEON_CASTQ(f64, p64); UTL_NEON_CASTD(f64, p64);
UTL_NEON_CASTQ(p16, p64); UTL_NEON_CASTD(p16, p64);
UTL_NEON_CASTQ(p64, p64); UTL_NEON_CASTD(p64, p64);
// clang-format on

#  undef UTL_NEON_CASTD
#  undef UTL_NEON_CASTQ

namespace qregister {
UTL_INLINE_CXX17 constexpr size_t width = 16;
}

namespace dregister {
UTL_INLINE_CXX17 constexpr size_t width = 8;
}

template <typename T>
using qregister_t = vector<T, qregister::width / sizeof(T)>;
template <typename T>
using dregister_t = vector<T, dregister::width / sizeof(T)>;

namespace qregister {
template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
constexpr size_t length() noexcept {
    return width / sizeof(T);
}

template <typename T>
using native_array = T[length<T>()];

template <typename T>
struct alignas(width) array {
    static constexpr size_t size = length<T>();

    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE)
    static array store(qregister_t<T> reg) noexcept {
        array output;
        UTL_SCOPE store<T, size>(output.val, reg);
        return output;
    }

    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
    T& operator[](size_t idx) noexcept { return val[idx]; }
    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
    T operator[](size_t idx) const noexcept { return val[idx]; }

    UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
    native_array<T>& as_native() noexcept { return val; }
    UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
    native_array<T> const& as_native() const noexcept { return val; }

private:
    native_array<T> val;
};

template <typename To, typename From>
UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE)
array<To> store_as(qregister_t<From> reg) noexcept {
    return array<To>::store(reinterpret<To>(reg));
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
inline qregister_t<T> dup(T const val) noexcept {
    return UTL_SCOPE neon::dup<T, length<T>()>(val);
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
inline qregister_t<T> zero() noexcept {
    return UTL_SCOPE neon::zero<T, length<T>()>();
}

template <typename T, typename U>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
inline enable_if_t<traits::is_loadable<U, T>::value, qregister_t<T>> load(U const* data) noexcept {
    return UTL_SCOPE neon::load<T, length<T>()>((T const*)data);
}

template <typename T>
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(T* dst, qregister_t<T> src) noexcept {
    return UTL_SCOPE neon::store<T, length<T>()>(dst, src);
}

template <typename To, typename From>
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store_as(typename type_identity<To>::type* dst, qregister_t<From> reg) noexcept {
    return store(dst, reinterpret<To>(reg));
}
} // namespace qregister

namespace dregister {
template <typename T>
UTL_CONSTEVAL size_t length() noexcept {
    return width / sizeof(T);
}
template <typename T>
using native_array = T[length<T>()];

template <typename T>
struct alignas(width) array {
    static constexpr size_t size = length<T>();

    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE)
    static array store(dregister_t<T> reg) noexcept {
        array output;
        UTL_SCOPE store<T, size>(output.val, reg);
        return output;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
    T& operator[](size_t idx) noexcept { return val[idx]; }
    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
    T operator[](size_t idx) const noexcept { return val[idx]; }

    UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
    native_array<T>& as_native() noexcept { return val; }
    UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
    native_array<T> const& as_native() const noexcept { return val; }

private:
    native_array<T> val;
};

template <typename To, typename From>
UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE)
inline array<To> store_as(dregister_t<From> reg) noexcept {
    return array<To>::store(reinterpret<To>(reg));
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
inline dregister_t<T> dup(T const val) noexcept {
    return UTL_SCOPE neon::dup<T, length<T>()>(val);
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
inline dregister_t<T> zero() noexcept {
    return UTL_SCOPE neon::zero<T, length<T>()>();
}

template <typename T, typename U>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
inline enable_if_t<traits::is_loadable<U, T>::value, dregister_t<T>> load(U const* data) noexcept {
    return UTL_SCOPE neon::load<T, length<T>()>((T const*)data);
}

template <typename T>
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(T* dst, dregister_t<T> src) noexcept {
    return UTL_SCOPE neon::store<T, length<T>()>(dst, src);
}

template <typename To, typename From>
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store_as(typename type_identity<To>::type* dst, dregister_t<From> reg) noexcept {
    return store(dst, reinterpret<To>(reg));
}

} // namespace dregister
} // namespace neon

UTL_NAMESPACE_END

#  undef UTL_NEON_UNARY_FUNC
#  undef UTL_NEON_BINARY_FUNC

#endif // ifdef UTL_SIMD_ARM_NEON
