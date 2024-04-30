// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_bit_ceil.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_type_identity.h"

#include <limits.h>

#ifdef UTL_SIMD_ARM_SVE
#  ifdef UTL_COMPILER_MSVC
#    error MSVC unsupported
#  endif

#  define UTL_SVE_CONST UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
#  define UTL_SVE_PURE UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)

UTL_NAMESPACE_BEGIN
namespace sve {
namespace details {
template <typename T>
struct vector_type;

// clang-format off
template <> struct vector_type<int8_t> { using type = svint8_t; };
template <> struct vector_type<int16_t> { using type = svint16_t; };
template <> struct vector_type<int32_t> { using type = svint32_t; };
template <> struct vector_type<int64_t> { using type = svint64_t; };
template <> struct vector_type<uint8_t> { using type = svuint8_t; };
template <> struct vector_type<uint16_t> { using type = svuint16_t; };
template <> struct vector_type<uint32_t> { using type = svuint32_t; };
template <> struct vector_type<uint64_t> { using type = svuint64_t; };
template <> struct vector_type<bfloat16_t> { using type = svbfloat16_t; };
template <> struct vector_type<float16_t> { using type = svfloat16_t; };
template <> struct vector_type<float32_t> { using type = svfloat32_t; };
template <> struct vector_type<float64_t> { using type = svfloat64_t; };
// clang-format on

using vector_s8 = svint8_t;
using vector_s16 = svint16_t;
using vector_s32 = svint32_t;
using vector_s64 = svint64_t;
using vector_u8 = svuint8_t;
using vector_u16 = svuint16_t;
using vector_u32 = svuint32_t;
using vector_u64 = svuint64_t;
using vector_bf16 = svbfloat16_t;
using vector_f16 = svfloat16_t;
using vector_f32 = svfloat32_t;
using vector_f64 = svfloat64_t;

template <typename T>
using vector = typename vector_type<T>::type;
using predicate = svbool_t;

template <typename T, typename = void>
struct is_element : false_type {};
template <typename T>
struct is_element<T, void_t<vector<T>>> : true_type {};

template <typename From, typename To>
using is_loadable = bool_constant<is_element<To>::value &&
    (sizeof(From) >= sizeof(To) && alignof(From) >= alignof(To))>;
template <typename From, typename To>
struct is_reinterpretable : bool_constant<is_element<From>::value && is_element<To>::value> {};

#  ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_element_v = is_element<T>::value;
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_loadable_v = is_loadable<From, To>::value;
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_reinterpretable_v = is_reinterpretable<From, To>::value;
#  endif

#  ifdef UTL_CXX20
template <typename T>
concept element = is_element_v<T>;
template <typename From, typename To>
concept loadable_as = element<To> && (sizeof(From) >= sizeof(To) && alignof(From) >= alignof(To));
template <typename To, typename From>
concept loadable_from = loadable_as<From, To>;
template <typename From, typename To>
concept reinterpretable_as = element<From> && element<To> && is_reinterpretable_v<From, To>;
template <typename To, typename From>
concept reinterpretable_from = reinterpretable_as<From, To>;
#  endif

using s8_t = int8_t;
using s16_t = int16_t;
using s32_t = int32_t;
using s64_t = int64_t;
using u8_t = uint8_t;
using u16_t = uint16_t;
using u32_t = uint32_t;
using u64_t = uint64_t;
using bf16_t = bfloat16_t;
using f16_t = float16_t;
using f32_t = float32_t;
using f64_t = float64_t;

inline predicate break_on_true(predicate pg, predicate vec) noexcept { return svbrkb_b_z(pg, vec); }
inline predicate break_after_true(predicate pg, predicate vec) noexcept {
    return svbrka_b_z(pg, vec);
}

// clang-format off
UTL_SVE_CONST
inline vector_s8 dup(s8_t val) noexcept { return svdup_n_s8(val); }
UTL_SVE_CONST
inline vector_s16 dup(s16_t val) noexcept { return svdup_n_s16(val); }
UTL_SVE_CONST
inline vector_s32 dup(s32_t val) noexcept { return svdup_n_s32(val); }
UTL_SVE_CONST
inline vector_s64 dup(s64_t val) noexcept { return svdup_n_s64(val); }
UTL_SVE_CONST
inline vector_u8 dup(u8_t val) noexcept { return svdup_n_u8(val); }
UTL_SVE_CONST
inline vector_u16 dup(u16_t val) noexcept { return svdup_n_u16(val); }
UTL_SVE_CONST
inline vector_u32 dup(u32_t val) noexcept { return svdup_n_u32(val); }
UTL_SVE_CONST
inline vector_u64 dup(u64_t val) noexcept { return svdup_n_u64(val); }
UTL_SVE_CONST
inline vector_bf16 dup(bf16_t val) noexcept { return svdup_n_bf16(val); }
UTL_SVE_CONST
inline vector_f16 dup(f16_t val) noexcept { return svdup_n_f16(val); }
UTL_SVE_CONST
inline vector_f32 dup(f32_t val) noexcept { return svdup_n_f32(val); }
UTL_SVE_CONST
inline vector_f64 dup(f64_t val) noexcept { return svdup_n_f64(val); }
// clang-format on

UTL_SVE_CONST
inline vector_s8 dup_or_zero(predicate pg, s8_t val) noexcept { return svdup_n_s8_z(pg, val); }
UTL_SVE_CONST
inline vector_s16 dup_or_zero(predicate pg, s16_t val) noexcept { return svdup_n_s16_z(pg, val); }
UTL_SVE_CONST
inline vector_s32 dup_or_zero(predicate pg, s32_t val) noexcept { return svdup_n_s32_z(pg, val); }
UTL_SVE_CONST
inline vector_s64 dup_or_zero(predicate pg, s64_t val) noexcept { return svdup_n_s64_z(pg, val); }
UTL_SVE_CONST
inline vector_u8 dup_or_zero(predicate pg, u8_t val) noexcept { return svdup_n_u8_z(pg, val); }
UTL_SVE_CONST
inline vector_u16 dup_or_zero(predicate pg, u16_t val) noexcept { return svdup_n_u16_z(pg, val); }
UTL_SVE_CONST
inline vector_u32 dup_or_zero(predicate pg, u32_t val) noexcept { return svdup_n_u32_z(pg, val); }
UTL_SVE_CONST
inline vector_u64 dup_or_zero(predicate pg, u64_t val) noexcept { return svdup_n_u64_z(pg, val); }
UTL_SVE_CONST
inline vector_bf16 dup_or_zero(predicate pg, bf16_t val) noexcept {
    return svdup_n_bf16_z(pg, val);
}
UTL_SVE_CONST
inline vector_f16 dup_or_zero(predicate pg, f16_t val) noexcept { return svdup_n_f16_z(pg, val); }
UTL_SVE_CONST
inline vector_f32 dup_or_zero(predicate pg, f32_t val) noexcept { return svdup_n_f32_z(pg, val); }
UTL_SVE_CONST
inline vector_f64 dup_or_zero(predicate pg, f64_t val) noexcept { return svdup_n_f64_z(pg, val); }

UTL_SVE_CONST
inline vector_s8 dup_or_ignore(predicate pg, s8_t val) noexcept { return svdup_n_s8_x(pg, val); }
UTL_SVE_CONST
inline vector_s16 dup_or_ignore(predicate pg, s16_t val) noexcept { return svdup_n_s16_x(pg, val); }
UTL_SVE_CONST
inline vector_s32 dup_or_ignore(predicate pg, s32_t val) noexcept { return svdup_n_s32_x(pg, val); }
UTL_SVE_CONST
inline vector_s64 dup_or_ignore(predicate pg, s64_t val) noexcept { return svdup_n_s64_x(pg, val); }
UTL_SVE_CONST
inline vector_u8 dup_or_ignore(predicate pg, u8_t val) noexcept { return svdup_n_u8_x(pg, val); }
UTL_SVE_CONST
inline vector_u16 dup_or_ignore(predicate pg, u16_t val) noexcept { return svdup_n_u16_x(pg, val); }
UTL_SVE_CONST
inline vector_u32 dup_or_ignore(predicate pg, u32_t val) noexcept { return svdup_n_u32_x(pg, val); }
UTL_SVE_CONST
inline vector_u64 dup_or_ignore(predicate pg, u64_t val) noexcept { return svdup_n_u64_x(pg, val); }
UTL_SVE_CONST
inline vector_bf16 dup_or_ignore(predicate pg, bf16_t val) noexcept {
    return svdup_n_bf16_x(pg, val);
}
UTL_SVE_CONST
inline vector_f16 dup_or_ignore(predicate pg, f16_t val) noexcept { return svdup_n_f16_x(pg, val); }
UTL_SVE_CONST
inline vector_f32 dup_or_ignore(predicate pg, f32_t val) noexcept { return svdup_n_f32_x(pg, val); }
UTL_SVE_CONST
inline vector_f64 dup_or_ignore(predicate pg, f64_t val) noexcept { return svdup_n_f64_x(pg, val); }
UTL_SVE_CONST
inline vector_s8 dup_or_default(predicate pg, s8_t val, vector_s8 inactive) noexcept {
    return svdup_n_s8_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_s16 dup_or_default(predicate pg, s16_t val, vector_s16 inactive) noexcept {
    return svdup_n_s16_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_s32 dup_or_default(predicate pg, s32_t val, vector_s32 inactive) noexcept {
    return svdup_n_s32_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_s64 dup_or_default(predicate pg, s64_t val, vector_s64 inactive) noexcept {
    return svdup_n_s64_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_u8 dup_or_default(predicate pg, u8_t val, vector_u8 inactive) noexcept {
    return svdup_n_u8_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_u16 dup_or_default(predicate pg, u16_t val, vector_u16 inactive) noexcept {
    return svdup_n_u16_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_u32 dup_or_default(predicate pg, u32_t val, vector_u32 inactive) noexcept {
    return svdup_n_u32_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_u64 dup_or_default(predicate pg, u64_t val, vector_u64 inactive) noexcept {
    return svdup_n_u64_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_bf16 dup_or_default(predicate pg, bf16_t val, vector_bf16 inactive) noexcept {
    return svdup_n_bf16_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_f16 dup_or_default(predicate pg, f16_t val, vector_f16 inactive) noexcept {
    return svdup_n_f16_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_f32 dup_or_default(predicate pg, f32_t val, vector_f32 inactive) noexcept {
    return svdup_n_f32_m(inactive, pg, val);
}
UTL_SVE_CONST
inline vector_f64 dup_or_default(predicate pg, f64_t val, vector_f64 inactive) noexcept {
    return svdup_n_f64_m(inactive, pg, val);
}

template <typename T>
vector<T> zero() noexcept;
// clang-format off
template <>
UTL_SVE_CONST
vector_s8 zero<s8_t>() noexcept { return svdup_n_s8(0); }
template <>
UTL_SVE_CONST
vector_s16 zero<s16_t>() noexcept { return svdup_n_s16(0); }
template <>
UTL_SVE_CONST
vector_s32 zero<s32_t>() noexcept { return svdup_n_s32(0); }
template <>
UTL_SVE_CONST
vector_s64 zero<s64_t>() noexcept { return svdup_n_s64(0); }
template <>
UTL_SVE_CONST
vector_u8 zero<u8_t>() noexcept { return svdup_n_u8(0); }
template <>
UTL_SVE_CONST
vector_u16 zero<u16_t>() noexcept { return svdup_n_u16(0); }
template <>
UTL_SVE_CONST
vector_u32 zero<u32_t>() noexcept { return svdup_n_u32(0); }
template <>
UTL_SVE_CONST
vector_u64 zero<u64_t>() noexcept { return svdup_n_u64(0); }
template <>
UTL_SVE_CONST
vector_bf16 zero<bf16_t>() noexcept { return svdup_n_bf16(0); }
template <>
UTL_SVE_CONST
vector_f16 zero<f16_t>() noexcept { return svdup_n_f16(0); }
template <>
UTL_SVE_CONST
vector_f32 zero<f32_t>() noexcept { return svdup_n_f32(0); }
template <>
UTL_SVE_CONST
vector_f64 zero<f64_t>() noexcept { return svdup_n_f64(0); }
// clang-format on

UTL_SVE_CONST
inline vector_s8 range(s8_t offset, s8_t step) noexcept { return svindex_s8(offset, step); }
UTL_SVE_CONST
inline vector_s16 range(s16_t offset, s16_t step) noexcept { return svindex_s16(offset, step); }
UTL_SVE_CONST
inline vector_s32 range(s32_t offset, s32_t step) noexcept { return svindex_s32(offset, step); }
UTL_SVE_CONST
inline vector_s64 range(s64_t offset, s64_t step) noexcept { return svindex_s64(offset, step); }
UTL_SVE_CONST
inline vector_u8 range(u8_t offset, u8_t step) noexcept { return svindex_u8(offset, step); }
UTL_SVE_CONST
inline vector_u16 range(u16_t offset, u16_t step) noexcept { return svindex_u16(offset, step); }
UTL_SVE_CONST
inline vector_u32 range(u32_t offset, u32_t step) noexcept { return svindex_u32(offset, step); }
UTL_SVE_CONST
inline vector_u64 range(u64_t offset, u64_t step) noexcept { return svindex_u64(offset, step); }

UTL_SVE_PURE
inline vector_s8 load(predicate pg, s8_t const* p) noexcept { return svld1_s8(pg, p); }
UTL_SVE_PURE
inline vector_s16 load(predicate pg, s16_t const* p) noexcept { return svld1_s16(pg, p); }
UTL_SVE_PURE
inline vector_s32 load(predicate pg, s32_t const* p) noexcept { return svld1_s32(pg, p); }
UTL_SVE_PURE
inline vector_s64 load(predicate pg, s64_t const* p) noexcept { return svld1_s64(pg, p); }
UTL_SVE_PURE
inline vector_u8 load(predicate pg, u8_t const* p) noexcept { return svld1_u8(pg, p); }
UTL_SVE_PURE
inline vector_u16 load(predicate pg, u16_t const* p) noexcept { return svld1_u16(pg, p); }
UTL_SVE_PURE
inline vector_u32 load(predicate pg, u32_t const* p) noexcept { return svld1_u32(pg, p); }
UTL_SVE_PURE
inline vector_u64 load(predicate pg, u64_t const* p) noexcept { return svld1_u64(pg, p); }
UTL_SVE_PURE
inline vector_bf16 load(predicate pg, bf16_t const* p) noexcept { return svld1_bf16(pg, p); }
UTL_SVE_PURE
inline vector_f16 load(predicate pg, f16_t const* p) noexcept { return svld1_f16(pg, p); }
UTL_SVE_PURE
inline vector_f32 load(predicate pg, f32_t const* p) noexcept { return svld1_f32(pg, p); }
UTL_SVE_PURE
inline vector_f64 load(predicate pg, f64_t const* p) noexcept { return svld1_f64(pg, p); }

UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, s8_t* dst, vector_s8 src) noexcept { svst1_s8(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, s16_t* dst, vector_s16 src) noexcept { svst1_s16(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, s32_t* dst, vector_s32 src) noexcept { svst1_s32(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, s64_t* dst, vector_s64 src) noexcept { svst1_s64(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, u8_t* dst, vector_u8 src) noexcept { svst1_u8(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, u16_t* dst, vector_u16 src) noexcept { svst1_u16(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, u32_t* dst, vector_u32 src) noexcept { svst1_u32(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, u64_t* dst, vector_u64 src) noexcept { svst1_u64(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, bf16_t* dst, vector_bf16 src) noexcept { svst1_bf16(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, f16_t* dst, vector_f16 src) noexcept { svst1_f16(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, f32_t* dst, vector_f32 src) noexcept { svst1_f32(pg, dst, src); }
UTL_ATTRIBUTES(ALWAYS_INLINE)
inline void store(predicate pg, f64_t* dst, vector_f64 src) noexcept { svst1_f64(pg, dst, src); }
// TODO use span

UTL_SVE_CONST
inline s8_t last(predicate pg, vector_s8 vec) noexcept { return svlastb_s8(pg, vec); }
UTL_SVE_CONST
inline s16_t last(predicate pg, vector_s16 vec) noexcept { return svlastb_s16(pg, vec); }
UTL_SVE_CONST
inline s32_t last(predicate pg, vector_s32 vec) noexcept { return svlastb_s32(pg, vec); }
UTL_SVE_CONST
inline s64_t last(predicate pg, vector_s64 vec) noexcept { return svlastb_s64(pg, vec); }
UTL_SVE_CONST
inline u8_t last(predicate pg, vector_u8 vec) noexcept { return svlastb_u8(pg, vec); }
UTL_SVE_CONST
inline u16_t last(predicate pg, vector_u16 vec) noexcept { return svlastb_u16(pg, vec); }
UTL_SVE_CONST
inline u32_t last(predicate pg, vector_u32 vec) noexcept { return svlastb_u32(pg, vec); }
UTL_SVE_CONST
inline u64_t last(predicate pg, vector_u64 vec) noexcept { return svlastb_u64(pg, vec); }
UTL_SVE_CONST
inline bf16_t last(predicate pg, vector_bf16 vec) noexcept { return svlastb_bf16(pg, vec); }
UTL_SVE_CONST
inline f16_t last(predicate pg, vector_f16 vec) noexcept { return svlastb_f16(pg, vec); }
UTL_SVE_CONST
inline f32_t last(predicate pg, vector_f32 vec) noexcept { return svlastb_f32(pg, vec); }
UTL_SVE_CONST
inline f64_t last(predicate pg, vector_f64 vec) noexcept { return svlastb_f64(pg, vec); }

template <typename To, typename From>
vector<To> reinterpret(vector<From> val) noexcept;
// clang-format off
template <>
UTL_SVE_CONST
vector_s8 reinterpret<s8_t, s8_t>(vector_s8 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_s16 reinterpret<s16_t, s16_t>(vector_s16 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_s32 reinterpret<s32_t, s32_t>(vector_s32 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_s64 reinterpret<s64_t, s64_t>(vector_s64 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_u8 reinterpret<u8_t, u8_t>(vector_u8 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_u16 reinterpret<u16_t, u16_t>(vector_u16 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_u32 reinterpret<u32_t, u32_t>(vector_u32 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_u64 reinterpret<u64_t, u64_t>(vector_u64 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_bf16 reinterpret<bf16_t, bf16_t>(vector_bf16 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_f16 reinterpret<f16_t, f16_t>(vector_f16 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_f32 reinterpret<f32_t, f32_t>(vector_f32 val) noexcept { return val; }
template <>
UTL_SVE_CONST
vector_f64 reinterpret<f64_t, f64_t>(vector_f64 val) noexcept { return val; }
// clang-format on

#  define UTL_SVE_CAST(FROM, TO)                                                            \
      template <>                                                                           \
      UTL_SVE_CONST vector_##TO reinterpret<TO##_t, FROM##_t>(vector_##FROM val) noexcept { \
          return svreinterpret_##TO##_##FROM(val);                                          \
      }                                                                                     \
      static_assert(true, "semi-colon required")

UTL_SVE_CAST(s16, s8);
UTL_SVE_CAST(s32, s8);
UTL_SVE_CAST(s64, s8);
UTL_SVE_CAST(u8, s8);
UTL_SVE_CAST(u16, s8);
UTL_SVE_CAST(u32, s8);
UTL_SVE_CAST(u64, s8);
UTL_SVE_CAST(f16, s8);
UTL_SVE_CAST(bf16, s8);
UTL_SVE_CAST(f32, s8);
UTL_SVE_CAST(f64, s8);

UTL_SVE_CAST(s8, s16);
UTL_SVE_CAST(s32, s16);
UTL_SVE_CAST(s64, s16);
UTL_SVE_CAST(u8, s16);
UTL_SVE_CAST(u16, s16);
UTL_SVE_CAST(u32, s16);
UTL_SVE_CAST(u64, s16);
UTL_SVE_CAST(f16, s16);
UTL_SVE_CAST(bf16, s16);
UTL_SVE_CAST(f32, s16);
UTL_SVE_CAST(f64, s16);

UTL_SVE_CAST(s8, s32);
UTL_SVE_CAST(s16, s32);
UTL_SVE_CAST(s64, s32);
UTL_SVE_CAST(u8, s32);
UTL_SVE_CAST(u16, s32);
UTL_SVE_CAST(u32, s32);
UTL_SVE_CAST(u64, s32);
UTL_SVE_CAST(f16, s32);
UTL_SVE_CAST(bf16, s32);
UTL_SVE_CAST(f32, s32);
UTL_SVE_CAST(f64, s32);

UTL_SVE_CAST(s8, s64);
UTL_SVE_CAST(s16, s64);
UTL_SVE_CAST(s32, s64);
UTL_SVE_CAST(u8, s64);
UTL_SVE_CAST(u16, s64);
UTL_SVE_CAST(u32, s64);
UTL_SVE_CAST(u64, s64);
UTL_SVE_CAST(f16, s64);
UTL_SVE_CAST(bf16, s64);
UTL_SVE_CAST(f32, s64);
UTL_SVE_CAST(f64, s64);

UTL_SVE_CAST(s8, u8);
UTL_SVE_CAST(s16, u8);
UTL_SVE_CAST(s32, u8);
UTL_SVE_CAST(s64, u8);
UTL_SVE_CAST(u16, u8);
UTL_SVE_CAST(u32, u8);
UTL_SVE_CAST(u64, u8);
UTL_SVE_CAST(f16, u8);
UTL_SVE_CAST(bf16, u8);
UTL_SVE_CAST(f32, u8);
UTL_SVE_CAST(f64, u8);

UTL_SVE_CAST(s8, u16);
UTL_SVE_CAST(s16, u16);
UTL_SVE_CAST(s32, u16);
UTL_SVE_CAST(s64, u16);
UTL_SVE_CAST(u8, u16);
UTL_SVE_CAST(u32, u16);
UTL_SVE_CAST(u64, u16);
UTL_SVE_CAST(f16, u16);
UTL_SVE_CAST(bf16, u16);
UTL_SVE_CAST(f32, u16);
UTL_SVE_CAST(f64, u16);

UTL_SVE_CAST(s8, u32);
UTL_SVE_CAST(s16, u32);
UTL_SVE_CAST(s32, u32);
UTL_SVE_CAST(s64, u32);
UTL_SVE_CAST(u8, u32);
UTL_SVE_CAST(u16, u32);
UTL_SVE_CAST(u64, u32);
UTL_SVE_CAST(f16, u32);
UTL_SVE_CAST(bf16, u32);
UTL_SVE_CAST(f32, u32);
UTL_SVE_CAST(f64, u32);

UTL_SVE_CAST(s8, u64);
UTL_SVE_CAST(s16, u64);
UTL_SVE_CAST(s32, u64);
UTL_SVE_CAST(s64, u64);
UTL_SVE_CAST(u8, u64);
UTL_SVE_CAST(u16, u64);
UTL_SVE_CAST(u32, u64);
UTL_SVE_CAST(f16, u64);
UTL_SVE_CAST(bf16, u64);
UTL_SVE_CAST(f32, u64);
UTL_SVE_CAST(f64, u64);

UTL_SVE_CAST(s8, f16);
UTL_SVE_CAST(s16, f16);
UTL_SVE_CAST(s32, f16);
UTL_SVE_CAST(s64, f16);
UTL_SVE_CAST(u8, f16);
UTL_SVE_CAST(u16, f16);
UTL_SVE_CAST(u32, f16);
UTL_SVE_CAST(u64, f16);
UTL_SVE_CAST(bf16, f16);
UTL_SVE_CAST(f32, f16);
UTL_SVE_CAST(f64, f16);

UTL_SVE_CAST(s8, bf16);
UTL_SVE_CAST(s16, bf16);
UTL_SVE_CAST(s32, bf16);
UTL_SVE_CAST(s64, bf16);
UTL_SVE_CAST(u8, bf16);
UTL_SVE_CAST(u16, bf16);
UTL_SVE_CAST(u32, bf16);
UTL_SVE_CAST(u64, bf16);
UTL_SVE_CAST(f16, bf16);
UTL_SVE_CAST(f32, bf16);
UTL_SVE_CAST(f64, bf16);

UTL_SVE_CAST(s8, f32);
UTL_SVE_CAST(s16, f32);
UTL_SVE_CAST(s32, f32);
UTL_SVE_CAST(s64, f32);
UTL_SVE_CAST(u8, f32);
UTL_SVE_CAST(u16, f32);
UTL_SVE_CAST(u32, f32);
UTL_SVE_CAST(u64, f32);
UTL_SVE_CAST(f16, f32);
UTL_SVE_CAST(bf16, f32);
UTL_SVE_CAST(f64, f32);

UTL_SVE_CAST(s8, f64);
UTL_SVE_CAST(s16, f64);
UTL_SVE_CAST(s32, f64);
UTL_SVE_CAST(s64, f64);
UTL_SVE_CAST(u8, f64);
UTL_SVE_CAST(u16, f64);
UTL_SVE_CAST(u32, f64);
UTL_SVE_CAST(u64, f64);
UTL_SVE_CAST(f16, f64);
UTL_SVE_CAST(bf16, f64);
UTL_SVE_CAST(f32, f64);

#  undef UTL_SVE_CAST

#  define UTL_SVE_BINARY_FUNC(RETURN_TYPE, NAME, TYPE, INTRINSIC)             \
      UTL_SVE_CONST                                                           \
      inline RETURN_TYPE NAME(predicate pg, TYPE left, TYPE right) noexcept { \
          return INTRINSIC(pg, left, right);                                  \
      }                                                                       \
      static_assert(true, "semi-colon required after this macro")
#  define UTL_SVE_UNARY_FUNC(RETURN_TYPE, NAME, TYPE, INTRINSIC)                              \
      UTL_SVE_CONST                                                                           \
      inline RETURN_TYPE NAME(predicate pg, TYPE arg) noexcept { return INTRINSIC(pg, arg); } \
      static_assert(true, "semi-colon required after this macro")

UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_u8, svcmpeq_u8);
UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_u16, svcmpeq_u16);
UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_u32, svcmpeq_u32);
UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_u64, svcmpeq_u64);
UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_s8, svcmpeq_s8);
UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_s16, svcmpeq_s16);
UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_s32, svcmpeq_s32);
UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_s64, svcmpeq_s64);
UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_f16, svcmpeq_f16);
UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_f32, svcmpeq_f32);
UTL_SVE_BINARY_FUNC(predicate, cmpeq, vector_f64, svcmpeq_f64);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_u8, svcmpne_u8);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_u16, svcmpne_u16);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_u32, svcmpne_u32);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_u64, svcmpne_u64);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_s8, svcmpne_s8);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_s16, svcmpne_s16);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_s32, svcmpne_s32);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_s64, svcmpne_s64);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_f16, svcmpne_f16);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_f32, svcmpne_f32);
UTL_SVE_BINARY_FUNC(predicate, cmpne, vector_f64, svcmpne_f64);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_u8, svcmplt_u8);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_u16, svcmplt_u16);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_u32, svcmplt_u32);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_u64, svcmplt_u64);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_s8, svcmplt_s8);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_s16, svcmplt_s16);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_s32, svcmplt_s32);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_s64, svcmplt_s64);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_f16, svcmplt_f16);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_f32, svcmplt_f32);
UTL_SVE_BINARY_FUNC(predicate, cmplt, vector_f64, svcmplt_f64);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_u8, svcmple_u8);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_u16, svcmple_u16);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_u32, svcmple_u32);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_u64, svcmple_u64);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_s8, svcmple_s8);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_s16, svcmple_s16);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_s32, svcmple_s32);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_s64, svcmple_s64);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_f16, svcmple_f16);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_f32, svcmple_f32);
UTL_SVE_BINARY_FUNC(predicate, cmple, vector_f64, svcmple_f64);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_u8, svcmpgt_u8);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_u16, svcmpgt_u16);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_u32, svcmpgt_u32);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_u64, svcmpgt_u64);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_s8, svcmpgt_s8);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_s16, svcmpgt_s16);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_s32, svcmpgt_s32);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_s64, svcmpgt_s64);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_f16, svcmpgt_f16);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_f32, svcmpgt_f32);
UTL_SVE_BINARY_FUNC(predicate, cmpgt, vector_f64, svcmpgt_f64);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_u8, svcmpge_u8);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_u16, svcmpge_u16);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_u32, svcmpge_u32);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_u64, svcmpge_u64);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_s8, svcmpge_s8);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_s16, svcmpge_s16);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_s32, svcmpge_s32);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_s64, svcmpge_s64);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_f16, svcmpge_f16);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_f32, svcmpge_f32);
UTL_SVE_BINARY_FUNC(predicate, cmpge, vector_f64, svcmpge_f64);
UTL_SVE_BINARY_FUNC(vector_u8, add, vector_u8, svadd_u8_x);
UTL_SVE_BINARY_FUNC(vector_u16, add, vector_u16, svadd_u16_x);
UTL_SVE_BINARY_FUNC(vector_u32, add, vector_u32, svadd_u32_x);
UTL_SVE_BINARY_FUNC(vector_u64, add, vector_u64, svadd_u64_x);
UTL_SVE_BINARY_FUNC(vector_s8, add, vector_s8, svadd_s8_x);
UTL_SVE_BINARY_FUNC(vector_s16, add, vector_s16, svadd_s16_x);
UTL_SVE_BINARY_FUNC(vector_s32, add, vector_s32, svadd_s32_x);
UTL_SVE_BINARY_FUNC(vector_s64, add, vector_s64, svadd_s64_x);
UTL_SVE_BINARY_FUNC(vector_f16, add, vector_f16, svadd_f16_x);
UTL_SVE_BINARY_FUNC(vector_f32, add, vector_f32, svadd_f32_x);
UTL_SVE_BINARY_FUNC(vector_f64, add, vector_f64, svadd_f64_x);
UTL_SVE_BINARY_FUNC(vector_u8, sub, vector_u8, svsub_u8_x);
UTL_SVE_BINARY_FUNC(vector_u16, sub, vector_u16, svsub_u16_x);
UTL_SVE_BINARY_FUNC(vector_u32, sub, vector_u32, svsub_u32_x);
UTL_SVE_BINARY_FUNC(vector_u64, sub, vector_u64, svsub_u64_x);
UTL_SVE_BINARY_FUNC(vector_s8, sub, vector_s8, svsub_s8_x);
UTL_SVE_BINARY_FUNC(vector_s16, sub, vector_s16, svsub_s16_x);
UTL_SVE_BINARY_FUNC(vector_s32, sub, vector_s32, svsub_s32_x);
UTL_SVE_BINARY_FUNC(vector_s64, sub, vector_s64, svsub_s64_x);
UTL_SVE_BINARY_FUNC(vector_f16, sub, vector_f16, svsub_f16_x);
UTL_SVE_BINARY_FUNC(vector_f32, sub, vector_f32, svsub_f32_x);
UTL_SVE_BINARY_FUNC(vector_f64, sub, vector_f64, svsub_f64_x);
UTL_SVE_BINARY_FUNC(vector_u8, mul, vector_u8, svmul_u8_x);
UTL_SVE_BINARY_FUNC(vector_u16, mul, vector_u16, svmul_u16_x);
UTL_SVE_BINARY_FUNC(vector_u32, mul, vector_u32, svmul_u32_x);
UTL_SVE_BINARY_FUNC(vector_u64, mul, vector_u64, svmul_u64_x);
UTL_SVE_BINARY_FUNC(vector_s8, mul, vector_s8, svmul_s8_x);
UTL_SVE_BINARY_FUNC(vector_s16, mul, vector_s16, svmul_s16_x);
UTL_SVE_BINARY_FUNC(vector_s32, mul, vector_s32, svmul_s32_x);
UTL_SVE_BINARY_FUNC(vector_s64, mul, vector_s64, svmul_s64_x);
UTL_SVE_BINARY_FUNC(vector_f16, mul, vector_f16, svmul_f16_x);
UTL_SVE_BINARY_FUNC(vector_f32, mul, vector_f32, svmul_f32_x);
UTL_SVE_BINARY_FUNC(vector_f64, mul, vector_f64, svmul_f64_x);
UTL_SVE_BINARY_FUNC(vector_u8, div, vector_u8, svdiv_u8_x);
UTL_SVE_BINARY_FUNC(vector_u16, div, vector_u16, svdiv_u16_x);
UTL_SVE_BINARY_FUNC(vector_u32, div, vector_u32, svdiv_u32_x);
UTL_SVE_BINARY_FUNC(vector_u64, div, vector_u64, svdiv_u64_x);
UTL_SVE_BINARY_FUNC(vector_s8, div, vector_s8, svdiv_s8_x);
UTL_SVE_BINARY_FUNC(vector_s16, div, vector_s16, svdiv_s16_x);
UTL_SVE_BINARY_FUNC(vector_s32, div, vector_s32, svdiv_s32_x);
UTL_SVE_BINARY_FUNC(vector_s64, div, vector_s64, svdiv_s64_x);
UTL_SVE_BINARY_FUNC(vector_f16, div, vector_f16, svdiv_f16_x);
UTL_SVE_BINARY_FUNC(vector_f32, div, vector_f32, svdiv_f32_x);
UTL_SVE_BINARY_FUNC(vector_f64, div, vector_f64, svdiv_f64_x);
UTL_SVE_UNARY_FUNC(vector_s8, abs, vector_s8, svabs_s8_x);
UTL_SVE_UNARY_FUNC(vector_s16, abs, vector_s16, svabs_s16_x);
UTL_SVE_UNARY_FUNC(vector_s32, abs, vector_s32, svabs_s32_x);
UTL_SVE_UNARY_FUNC(vector_s64, abs, vector_s64, svabs_s64_x);
UTL_SVE_UNARY_FUNC(vector_f16, abs, vector_f16, svabs_f16_x);
UTL_SVE_UNARY_FUNC(vector_f32, abs, vector_f32, svabs_f32_x);
UTL_SVE_UNARY_FUNC(vector_f64, abs, vector_f64, svabs_f64_x);
UTL_SVE_BINARY_FUNC(vector_u8, bw_and, vector_u8, svand_u8_x);
UTL_SVE_BINARY_FUNC(vector_u16, bw_and, vector_u16, svand_u16_x);
UTL_SVE_BINARY_FUNC(vector_u32, bw_and, vector_u32, svand_u32_x);
UTL_SVE_BINARY_FUNC(vector_u64, bw_and, vector_u64, svand_u64_x);
UTL_SVE_BINARY_FUNC(vector_s8, bw_and, vector_s8, svand_s8_x);
UTL_SVE_BINARY_FUNC(vector_s16, bw_and, vector_s16, svand_s16_x);
UTL_SVE_BINARY_FUNC(vector_s32, bw_and, vector_s32, svand_s32_x);
UTL_SVE_BINARY_FUNC(vector_s64, bw_and, vector_s64, svand_s64_x);
UTL_SVE_BINARY_FUNC(vector_u8, bw_or, vector_u8, svorr_u8_x);
UTL_SVE_BINARY_FUNC(vector_u16, bw_or, vector_u16, svorr_u16_x);
UTL_SVE_BINARY_FUNC(vector_u32, bw_or, vector_u32, svorr_u32_x);
UTL_SVE_BINARY_FUNC(vector_u64, bw_or, vector_u64, svorr_u64_x);
UTL_SVE_BINARY_FUNC(vector_s8, bw_or, vector_s8, svorr_s8_x);
UTL_SVE_BINARY_FUNC(vector_s16, bw_or, vector_s16, svorr_s16_x);
UTL_SVE_BINARY_FUNC(vector_s32, bw_or, vector_s32, svorr_s32_x);
UTL_SVE_BINARY_FUNC(vector_s64, bw_or, vector_s64, svorr_s64_x);
UTL_SVE_BINARY_FUNC(vector_u8, bw_xor, vector_u8, sveor_u8_x);
UTL_SVE_BINARY_FUNC(vector_u16, bw_xor, vector_u16, sveor_u16_x);
UTL_SVE_BINARY_FUNC(vector_u32, bw_xor, vector_u32, sveor_u32_x);
UTL_SVE_BINARY_FUNC(vector_u64, bw_xor, vector_u64, sveor_u64_x);
UTL_SVE_BINARY_FUNC(vector_s8, bw_xor, vector_s8, sveor_s8_x);
UTL_SVE_BINARY_FUNC(vector_s16, bw_xor, vector_s16, sveor_s16_x);
UTL_SVE_BINARY_FUNC(vector_s32, bw_xor, vector_s32, sveor_s32_x);
UTL_SVE_BINARY_FUNC(vector_s64, bw_xor, vector_s64, sveor_s64_x);
UTL_SVE_BINARY_FUNC(vector_u8, bw_not, vector_u8, svnot_u8_x);
UTL_SVE_BINARY_FUNC(vector_u16, bw_not, vector_u16, svnot_u16_x);
UTL_SVE_BINARY_FUNC(vector_u32, bw_not, vector_u32, svnot_u32_x);
UTL_SVE_BINARY_FUNC(vector_u64, bw_not, vector_u64, svnot_u64_x);
UTL_SVE_UNARY_FUNC(vector_s8, bw_not, vector_s8, svnot_s8_x);
UTL_SVE_UNARY_FUNC(vector_s16, bw_not, vector_s16, svnot_s16_x);
UTL_SVE_UNARY_FUNC(vector_s32, bw_not, vector_s32, svnot_s32_x);
UTL_SVE_UNARY_FUNC(vector_s64, bw_not, vector_s64, svnot_s64_x);
UTL_SVE_BINARY_FUNC(vector_u8, bw_bic, vector_u8, svbic_u8_x);
UTL_SVE_BINARY_FUNC(vector_u16, bw_bic, vector_u16, svbic_u16_x);
UTL_SVE_BINARY_FUNC(vector_u32, bw_bic, vector_u32, svbic_u32_x);
UTL_SVE_BINARY_FUNC(vector_u64, bw_bic, vector_u64, svbic_u64_x);
UTL_SVE_BINARY_FUNC(vector_s8, bw_bic, vector_s8, svbic_s8_x);
UTL_SVE_BINARY_FUNC(vector_s16, bw_bic, vector_s16, svbic_s16_x);
UTL_SVE_BINARY_FUNC(vector_s32, bw_bic, vector_s32, svbic_s32_x);
UTL_SVE_BINARY_FUNC(vector_s64, bw_bic, vector_s64, svbic_s64_x);

