// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/bit/utl_countr_zero.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/string/utl_libc_common.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_trivially_copyable.h"
#include "utl/type_traits/utl_is_unsigned.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_signed_type.h"
#include "utl/type_traits/utl_unsigned_type.h"
#include "utl/utility/utl_seq.h"
#include "utl/utility/utl_signs.h"

UTL_NAMESPACE_BEGIN

namespace libc {
namespace runtime {
namespace standard {

template <UTL_CONCEPT_CXX20(trivially_copyable)
        T UTL_REQUIRES_CXX11(is_trivially_copyable<T>::value)>
T* memcpy(T* UTL_RESTRICT dst, T const* UTL_RESTRICT src, element_count_t count) noexcept {
#if UTL_HAS_BUILTIN(__builtin_memcpy)
    return (T*)__builtin_memcpy(dst, src, byte_count(src, count));
#else
    return (T*)::memcpy(dst, src, byte_count(src, count));
#endif
}

template <UTL_CONCEPT_CXX20(trivially_copyable)
        T UTL_REQUIRES_CXX11(is_trivially_copyable<T>::value)>
T* memmove(T* dst, T const* src, element_count_t count) noexcept {
#if UTL_HAS_BUILTIN(__builtin_memcpy)
    return (T*)__builtin_memmove(dst, src, byte_count(dst, count));
#else
    return (T*)::memmove(dst, src, byte_count(src, count));
#endif
}

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
UTL_ATTRIBUTE(NODISCARD, PURE)
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
#if UTL_HAS_BUILTIN(__builtin_char_memchr)
    return (T*)__builtin_char_memchr(ptr, value, bytes);
#else
    return (T*)::memchr(ptr, value, bytes);
#endif
}

template <typename T, typename U>
UTL_ATTRIBUTE(NODISCARD, PURE)
int memcmp(T const* lhs, U const* rhs, element_count_t count) noexcept {
    static_assert(is_trivially_lexicographically_comparable<T, U>::value,
        "Types must be lexicographically comparable");
#if UTL_HAS_BUILTIN(__builtin_char_memchr)
    return __builtin_memcmp(lhs, rhs, byte_count(lhs, count));
#else
    return ::memcmp(lhs, rhs, byte_count(lhs, count));
#endif
}

UTL_ATTRIBUTES(NODISCARD, PURE)
size_t strlen(char const* str) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strlen)
    return __builtin_strlen(str);
#else
    return ::strlen(str);
#endif
}

UTL_ATTRIBUTES(NODISCARD, PURE)
size_t strlen(wchar_t const* str) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcslen)
    return __builtin_wcslen(str);
#else
    return ::wcslen(str);
#endif
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
size_t strlen(CharType const* str) noexcept {
    size_t count = 0;
    while (*str) {
        ++str;
        ++count;
    }

    return count;
}

UTL_ATTRIBUTES(NODISCARD, PURE)
T* strchr(char const* str, char const ch) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strchr)
    return (T*)__builtin_strchr(str, ch);
#else
    return (T*)::strchr(str, ch);
#endif
}

UTL_ATTRIBUTES(NODISCARD, PURE)
wchar_t* strchr(wchar_t const* str, wchar_t const ch) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcschr)
    return __builtin_wcschr(str, ch);
#else
    return ::wcschr(str, ch);
#endif
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
T* strchr(CharType const* str, CharType const ch) noexcept {
    while (*str != ch) {
        if (!*str) {
            return nullptr;
        }
        ++str;
    }

    return (T*)str;
}

UTL_ATTRIBUTE(NODISCARD, PURE)
int strcmp(char const* left, char const* right) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strcmp)
    return __builtin_strcmp(left, right);
#else
    return ::strcmp(left, right);
#endif
}

UTL_ATTRIBUTE(NODISCARD, PURE)
int strcmp(wchar_t const* left, wchar_t const* right) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcscmp)
    return __builtin_wcscmp(left, right);
#else
    return ::wcscmp(left, right);
#endif
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTE(NODISCARD, PURE)
int strcmp(CharType const* left, CharType const* right) noexcept {
    while (*left == *right) {
        if (!*left && !*right) {
            return 0;
        }

        ++left;
        ++right;
    }

    return (*left < *right) ? -1 : 1;
}

