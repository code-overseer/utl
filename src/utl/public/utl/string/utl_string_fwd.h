// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/memory/utl_allocator_fwd.h"

#include <limits.h>

UTL_NAMESPACE_BEGIN
using size_t = decltype(sizeof(0));

template <typename>
struct __UTL_PUBLIC_TEMPLATE char_traits;

template <typename CharType, typename Traits = char_traits<CharType>>
class __UTL_PUBLIC_TEMPLATE basic_string_view;
template <typename CharType, typename Traits = char_traits<CharType>>
class __UTL_PUBLIC_TEMPLATE basic_zstring_view;

template <typename CharType, size_t ShortSize, typename Traits = char_traits<CharType>,
    typename Alloc = allocator<CharType>>
class __UTL_PUBLIC_TEMPLATE basic_short_string;

namespace details {
namespace string {

template <typename Alloc>
struct default_size_traits {
private:
    using alloc_traits UTL_NODEBUG = allocator_traits<Alloc>;
    struct heap_type_emulator {
        typename alloc_traits::pointer p;
        typename alloc_traits::size_type s;
    };

public:
    static constexpr size_t size_type = sizeof(typename alloc_traits::size_type);
    static constexpr size_t heap_type = sizeof(heap_type_emulator);
};

template <typename CharType, typename Alloc>
struct default_inline_size {
private:
    using value_type UTL_NODEBUG = CharType;
    static constexpr size_t bytes = default_size_traits<Alloc>::heap_type;

public:
    static constexpr size_t value = bytes / sizeof(value_type) - 1 > 2
        ? bytes / sizeof(value_type) - 1
        : 2;
};

template <>
struct default_inline_size<char, __UTL allocator<char>> {
private:
    static constexpr size_t bytes = 24;

public:
    static constexpr size_t value = bytes - 1;
};

template <typename CharType>
struct default_inline_size<CharType, __UTL allocator<CharType>> {
private:
    static constexpr size_t bytes = 32;

public:
    static constexpr size_t value = bytes / sizeof(CharType) - 1;
};

#if UTL_SUPPORTS_CHAR8_T
template <>
struct default_inline_size<char8_t, __UTL allocator<char8_t>> :
    default_inline_size<char, __UTL allocator<char>> {};
#endif

template <typename CharType, typename Traits, typename Alloc>
using default_type =
    basic_short_string<CharType, default_inline_size<CharType, Alloc>::value, Traits, Alloc>;
} // namespace string
} // namespace details

template <typename CharType, typename Traits = char_traits<CharType>,
    typename Alloc = allocator<CharType>>
using basic_string = details::string::default_type<CharType, Traits, Alloc>;

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;
#if UTL_SUPPORTS_CHAR8_T
using u8string = basic_string<char8_t>;
#else
using u8string = basic_string<char>;
#endif

template <size_t N>
using short_string = basic_short_string<char, N>;
template <size_t N>
using short_wstring = basic_short_string<wchar_t, N>;
template <size_t N>
using short_u16string = basic_short_string<char16_t, N>;
template <size_t N>
using short_u32string = basic_short_string<char32_t, N>;
#if UTL_SUPPORTS_CHAR8_T
template <size_t N>
using short_u8string = basic_short_string<char8_t, N>;
#else
template <size_t N>
using short_u8string = basic_short_string<char, N>;
#endif

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;
#if UTL_SUPPORTS_CHAR8_T
using u8string_view = basic_string_view<char8_t>;
#else
using u8string_view = basic_string_view<char>;
#endif

using zstring_view = basic_zstring_view<char>;
using zwstring_view = basic_zstring_view<wchar_t>;
using zu16string_view = basic_zstring_view<char16_t>;
using zu32string_view = basic_zstring_view<char32_t>;
#if UTL_SUPPORTS_CHAR8_T
using zu8string_view = basic_zstring_view<char8_t>;
#else
using zu8string_view = basic_zstring_view<char>;
#endif

UTL_NAMESPACE_END