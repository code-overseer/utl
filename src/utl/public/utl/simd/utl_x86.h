// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/utility/utl_seq.h"
#include "utl/utility/utl_signs.h"

UTL_NAMESPACE_BEGIN

#define UTL_X86_CONST UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
#define UTL_X86_PURE UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)

namespace x86 {

using float32_t = float;
using float64_t = double;
enum class mask8_t : uint8_t {
};
enum class mask16_t : uint16_t {
};
enum class mask32_t : uint32_t {
};
enum class mask64_t : uint64_t {
};

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
struct is_element<float32_t> : true_type {};
template <>
struct is_element<float64_t> : true_type {};

UTL_CONSTEVAL bool is_valid_size(size_t n) noexcept {
#ifdef UTL_SIMD_X86_AVX512
    return n == 16 || n == 32 || n == 64;
#elif defined(UTL_SIMD_X86_AVX2)
    return n == 16 || n == 32;
#elif defined(UTL_SIMD_X86_SSE4_2)
    return n == 16;
#else
    return false;
#endif
}

template <typename T, size_t N>
struct is_valid : bool_constant<is_element<T>::value && is_valid_size(sizeof(T) * N)> {};
template <typename From, typename To>
using is_loadable = bool_constant<is_element<To>::value && sizeof(From) == sizeof(To) &&
    (alignof(From) >= alignof(To))>;
template <typename From, typename To>
struct is_reinterpretable : bool_constant<is_element<From>::value && is_element<To>::value> {};

#ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_element_v = is_element<T>::value;
template <typename T, size_t N>
UTL_INLINE_CXX17 constexpr bool is_valid_v = is_valid<T, N>::value;
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_loadable_v = is_loadable<From, To>::value;
template <typename From, typename To>
UTL_INLINE_CXX17 constexpr bool is_reinterpretable_v = is_reinterpretable<From, To>::value;
#endif

#ifdef UTL_CXX20
template <typename T>
concept element = is_element_v<T>;
template <typename T, size_t N>
concept valid = element<T> && is_valid_size(sizeof(T) * N);
template <typename From, typename To>
concept loadable_as = element<To> && (sizeof(From) >= sizeof(To) && alignof(From) >= alignof(To));
template <typename To, typename From>
concept loadable_from = loadable_as<From, To>;
template <typename From, typename To>
concept reinterpretable_as = element<From> && element<To> && is_reinterpretable_v<From, To>;
template <typename To, typename From>
concept reinterpretable_from = element<From> && element<To> && is_reinterpretable_v<From, To>;
#endif
} // namespace traits