UTL_SVE_BINARY_FUNC(predicate, bw_and, predicate, svand_b_z);
UTL_SVE_BINARY_FUNC(predicate, bw_or, predicate, svorr_b_z);
UTL_SVE_UNARY_FUNC(predicate, bw_not, predicate, svnot_b_z);
UTL_SVE_BINARY_FUNC(predicate, bw_xor, predicate, sveor_b_z);
UTL_SVE_BINARY_FUNC(predicate, bw_bic, predicate, svbic_b_z);
UTL_SVE_BINARY_FUNC(predicate, bw_orn, predicate, svorn_b_z);
#  undef UTL_SVE_UNARY_FUNC
#  undef UTL_SVE_BINARY_FUNC

UTL_SVE_CONST
inline predicate move(predicate pg, predicate vec) noexcept { return svmov_b_z(pg, vec); }

UTL_SVE_CONST
inline bool test_any(predicate pg, predicate vec) noexcept { return svptest_any(pg, vec); }

template <size_t N>
size_t length() noexcept;
// clang-format off
template <>
UTL_SVE_CONST
size_t length<1>() noexcept { return svcntb(); }
template <>
UTL_SVE_CONST
size_t length<2>() noexcept { return svcnth(); }
template <>
UTL_SVE_CONST
size_t length<4>() noexcept { return svcntw(); }
template <>
UTL_SVE_CONST
size_t length<8>() noexcept { return svcntd(); }