UTL_ATTRIBUTE(NODISCARD, PURE)
int strncmp(char const* left, char const* right, size_t len) noexcept {
#if UTL_HAS_BUILTIN(__builtin_strcmp)
    return __builtin_strncmp(left, right, len);
#else
    return ::strncmp(left, right, len);
#endif
}

UTL_ATTRIBUTE(NODISCARD, PURE)
int strncmp(wchar_t const* left, wchar_t const* right, size_t len) noexcept {
#if UTL_HAS_BUILTIN(__builtin_wcscmp)
    return __builtin_wcsncmp(left, right, len);
#else
    return ::wcsncmp(left, right, len);
#endif
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTE(NODISCARD, PURE)
int strncmp(T const* left, T const* right, size_t len) noexcept {
    while (len) {
        if (*left != *right) {
            return (*left < *right) ? -1 : 1;
        }

        if (!*left && !*right) {
            return 0;
        }

        ++left;
        ++right;
        --len;
    }

    return 0;
}
} // namespace standard

#if defined(UTL_SUPPORTS_SIMD_INTRINSICS) && !defined(UTL_DISABLE_SIMD_LIBC)
namespace simd {
UTL_INLINE_CXX17 constexpr size_t npos = (size_t)-1;

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
UTL_CONSTEVAL size_t bit_count() noexcept {
    return sizeof(T) * CHAR_BIT;
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr T min(T l, T r) noexcept {
    return l < r ? l : r;
}

namespace x86 {
#  if defined(UTL_SIMD_X86_AVX512) && !defined(UTL_DISABLE_AVX512_LIBC)

namespace mm512 {
template <UTL_CONCEPT_CXX20(exact_size<1>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 1>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m512i set1(CharType const ch) noexcept {
    return _mm512_set1_epi8(*reinterpret_cast<char const*>(&ch));
}

template <UTL_CONCEPT_CXX20(exact_size<2>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 2>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m512i set1(CharType const ch) noexcept {
    return _mm512_set1_epi16(*reinterpret_cast<short const*>(&ch));
}

template <UTL_CONCEPT_CXX20(exact_size<4>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 4>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m512i set1(CharType const ch) noexcept {
    return _mm512_set1_epi32(*reinterpret_cast<int const*>(&ch));
}
template <UTL_CONCEPT_CXX20(exact_size<1>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 1>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__mmask64 cmpeq(__m512i left, __m512i right) noexcept {
    return _mm512_cmpeq_epu8_mask(left, right);
}

template <UTL_CONCEPT_CXX20(exact_size<2>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 2>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__mmask64 cmpeq(__m512i left, __m512i right) noexcept {
    return _mm512_cmpeq_epu16_mask(left, right);
}

template <UTL_CONCEPT_CXX20(exact_size<4>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 4>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__mmask64 cmpeq(__m512i left, __m512i right) noexcept {
    return _mm512_cmpeq_epu32_mask(left, right);
}
} // namespace mm512

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
    using register_t = __m512i;
    static constexpr size_t char_per_register = sizeof(register_t);
    register_t needles = _mm512_set1_epi8(*reinterpret_cast<char const*>(&value));

    for (T const* const end = ptr + bytes; ptr < end; ptr += char_per_register) {
        register_t haystack = _mm512_loadu_si512(ptr);
        __mmask64 mask = _mm512_cmpeq_epu8_mask(haystack, needles);
        size_t const idx = countr_zero(_cvtmask64_u64(mask));
        if (idx < bit_count<uint64_t>()) {
            ptr += idx;
            return ptr < end ? const_cast<T*>(ptr) : nullptr;
        }
    }

    return nullptr;
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
size_t strlen(CharType const* src) noexcept {
    using register_t = __m512i;
    static constexpr size_t char_per_register = sizeof(register_t) / sizeof(CharType);
    register_t zvec = _mm512_setzero_si512();

    for (size_t offset = 0;; offset += char_per_register) {
        register_t value = _mm512_loadu_si512(src + offset);
        __mmask64 mask = mm512::cmpeq<CharType>(value, zvec);
        size_t const idx = countr_zero(_cvtmask64_u64(mask));
        if (idx < bit_count<uint64_t>()) {
            return offset + idx / sizeof(CharType);
        }
    }

    UTL_BUILTIN_unreachable();
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
CharType* strchr(CharType const* str, CharType const ch) noexcept {
    using register_t = __m512i;
    static constexpr size_t char_per_register = sizeof(register_t) / sizeof(CharType);
    register_t const needles = mm512::set1(*reinterpret_cast<CharType const*>(&value));
    register_t const zvec = _mm512_setzero_si512();

    for (size_t offset = 0;; offset += char_per_register) {
        register_t value = _mm512_loadu_si512(str + offset);
        __mmask64 end_mask = mm512::cmpeq<CharType>(value, zvec);
        __mmask64 ch_mask = mm512::cmpeq<CharType>(value, needles);
        size_t const idx = countr_zero(_cvtmask64_u64(_kor_mask64(end_mask, ch_mask)));
        if (idx < bit_count<uint64_t>()) {
            str += offset + idx / sizeof(CharType);
            return *str == ch ? const_cast<CharType*>(str) : nullptr;
        }
    }

    UTL_BUILTIN_unreachable();
}

#  elif defined(UTL_SIMD_X86_AVX2) && !defined(UTL_DISABLE_AVX_LIBC)

namespace mm256 {
template <UTL_CONCEPT_CXX20(exact_size<1>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 1>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m256i set1(CharType const ch) noexcept {
    _mm256_set1_epi8(*reinterpret_cast<char const*>(&ch));
}

template <UTL_CONCEPT_CXX20(exact_size<2>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 2>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m256i set1(CharType const ch) noexcept {
    _mm256_set1_epi16(*reinterpret_cast<short const*>(&ch));
}

template <UTL_CONCEPT_CXX20(exact_size<4>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 4>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m256i set1(CharType const ch) noexcept {
    _mm256_set1_epi32(*reinterpret_cast<int const*>(&ch));
}

template <UTL_CONCEPT_CXX20(exact_size<1>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 1>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m256i cmpeq(__m256i left, __m256i right) noexcept {
    return _mm256_cmpeq_epi8(left, right);
}

template <UTL_CONCEPT_CXX20(exact_size<2>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 2>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m256i cmpeq(__m256i left, __m256i right) noexcept {
    return _mm256_cmpeq_epi16(left, right);
}

template <UTL_CONCEPT_CXX20(exact_size<4>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 4>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m256i cmpeq(__m256i left, __m256i right) noexcept {
    return _mm256_cmpeq_epi32(left, right);
}
} // namespace mm256

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
    using register_t = __m256i;
    static constexpr size_t char_per_register = sizeof(register_t);
    register_t needles = mm256_set1_epi8(*reinterpret_cast<char const*>(&value));
    for (T const* const end = ptr + bytes; ptr < end; ptr += char_per_register) {
        register_t haystack = _mm256_loadu_si256((register_t const*)(ptr));
        int const mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(haystack, needles));
        size_t const idx = countr_zero(to_unsigned(mask));
        if (idx < bit_count<decltype(mask)>()) {
            ptr += idx;
            return ptr < end ? const_cast<T*>(ptr) : nullptr;
        }
    }

    return nullptr;
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
size_t strlen(CharType const* str) noexcept {
    using register_t = __m256i;
    static constexpr size_t char_per_register = sizeof(register_t) / sizeof(CharType);
    register_t zvec = _mm256_setzero_si256();

    for (size_t offset = 0;; offset += char_per_register) {
        register_t value = _mm256_loadu_si256((register_t const*)(str + offset));
        int const mask = _mm256_movemask_epi8(mm256::cmpeq<CharType>(value, zvec));
        size_t const idx = countr_zero(to_unsigned(mask));
        if (idx < bit_count<uint64_t>()) {
            return offset + idx / sizeof(CharType);
        }
    }

    UTL_BUILTIN_unreachable();
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
CharType* strchr(CharType const* str, CharType const ch) noexcept {
    using register_t = __m256i;
    static constexpr size_t char_per_register = sizeof(register_t) / sizeof(CharType);
    register_t const needles = mm256::set1(*reinterpret_cast<CharType const*>(&value));
    register_t const zvec = _mm256_setzero_si256();

    for (size_t offset = 0;; offset += char_per_register) {
        register_t value = _mm256_loadu_si256((register_t const*)(str + offset));
        int const mask = _mm256_movemask_epi8(_mm256_or_si256(
            mm256::cmpeq<CharType>(value, zvec), mm256::cmpeq<CharType>(value, needles)));
        size_t const idx = countr_zero(to_unsigned(mask));
        if (idx < bit_count<uint32_t>()) {
            str += offset + idx / sizeof(CharType);
            return *str == ch ? const_cast<CharType*>(str) : nullptr;
        }
    }

    UTL_BUILTIN_unreachable();
}

#  elif defined(UTL_SIMD_X86_SSE4_2)

namespace mm128 {
template <UTL_CONCEPT_CXX20(exact_size<1>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 1>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m128i set1(CharType const ch) noexcept {
    _mm128_set1_epi8(*reinterpret_cast<char const*>(&ch));
}

template <UTL_CONCEPT_CXX20(exact_size<2>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 2>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m128i set1(CharType const ch) noexcept {
    _mm128_set1_epi16(*reinterpret_cast<short const*>(&ch));
}

template <UTL_CONCEPT_CXX20(exact_size<4>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 4>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m128i set1(CharType const ch) noexcept {
    _mm128_set1_epi32(*reinterpret_cast<int const*>(&ch));
}

template <UTL_CONCEPT_CXX20(exact_size<1>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 1>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m128i cmpeq(__m128i left, __m128i right) noexcept {
    return _mm_cmpeq_epi8(left, right);
}

template <UTL_CONCEPT_CXX20(exact_size<2>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 2>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m128i cmpeq(__m128i left, __m128i right) noexcept {
    return _mm_cmpeq_epi16(left, right);
}

template <UTL_CONCEPT_CXX20(exact_size<4>)
        CharType UTL_REQUIRES_CXX11(exact_size<CharType, 4>::value)>
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
__m128i cmpeq(__m128i left, __m128i right) noexcept {
    return _mm_cmpeq_epi32(left, right);
}
} // namespace mm128

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
    using register_t = __m128i;
    static constexpr size_t char_per_register = sizeof(register_t);
    register_t needles = _mm_set1_epi8(*reinterpret_cast<char const*>(&value));
    for (T const* const end = ptr + bytes; ptr < end; ptr += char_per_register) {
        register_t haystack = _mm_loadu_si128((register_t const*)(ptr));
        int const mask = _mm_movemask_epi8(_mm_cmpeq_epi8(haystack, needles));
        size_t const idx = countr_zero(to_unsigned(mask));
        if (idx < bit_count<uint32_t>()) {
            ptr += idx;
            return ptr < end ? const_cast<T*>(ptr) : nullptr;
        }
    }

    return nullptr;
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
size_t strlen(CharType const* src) noexcept {
    using register_t = __m128i;
    static constexpr size_t char_per_register = sizeof(register_t) / sizeof(CharType);
    register_t zvec = _mm_setzero_si128();

    for (size_t offset = 0;; offset += char_per_register) {
        register_t value = _mm_loadu_si128((register_t const*)(src + offset));
        int const mask = _mm_movemask_epi8(mm128::cmpeq<CharType>(value, zvec));
        size_t const idx = countr_zero(to_unsigned(mask));
        if (idx < bit_count<uint32_t>()) {
            return offset + idx / sizeof(CharType);
        }
    }

    UTL_BUILTIN_unreachable();
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
CharType* strchr(CharType const* str, CharType const ch) noexcept {
    using register_t = __m128i;
    static constexpr size_t char_per_register = sizeof(register_t) / sizeof(CharType);
    register_t const needles = mm128::set1(*reinterpret_cast<CharType const*>(&value));
    register_t const zvec = _mm_setzero_si128();

    for (size_t offset = 0;; offset += char_per_register) {
        register_t value = _mm_loadu_si128((register_t const*)(str + offset));
        int const mask = _mm_movemask_epi8(_mm_or_si128(
            mm128::cmpeq<CharType>(value, zvec), mm128::cmpeq<CharType>(value, needles)));
        size_t const idx = countr_zero(to_unsigned(mask));
        if (idx < bit_count<uint32_t>()) {
            str += offset + idx / sizeof(CharType);
            return *str == ch ? const_cast<CharType*>(str) : nullptr;
        }
    }

    UTL_BUILTIN_unreachable();
}

#  endif // elif defined(UTL_SIMD_X86_SSE4_2)
} // namespace x86

namespace arm {
#  if defined(UTL_SIMD_ARM_SVE) && !defined(UTL_DISABLE_SVE_LIBC)

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
    using element_t = uint8_t;
    auto const needles = sve::dup(*((element_t const*)(&value)));
    auto const indices = sve::range(element_t(0), element_t(1));
    T const* const end = ptr + bytes;
    size_t const register_width = sve::length<element_t>();
    do {
        auto const count = sve::dup((element_t)min<size_t>(end - ptr, register_width));
        sve::predicate const active = sve::cmplt(sve::all_true<element_t>(), indices, count);
        auto const haystack = sve::load<element_t>(active, ptr);
        sve::predicate const is_needle = sve::cmpeq(active, haystack, needle);
        if (sve::test_any(active, is_needle)) {
            ptr += sve::last(sve::break_after_true(active, is_needle), indices);
            return ptr < end ? const_cast<T*>(ptr) : nullptr;
        }
        ptr += count;
    } while (ptr < end);

    return nullptr;
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
size_t strlen(CharType const* const src) noexcept {
    using element_t = unsigned_type_t<CharType>;
    size_t const window_size = sve::length<element_t>();
    auto const active = sve::all_true<element_t>();
    for (size_t offset = 0;; offset += window_size) {
        auto const window = sve::load<element_t>(active, src + offset);
        auto const is_zero = sve::cmpeq(active, window, sve::zero<element_t>());
        if (sve::test_any(active, is_zero)) {
            auto const indices = sve::range(element_t(0), element_t(1));
            return offset + sve::last(sve::break_after_true(active, is_zero), indices);
        }
    }

    UTL_BUILTIN_unreachable();
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
size_t strchr(CharType const* str, CharType const ch) noexcept {
    using element_t = unsigned_type_t<CharType>;
    size_t const window_size = sve::length<element_t>();
    sve::predicate const active = sve::all_true<element_t>();
    auto const needles = sve::dup(*((element_t const*)&ch));
    for (;; str += window_size) {
        auto const window = sve::load<element_t>(active, str);
        sve::predicate const is_zero = sve::cmpeq(active, window, sve::zero<element_t>());
        sve::predicate const is_ch = sve::cmpeq(active, window, needles);
        sve::predicate const mask = sve::bw_or(active, is_zero, is_ch);
        if (sve::test_any(active, mask)) {
            auto const indices = sve::range(element_t(0), element_t(1));
            str += sve::last(sve::break_after_true(active, mask), indices);
            return *str == ch ? const_cast<CharType*>(src) : nullptr;
        }
    }

    UTL_BUILTIN_unreachable();
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
int strcmp(CharType const* lhs, CharType const* rhs) noexcept {
    using element_t = signed_type_t<CharType>;
    size_t const window_size = sve::length<element_t>();
    for (sve::predicate active = sve::all_true<element_t>();;
         lhs += window_size, rhs += window_size) {
        auto const left = sve::load<element_t>(active, lhs);
        auto const right = sve::load<element_t>(active, rhs);

        sve::predicate const is_ended =
            sve::cmpeq(active, sve::mul(active, left, right), sve::zero<element_t>());
        active = sve::break_after_true(active, is_ended);

        auto const diff = sve::cmpeq(active, left, right);
        sve::predicate const has_diff = sve::cmpne(active, diff, sve::zero<element_t>());
        if (sve::test_any(sve::bw_or(active, is_ended, has_diff))) {
            return sve::last(sve::break_after_true(active, has_diff), diff);
        }
    }

    UTL_BUILTIN_unreachable();
}

#  elif defined(UTL_SIMD_ARM_NEON) && !defined(UTL_DISABLE_NEON_LIBC)

namespace details {
template <size_t N>
UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
UTL_CONSTEXPR_CXX14 size_t countr_zero(uint64_t a[N]) noexcept {
    for (auto i = 0; i < N; ++i) {
        if (a[i]) {
            return i * bit_count<uint64_t>() + countr_zero(a[i]);
        }
    }

    return bit_count<uint64_t>() * N;
}

} // namespace details

template <UTL_CONCEPT_CXX20(exact_size<1>) T,
    UTL_CONCEPT_CXX20(exact_size<1>)
        U UTL_REQUIRES_CXX11(exact_size<T, 1>::value&& exact_size<U, 1>::value)>
T* memchr(T const* ptr, U value, size_t bytes) noexcept {
    using element_t = uint8_t;
    T const* const end = ptr + bytes;
    auto const needles = neon::qregister::dup<uint8_t>(*(uint8_t const*)(&value));
    do {
        auto const window = neon::qregister::load<element_t>(ptr);
        auto const result = neon::qregister::store_as<uint64_t>(window == needles);
        size_t const window_offset =
            details::countr_zero(result.as_native()) / bit_count<element_t>();
        if (window_offset < neon::qregister::length<element_t>()) {
            ptr += window_offset;
            return ptr < end ? const_cast<char*>(ptr) : nullptr;
        }
        ptr += neon::qregister::length<element_t>();
    } while (ptr < end);

    return nullptr;
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
size_t strlen(CharType const* str) noexcept {
    using element_t = unsigned_type_t<sizeof(CharType)>;
    auto const zvec = neon::qregister::zero<element_t>();

    for (size_t offset = 0;; offset += neon::qregister::length<element_t>()) {
        auto const window = neon::qregister::load<element_t>(str + offset);
        auto const result = neon::qregister::store_as<uint64_t>(window == zvec);
        size_t const window_offset =
            details::countr_zero(result.as_native()) / bit_count<element_t>();
        if (window_offset < neon::qregister::length<element_t>()) {
            return offset + window_offset;
        }
    }

    UTL_BUILTIN_unreachable();
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
CharType* strchr(CharType const* str, CharType const ch) noexcept {
    using element_t = unsigned_type_t<sizeof(CharType)>;
    auto const zvec = neon::qregister::zero<element_t>();
    auto const needles = neon::qregister::dup<element_t>(*((element_t const*)&ch));

    for (;; str += neon::qregister::length<element_t>()) {
        register_t const window = neon::qregister::load<element_t>(str + offset);
        auto const result =
            neon::qregister::store_as<uint64_t>((window == zvec) | (window == needles));
        size_t const window_offset =
            details::countr_zero(result.as_native()) / bit_count<element_t>();
        if (window_offset < neon::qregister::length<element_t>()) {
            str += window_offset;
            return *str == ch ? const_cast<char*>(str) : nullptr;
        }
    }

    UTL_BUILTIN_unreachable();
}

template <UTL_CONCEPT_CXX20(string_char)
        CharType UTL_REQUIRES_CXX11(is_string_char<CharType>::value)>
UTL_ATTRIBUTES(NODISCARD, PURE)
int strcmp(CharType const* lhs, CharType const* rhs) noexcept {
    // use signed element
    using element_t = signed_type_t<sizeof(CharType)>;
    static constexpr size_t elements = neon::qregister::length<element_t>();

    auto const zvec = neon::qregister::zero<element_t>();
    for (;; lhs += elements, rhs += elements) {
        auto const left = neon::qregister::load<element_t>(lhs);
        auto const right = neon::qregister::load<element_t>(rhs);
        auto const is_neq = neon::qregister::store_as<uint64_t>(left - right);
        auto const is_end = neon::qregister::store_as<uint64_t>((left * right) == zvec);
        size_t const first_neq = details::countr_zero(is_neq.as_native()) / bit_count<element_t>();
        size_t const first_zero = details::countr_zero(is_end.as_native()) / bit_count<element_t>();
        if (first_zero < first_neq) {
            // If first_zero < first_neq: both left and right are zero in the same location
            // If only one side is zero, then we have first_zero == first_neq
            return 0;
        }

        if (first_neq < elements) {
            static_assert(UTL_SCOPE little_endian(), "Platform must be little endian");
            element_t const* val =
                reinterpret_cast<element_t const*>(is_neq.as_native()) + first_neq;
            return *val;
        }
    }

    UTL_BUILTIN_unreachable();
}

#  endif
} // namespace arm

#  ifdef UTL_ARCH_x86
using x86::memchr;
using x86::memcmp;
using x86::strchr;
using x86::strcmp;
using x86::strlen;
using x86::strncmp;
#  elif defined(UTL_ARCH_ARM)
using arm::memchr;
using arm::memcmp;
using arm::strchr;
using arm::strcmp;
using arm::strlen;
using arm::strncmp;
#  endif

} // namespace simd

using simd::memchr;
using simd::memcmp;
using simd::strchr;
using simd::strcmp;
using simd::strlen;
using simd::strncmp;
#else  // if defined(UTL_SUPPORTS_SIMD_INTRINSICS) && !defined(UTL_DISABLE_SIMD_LIBC)
using standard::memchr;
using standard::memcmp;
using standard::strchr;
using standard::strcmp;
using standard::strlen;
using standard::strncmp;
#endif // if defined(UTL_SUPPORTS_SIMD_INTRINSICS) && !defined(UTL_DISABLE_SIMD_LIBC)

using standard::memcpy;
using standard::memmove;
} // namespace runtime
} // namespace libc

UTL_NAMESPACE_END