namespace details {

template <typename T, size_t N>
struct vector_type;

#ifdef UTL_SIMD_X86_SSE4_2
// clang-format off
template <> struct vector_type<mask8_t, 16> { using type = __m128i; };
template <> struct vector_type<mask16_t, 8> { using type = __m128i; };
template <> struct vector_type<mask32_t, 4> { using type = __m128i; };
template <> struct vector_type<mask64_t, 2> { using type = __m128i; };
template <> struct vector_type<int8_t, 16> { using type = __m128i; };
template <> struct vector_type<int16_t, 8> { using type = __m128i; };
template <> struct vector_type<int32_t, 4> { using type = __m128i; };
template <> struct vector_type<int64_t, 2> { using type = __m128i; };
template <> struct vector_type<uint8_t, 16> { using type = __m128i; };
template <> struct vector_type<uint16_t, 8> { using type = __m128i; };
template <> struct vector_type<uint32_t, 4> { using type = __m128i; };
template <> struct vector_type<uint64_t, 2> { using type = __m128i; };
template <> struct vector_type<float32_t, 4> { using type = __m128; };
template <> struct vector_type<float64_t, 2> { using type = __m128; };
#endif

#ifdef UTL_SIMD_X86_AVX2
template <> struct vector_type<mask8_t, 32> { using type = __m256i; };
template <> struct vector_type<mask16_t, 16> { using type = __m256i; };
template <> struct vector_type<mask32_t, 8> { using type = __m256i; };
template <> struct vector_type<mask64_t, 4> { using type = __m256i; };
template <> struct vector_type<int8_t, 32> { using type = __m256i; };
template <> struct vector_type<int16_t, 16> { using type = __m256i; };
template <> struct vector_type<int32_t, 8> { using type = __m256i; };
template <> struct vector_type<int64_t, 4> { using type = __m256i; };
template <> struct vector_type<uint8_t, 32> { using type = __m256i; };
template <> struct vector_type<uint16_t, 16> { using type = __m256i; };
template <> struct vector_type<uint32_t, 8> { using type = __m256i; };
template <> struct vector_type<uint64_t, 4> { using type = __m256i; };
template <> struct vector_type<float32_t, 8> { using type = __m256; };
template <> struct vector_type<float64_t, 4> { using type = __m256; };
#endif

#ifdef UTL_SIMD_X86_AVX512
template <> struct vector_type<mask8_t, 64> { using type = __m512i; };
template <> struct vector_type<mask16_t, 32> { using type = __m512i; };
template <> struct vector_type<mask32_t, 16> { using type = __m512i; };
template <> struct vector_type<mask64_t, 8> { using type = __m512i; };
template <> struct vector_type<int8_t, 64> { using type = __m512i; };
template <> struct vector_type<int16_t, 32> { using type = __m512i; };
template <> struct vector_type<int32_t, 16> { using type = __m512i; };
template <> struct vector_type<int64_t, 8> { using type = __m512i; };
template <> struct vector_type<uint8_t, 64> { using type = __m512i; };
template <> struct vector_type<uint16_t, 32> { using type = __m512i; };
template <> struct vector_type<uint32_t, 16> { using type = __m512i; };
template <> struct vector_type<uint64_t, 8> { using type = __m512i; };
template <> struct vector_type<float32_t, 16> { using type = __m512; };
template <> struct vector_type<float64_t, 8> { using type = __m512d; };
#endif

template<typename T>
struct mask_type;
template<> struct mask_type<int8_t> { using type = mask8_t; }
template<> struct mask_type<int16_t> { using type = mask16_t; }
template<> struct mask_type<int32_t> { using type = mask32_t; }
template<> struct mask_type<int64_t> { using type = mask64_t; }
template<> struct mask_type<uint8_t> { using type = mask8_t; }
template<> struct mask_type<uint16_t> { using type = mask16_t; }
template<> struct mask_type<uint32_t> { using type = mask32_t; }
template<> struct mask_type<uint64_t> { using type = mask64_t; }
template<> struct mask_type<float32_t> { using type = mask32_t; }
template<> struct mask_type<float64_t> { using type = mask64_t; }
// clang-format on

template <typename T>
using vector_type_t = typename vector_type<T>::type;
template <typename T>
using mask_type_t = typename mask_type<T>::type;
} // namespace details

#ifdef UTL_COMPILER_MSVC
template <typename T, size_t N>
struct vector {
    using native_type = details::vector_type_t<T, N>;
    vector(native_type val) noexcept : val(val) {}
    UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
    operator native_type const&() const noexcept { return val; }
    native_type val;
};
template <typename T, size_t N>
using mask = vector<details::mask_type_t<T>, N>;

template <typename T>
using vector128 = vector<T, 16 / sizeof(T)>;
template <typename T>
using mask128 = vector128<details::mask_type_t<T>>;
template <typename T>
using vector256 = vector<T, 32 / sizeof(T)>;
template <typename T>
using mask256 = vector256<details::mask_type_t<T>>;
template <typename T>
using vector512 = vector<T, 64 / sizeof(T)>;
template <typename T>
using mask512 = vector256<details::mask_type_t<T>>;

UTL_X86_CONST inline vector128<float32_t> set1(float32_t val) noexcept { return _mm_set1_ps(val); }

UTL_X86_CONST inline vector128<float32_t> set1(float64_t val) noexcept { return _mm_set1_pd(val); }

template <UTL_CONCEPT_CXX20(exact_size<1>) T UTL_REQUIRES_CXX11(exact_size<T, 1>::value)>
UTL_X86_CONST inline vector128<T> set1(T val) noexcept {
    return _mm_set1_epi8(to_signed(val));
}

template <UTL_CONCEPT_CXX20(exact_size<2>) T UTL_REQUIRES_CXX11(exact_size<T, 2>::value)>
UTL_X86_CONST inline vector128<T> set1(T val) noexcept {
    return _mm_set1_epi16(to_signed(val));
}

template <UTL_CONCEPT_CXX20(exact_size<4>) T UTL_REQUIRES_CXX11(exact_size<T, 4>::value)>
UTL_X86_CONST inline vector128<T> set1(T val) noexcept {
    return _mm_set1_epi32(to_signed(val));
}