template <size_t N>
predicate all_true() noexcept;
// clang-format off
template <>
UTL_SVE_CONST
predicate all_true<1>() noexcept { return svptrue_b8(); }
template <>
UTL_SVE_CONST
predicate all_true<2>() noexcept { return svptrue_b16(); }
template <>
UTL_SVE_CONST
predicate all_true<4>() noexcept { return svptrue_b32(); }
template <>
UTL_SVE_CONST
predicate all_true<8>() noexcept { return svptrue_b64(); }
// clang-format on

} // namespace details

#  ifdef UTL_SIMD_ARM_SVE_BITS
UTL_INLINE_CXX17 constexpr size_t max_width = UTL_SIMD_ARM_SVE_BITS / CHAR_BIT;
#  else  // ifdef UTL_SIMD_ARM_SVE_BITS
UTL_INLINE_CXX17 constexpr size_t max_width = 2048 / CHAR_BIT;
#  endif // ifdef UTL_SIMD_ARM_SVE_BITS

template <UTL_CONCEPT_CXX20(details::element) T UTL_REQUIRES_CXX11(details::is_element<T>::value)>
UTL_SVE_CONST constexpr size_t max_length() noexcept {
    return max_width / sizeof(T);
}

#  ifdef UTL_SIMD_ARM_SVE_BITS
template <UTL_CONCEPT_CXX20(details::element) T UTL_REQUIRES_CXX11(details::is_element<T>::value)>
constexpr size_t length() noexcept {
    return max_length<sizeof(T)>();
}
#  else  // ifdef UTL_SIMD_ARM_SVE_BITS
template <UTL_CONCEPT_CXX20(details::element) T UTL_REQUIRES_CXX11(details::is_element<T>::value)>
size_t length() noexcept {
    return details::length<sizeof(T)>();
}
#  endif // ifdef UTL_SIMD_ARM_SVE_BITS

