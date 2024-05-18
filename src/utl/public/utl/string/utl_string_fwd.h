// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/utility/utl_compressed_pair.h"

#include <limits.h>

UTL_NAMESPACE_BEGIN
using size_t = decltype(sizeof(0));

template <typename>
struct char_traits;
template <typename>
class allocator;
template <typename, typename>
struct allocation_result;

template <typename CharType, typename Traits = char_traits<CharType>>
class basic_string_view;

template <typename CharType, size_t ShortSize, typename Traits = char_traits<CharType>,
    typename Alloc = allocator<CharType>>
class basic_short_string;

namespace details {
namespace string {

template <typename CharType>
struct standard_size {
    static constexpr size_t value = (48 - sizeof(size_t)) / sizeof(CharType) - 1;
};

template <>
struct standard_size<char> {
    static constexpr size_t value = 32 - sizeof(size_t) - sizeof(char);
};

#ifdef UTL_SUPPORTS_CHAR8_T
template <>
struct standard_size<char8_t> : standard_size<char> {};
#endif

} // namespace string
} // namespace details

template <typename CharType, typename Traits = char_traits<CharType>,
    typename Alloc = allocator<CharType>>
using basic_string =
    basic_short_string<CharType, details::string::standard_size<CharType>::value, Traits, Alloc>;

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;
#ifdef UTL_SUPPORTS_CHAR8_T
using u8string = basic_string<char8_t>;
#endif

template <size_t N>
using short_string = basic_short_string<char, N>;
template <size_t N>
using short_wstring = basic_short_string<wchar_t, N>;
template <size_t N>
using short_u16string = basic_short_string<char16_t, N>;
template <size_t N>
using short_u32string = basic_short_string<char32_t, N>;
#ifdef UTL_SUPPORTS_CHAR8_T
template <size_t N>
using short_u8string = basic_short_string<char8_t, N>;
#endif

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;
#ifdef UTL_SUPPORTS_CHAR8_T
using u8string_view = basic_string_view<char8_t>;
#endif

UTL_NAMESPACE_END