template <UTL_CONCEPT_CXX20(exact_size<8>) T UTL_REQUIRES_CXX11(exact_size<T, 8>::value)>
UTL_X86_CONST inline vector128<T> set1(T val) noexcept {
    return _mm_set1_epi64(__m64(val));
}

#  define UTL_X86_SIMD_BINARY_FUNC(RETURN_TYPE, NAME, TYPE, INTRINSIC)                           \
      UTL_X86_CONST                                                                              \
      inline RETURN_TYPE NAME(TYPE left, TYPE right) noexcept { return INTRINSIC(left, right); } \
      static_assert(true, "semi-colon required after this macro")
#  define UTL_X86_SIMD_UNARY_FUNC(RETURN_TYPE, NAME, TYPE, INTRINSIC)       \
      UTL_X86_CONST                                                         \
      inline RETURN_TYPE NAME(TYPE arg) noexcept { return INTRINSIC(arg); } \
      static_assert(true, "semi-colon required after this macro")

UTL_X86_SIMD_BINARY_FUNC(mask128<int8_t>, operator==, vector128<int8_t>, _mm_cmpeq_epi8);
UTL_X86_SIMD_BINARY_FUNC(mask128<int16_t>, operator==, vector128<int16_t>, _mm_cmpeq_epi16);
UTL_X86_SIMD_BINARY_FUNC(mask128<int32_t>, operator==, vector128<int32_t>, _mm_cmpeq_epi32);
UTL_X86_SIMD_BINARY_FUNC(mask128<int64_t>, operator==, vector128<int64_t>, _mm_cmpeq_epi64);
UTL_X86_SIMD_BINARY_FUNC(mask128<float32_t>, operator==, vector128<float32_t>, _mm_cmpeq_ps);
UTL_X86_SIMD_BINARY_FUNC(mask128<float64_t>, operator==, vector128<float64_t>, _mm_cmpeq_pd);
UTL_X86_SIMD_BINARY_FUNC(mask128<uint8_t>, operator==, vector128<uint8_t>, _mm_cmpeq_epi8);
UTL_X86_SIMD_BINARY_FUNC(mask128<uint16_t>, operator==, vector128<uint16_t>, _mm_cmpeq_epi16);
UTL_X86_SIMD_BINARY_FUNC(mask128<uint32_t>, operator==, vector128<uint32_t>, _mm_cmpeq_epi32);
UTL_X86_SIMD_BINARY_FUNC(mask128<uint64_t>, operator==, vector128<uint64_t>, _mm_cmpeq_epi64);

UTL_X86_SIMD_BINARY_FUNC(mask128<int8_t>, operator<, vector128<int8_t>, _mm_cmplt_epi8);
UTL_X86_SIMD_BINARY_FUNC(mask128<int16_t>, operator<, vector128<int16_t>, _mm_cmplt_epi16);
UTL_X86_SIMD_BINARY_FUNC(mask128<int32_t>, operator<, vector128<int32_t>, _mm_cmplt_epi32);
UTL_X86_SIMD_BINARY_FUNC(mask128<int64_t>, operator<, vector128<int64_t>, _mm_cmplt_epi64);
UTL_X86_SIMD_BINARY_FUNC(mask128<float32_t>, operator<, vector128<float32_t>, _mm_cmplt_ps);
UTL_X86_SIMD_BINARY_FUNC(mask128<float64_t>, operator<, vector128<float64_t>, _mm_cmplt_pd);

UTL_X86_SIMD_BINARY_FUNC(mask128<int8_t>, operator>, vector128<int8_t>, _mm_cmpgt_epi8);
UTL_X86_SIMD_BINARY_FUNC(mask128<int16_t>, operator>, vector128<int16_t>, _mm_cmpgt_epi16);
UTL_X86_SIMD_BINARY_FUNC(mask128<int32_t>, operator>, vector128<int32_t>, _mm_cmpgt_epi32);
UTL_X86_SIMD_BINARY_FUNC(mask128<int64_t>, operator>, vector128<int64_t>, _mm_cmpgt_epi64);
UTL_X86_SIMD_BINARY_FUNC(mask128<float32_t>, operator>, vector128<float32_t>, _mm_cmpgt_ps);
UTL_X86_SIMD_BINARY_FUNC(mask128<float64_t>, operator>, vector128<float64_t>, _mm_cmpgt_pd);