template <typename T, size_t N>
using native_array = T[N];

using details::abs;
using details::add;
using details::break_after_true;
using details::break_on_true;
using details::cmpeq;
using details::cmpge;
using details::cmpgt;
using details::cmple;
using details::cmplt;
using details::cmpne;
using details::div;
using details::dup;
using details::dup_or_ignore;
using details::last;
using details::load;
using details::move;
using details::mul;
using details::predicate;
using details::range;
using details::sub;
using details::vector;
using details::zero;

template <UTL_CONCEPT_CXX20(details::element) T UTL_REQUIRES_CXX11(details::is_element<T>::value)>
predicate all_true() noexcept {
    return details::all_true<sizeof(T)>();
}

template <typename To, UTL_CONCEPT_CXX20(details::reinterpretable_as<To>) From>
UTL_SVE_CONST UTL_ENABLE_IF_CXX11(vector<To>, details::is_reinterpretable<From, To>::value)
    reinterpret(vector<From> val) noexcept {
    return details::reinterpret<To>(val);
}

template <UTL_CONCEPT_CXX20(details::element) T, UTL_CONCEPT_CXX20(details::loadable_as<T>) U,
    UTL_REQUIRES_CXX11(!is_same<T, U>::value && details::is_element<T>::value &&
        details::is_loadable<U, T>::value)>
UTL_SVE_PURE vector<T> load(U const* src) noexcept {
    return load((T const*)src);
}

template <UTL_CONCEPT_CXX20(details::element) T, UTL_CONCEPT_CXX20(details::loadable_as<T>) U,
    UTL_REQUIRES_CXX11(!is_same<T, U>::value && details::is_element<T>::value &&
        details::is_loadable<U, T>::value)>
UTL_SVE_PURE mask_vector<T> load(predicate pg, U const* src) noexcept {
    return load(pg, (T const*)src);
}

template <typename T, size_t N>
class alignas(bit_ceil(sizeof(T) * N)) array {
    using native_type = native_array<T>;

public:
    UTL_SVE_CONST
    static constexpr size_t size() noexcept { return N; }

    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE)
    static array store(vector<T> reg) noexcept { return array::store(all_true<T>(), reg); }
    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE)
    static array store(mask_vector<T> reg) noexcept { return array::store(reg.pg, reg); }

    UTL_SVE_CONST
    T& operator[](size_t idx) noexcept { return val[idx]; }
    UTL_SVE_PURE
    T operator[](size_t idx) const noexcept { return val[idx]; }

    UTL_SVE_CONST
    native_type& as_native() noexcept { return val; }
    UTL_SVE_CONST
    native_type const& as_native() const noexcept { return val; }

private:
    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE)
    static array store(predicate pg, vector<T> reg) noexcept {
        using index_type = unsigned_type_t<sizeof(T)>;
        array output;
        pg = details::cmplt(
            pg, details::range(index_type(0), index_type(1)), details::dup(index_type(N)));
        details::store(pg, output.val, reg.val);
        return output;
    }

    native_type val;
};
} // namespace sve
UTL_NAMESPACE_END

#  undef UTL_SVE_CONST
#  undef UTL_SVE_PURE

#endif // ifdef UTL_SIMD_ARM_SVE