UTL_X86_SIMD_BINARY_FUNC(mask128<float32_t>, operator!=, vector128<float32_t>, _mm_cmpneq_ps);
UTL_X86_SIMD_BINARY_FUNC(mask128<float64_t>, operator!=, vector128<float64_t>, _mm_cmpneq_pd);
UTL_X86_SIMD_BINARY_FUNC(mask128<float32_t>, operator<=, vector128<float32_t>, _mm_cmple_ps);
UTL_X86_SIMD_BINARY_FUNC(mask128<float64_t>, operator<=, vector128<float64_t>, _mm_cmple_pd);
UTL_X86_SIMD_BINARY_FUNC(mask128<float32_t>, operator>=, vector128<float32_t>, _mm_cmpge_ps);
UTL_X86_SIMD_BINARY_FUNC(mask128<float64_t>, operator>=, vector128<float64_t>, _mm_cmpge_pd);

UTL_X86_SIMD_BINARY_FUNC(vector128<mask8_t>, operator^, vector128<mask8_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<mask16_t>, operator^, vector128<mask16_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<mask32_t>, operator^, vector128<mask32_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<mask64_t>, operator^, vector128<mask64_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<int8_t>, operator^, vector128<int8_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<int16_t>, operator^, vector128<int16_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<int32_t>, operator^, vector128<int32_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<int64_t>, operator^, vector128<int64_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<float32_t>, operator^, vector128<float32_t>, _mm_xor_ps);
UTL_X86_SIMD_BINARY_FUNC(vector128<float64_t>, operator^, vector128<float64_t>, _mm_xor_pd);
UTL_X86_SIMD_BINARY_FUNC(vector128<uint8_t>, operator^, vector128<uint8_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<uint16_t>, operator^, vector128<uint16_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<uint32_t>, operator^, vector128<uint32_t>, _mm_xor_si128);
UTL_X86_SIMD_BINARY_FUNC(vector128<uint64_t>, operator^, vector128<uint64_t>, _mm_xor_si128);

#  undef UTL_X86_SIMD_BINARY_FUNC
#  undef UTL_X86_SIMD_UNARY_FUNC

UTL_X86_CONST
inline vector128<mask8_t> operator~(vector128<mask8_t> arg) noexcept { return (arg == arg) ^ arg; }
UTL_X86_CONST
inline vector128<mask16_t> operator~(vector128<mask16_t> arg) noexcept {
    return (arg == arg) ^ arg;
}
UTL_X86_CONST
inline vector128<mask32_t> operator~(vector128<mask32_t> arg) noexcept {
    return (arg == arg) ^ arg;
}
UTL_X86_CONST
inline vector128<mask64_t> operator~(vector128<mask64_t> arg) noexcept {
    return (arg == arg) ^ arg;
}
UTL_X86_CONST
inline vector128<int8_t> operator~(vector128<int8_t> arg) noexcept { return (arg == arg) ^ arg; }
UTL_X86_CONST
inline vector128<int16_t> operator~(vector128<int16_t> arg) noexcept { return (arg == arg) ^ arg; }
UTL_X86_CONST
inline vector128<int32_t> operator~(vector128<int32_t> arg) noexcept { return (arg == arg) ^ arg; }
UTL_X86_CONST
inline vector128<int64_t> operator~(vector128<int64_t> arg) noexcept { return (arg == arg) ^ arg; }
UTL_X86_CONST
inline vector128<uint8_t> operator~(vector128<uint8_t> arg) noexcept { return (arg == arg) ^ arg; }
UTL_X86_CONST
inline vector128<uint16_t> operator~(vector128<uint16_t> arg) noexcept {
    return (arg == arg) ^ arg;
}
UTL_X86_CONST
inline vector128<uint32_t> operator~(vector128<uint32_t> arg) noexcept {
    return (arg == arg) ^ arg;
}
UTL_X86_CONST
inline vector128<uint64_t> operator~(vector128<uint64_t> arg) noexcept {
    return (arg == arg) ^ arg;
}
UTL_X86_CONST
inline mask128<int16_t> operator!=(vector128<int16_t> left, vector128<int16_t> right) noexcept {
    return ~(left == right);
}
UTL_X86_CONST
inline mask128<int32_t> operator!=(vector128<int32_t> left, vector128<int32_t> right) noexcept {
    return ~(left == right);
}
UTL_X86_CONST
inline mask128<int64_t> operator!=(vector128<int64_t> left, vector128<int64_t> right) noexcept {
    return ~(left == right);
}
UTL_X86_CONST
inline mask128<int8_t> operator!=(vector128<int8_t> left, vector128<int8_t> right) noexcept {
    return ~(left == right);
}
UTL_X86_CONST
inline mask128<int16_t> operator!=(vector128<int16_t> left, vector128<int16_t> right) noexcept {
    return ~(left == right);
}
UTL_X86_CONST
inline mask128<int32_t> operator!=(vector128<int32_t> left, vector128<int32_t> right) noexcept {
    return ~(left == right);
}
UTL_X86_CONST
inline mask128<int64_t> operator!=(vector128<int64_t> left, vector128<int64_t> right) noexcept {
    return ~(left == right);
}
UTL_X86_CONST
inline mask128<int8_t> operator<=(vector128<int8_t> left, vector128<int8_t> right) noexcept {
    return ~(right < left);
}
UTL_X86_CONST
inline mask128<int16_t> operator<=(vector128<int16_t> left, vector128<int16_t> right) noexcept {
    return ~(right < left);
}
UTL_X86_CONST
inline mask128<int32_t> operator<=(vector128<int32_t> left, vector128<int32_t> right) noexcept {
    return ~(right < left);
}
UTL_X86_CONST
inline mask128<int64_t> operator<=(vector128<int64_t> left, vector128<int64_t> right) noexcept {
    return ~(right < left);
}
UTL_X86_CONST
inline mask128<int8_t> operator<=(vector128<int8_t> left, vector128<int8_t> right) noexcept {
    return ~(right < left);
}
UTL_X86_CONST
inline mask128<int16_t> operator<=(vector128<int16_t> left, vector128<int16_t> right) noexcept {
    return ~(right < left);
}
UTL_X86_CONST
inline mask128<int32_t> operator<=(vector128<int32_t> left, vector128<int32_t> right) noexcept {
    return ~(right < left);
}
UTL_X86_CONST
inline mask128<int64_t> operator<=(vector128<int64_t> left, vector128<int64_t> right) noexcept {
    return ~(right < left);
}
UTL_X86_CONST
inline mask128<int8_t> operator>=(vector128<int8_t> left, vector128<int8_t> right) noexcept {
    return ~(left < right);
}
UTL_X86_CONST
inline mask128<int16_t> operator>=(vector128<int16_t> left, vector128<int16_t> right) noexcept {
    return ~(left < right);
}
UTL_X86_CONST
inline mask128<int32_t> operator>=(vector128<int32_t> left, vector128<int32_t> right) noexcept {
    return ~(left < right);
}
UTL_X86_CONST
inline mask128<int64_t> operator>=(vector128<int64_t> left, vector128<int64_t> right) noexcept {
    return ~(left < right);
}
UTL_X86_CONST
inline mask128<int8_t> operator>=(vector128<int8_t> left, vector128<int8_t> right) noexcept {
    return ~(left < right);
}
UTL_X86_CONST
inline mask128<int16_t> operator>=(vector128<int16_t> left, vector128<int16_t> right) noexcept {
    return ~(left < right);
}
UTL_X86_CONST
inline mask128<int32_t> operator>=(vector128<int32_t> left, vector128<int32_t> right) noexcept {
    return ~(left < right);
}
UTL_X86_CONST
inline mask128<int64_t> operator>=(vector128<int64_t> left, vector128<int64_t> right) noexcept {
    return ~(left < right);
}

UTL_X86_CONST
int move_mask(vector128<int8_t> mask) noexcept { return _mm_movemask_epi8(mask); }
UTL_X86_CONST
int move_mask(vector128<uint8_t> mask) noexcept { return _mm_movemask_epi8(mask); }
UTL_X86_CONST
int move_mask(vector128<mask8_t> mask) noexcept { return _mm_movemask_epi8(mask); }
UTL_X86_CONST
int move_mask(vector128<float32_t> mask) noexcept { return _mm_movemask_ps(mask); }
UTL_X86_CONST
int move_mask(vector128<float64_t> mask) noexcept { return _mm_movemask_pd(mask); }

#else // ifdef UTL_COMPILER_MSVC

template <typename T, size_t N>
using vector = details::vector_type_t<T, N>;
template <typename T, size_t N>
using mask = vector<details::mask_type_t<T>, N>;

#endif // ifdef UTL_COMPILER_MSVC

} // namespace x86

#undef UTL_X86_CONST
#undef UTL_X86_PURE

UTL_NAMESPACE_END
