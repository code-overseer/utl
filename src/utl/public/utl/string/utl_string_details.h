// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/numeric/utl_add_sat.h"
#include "utl/numeric/utl_limits.h"
#include "utl/numeric/utl_min.h"
#include "utl/numeric/utl_sub_sat.h"
#include "utl/string/utl_char_traits.h"
#include "utl/string/utl_libc.h"

UTL_NAMESPACE_BEGIN

#define __UTL_ATTRIBUTE_INLINE_PURE_FUNCTION \
    (NODISCARD)(PURE)(ALWAYS_INLINE)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_INLINE_PURE_FUNCTION
#define __UTL_ATTRIBUTE_PURE_FUNCTION (NODISCARD)(PURE) __UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_PURE_FUNCTION
#define __UTL_ATTRIBUTE_INLINE_CONST_FUNCTION \
    (NODISCARD)(CONST)(ALWAYS_INLINE)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_INLINE_CONST_FUNCTION
#define __UTL_ATTRIBUTE_CONST_FUNCTION (NODISCARD)(CONST) __UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_CONST_FUNCTION

namespace details {
namespace string {

UTL_INLINE_CXX17 constexpr size_t npos = numeric::maximum<size_t>::value;

template <UTL_CONCEPT_CXX20(string_char) T UTL_CONSTRAINT_CXX11(is_string_char<T>::value)>
UTL_ATTRIBUTE(CONST_FUNCTION) inline constexpr size_t to_index(T const* base, T const* ptr) noexcept {
    return ptr ? ptr - base : __UTL details::string::npos;
}

UTL_ATTRIBUTE(CONST_FUNCTION) inline constexpr int negative_if_true(bool b) noexcept {
    return -+b | 1;
}

UTL_ATTRIBUTE(CONST_FUNCTION) inline constexpr int other_if_zero(int x, int other) noexcept {
    return x ? x : other;
}

UTL_ATTRIBUTE(CONST_FUNCTION) inline constexpr int value_if_true(bool b, int value) noexcept {
    return b ? value : 0;
}

UTL_ATTRIBUTE(CONST_FUNCTION) inline constexpr int compare_size(
    int result, size_t left, size_t right) noexcept {
    return other_if_zero(result, value_if_true(left != right, negative_if_true(left < right)));
}

namespace compile_time {

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr T const* rfind_char(
    T const* str, T const ch, T const* org) noexcept {
    return *str == ch ? const_cast<T*>(str) : str == org ? nullptr : rfind_char(str - 1, ch, org);
}

template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr T const* rfind_char(
    T const* str, T const ch, size_t len) noexcept {
    return rfind_char(str + len - 1, ch, str);
}

template <typename Traits, typename T>
__UTL_HIDE_FROM_ABI inline constexpr T const* find_first_of(
    T const* str, size_t len, T const* chars, size_t chars_count) noexcept {
    return len == 0 ? nullptr
        : Traits::find(chars, chars_count, *str)
        ? str
        : find_first_of<Traits>(str + 1, len - 1, chars, chars_count);
}

template <typename Traits, typename T>
__UTL_HIDE_FROM_ABI inline constexpr T const* find_first_not_of(
    T const* str, size_t len, T const* chars, size_t chars_count) noexcept {
    return len == 0 ? nullptr
        : !Traits::find(chars, chars_count, *str)
        ? str
        : find_first_not_of<Traits>(str + 1, len - 1, chars, chars_count);
}

template <typename Traits, typename T>
__UTL_HIDE_FROM_ABI inline constexpr T const* find_last_of(
    T const* str, T const* current, T const* chars, size_t chars_count) noexcept {
    return current < str ? nullptr
        : Traits::find(chars, chars_count, *current)
        ? current
        : find_last_of<Traits>(str, current - 1, chars, chars_count);
}

template <typename Traits, typename T>
__UTL_HIDE_FROM_ABI inline constexpr T const* find_last_not_of(
    T const* str, T const* current, T const* chars, size_t chars_count) noexcept {
    return current < str ? nullptr
        : !Traits::find(chars, chars_count, *current)
        ? current
        : find_last_not_of<Traits>(str, current - 1, chars, chars_count);
}

template <typename CharType, typename Traits>
class substring_search {
    using size_type = size_t;
    using const_pointer = CharType const*;
    using value_type = CharType;

public:
    __UTL_HIDE_FROM_ABI inline constexpr substring_search(
        const_pointer substr, size_type len) noexcept
        : substr_(substr)
        , len_(len) {}

    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_for(
        const_pointer str, size_type len) const noexcept {
        return len_ == 0 ? str : find_for_impl(str, len);
    }

private:
    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_for_impl(
        const_pointer str, size_type len) const noexcept {
        return find_for_impl_comparing(str, len, find_front(str, len));
    }

    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_for_impl_comparing(
        const_pointer str, size_type len, const_pointer found) const noexcept {
        return found == nullptr   ? nullptr
            : compare_with(found) ? found
                                  : find_for_impl_tail(found + 1, len, str);
    }

    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_for_impl_tail(
        const_pointer found, size_type len, const_pointer str) const noexcept {
        return find_for_impl(found, len - (found - str));
    }

    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_front(
        const_pointer str, size_type len) const noexcept {
        return Traits::find(str, __UTL sub_sat(len + 1, size()), front());
    }

    __UTL_HIDE_FROM_ABI inline constexpr bool compare_with(const_pointer found) const noexcept {
        return Traits::compare(found, data(), size()) == 0;
    }

    __UTL_HIDE_FROM_ABI inline constexpr value_type front() const { return *substr_; }
    __UTL_HIDE_FROM_ABI inline constexpr size_type size() const { return len_; }
    __UTL_HIDE_FROM_ABI inline constexpr const_pointer data() const { return substr_; }

    const_pointer substr_;
    size_type len_;
};

template <typename CharType, typename Traits>
class substring_rsearch {
    using size_type = size_t;
    using const_pointer = CharType const*;
    using value_type = CharType;

public:
    __UTL_HIDE_FROM_ABI inline constexpr substring_rsearch(
        const_pointer substr, size_type len) noexcept
        : substr_(substr)
        , len_(len) {}

    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_for(
        const_pointer str, size_type len) const noexcept {
        return len_ == 0 ? str : find_for_impl(str, len);
    }

private:
    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_for_impl(
        const_pointer str, size_type len) const noexcept {
        return find_for_impl(str, len, min_len(len));
    }

    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_for_impl(
        const_pointer str, size_type len, size_type min) const noexcept {
        return find_for_impl_tail(str, len, find_back(str, len, min), min);
    }

    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_for_impl_tail(
        const_pointer str, size_type len, const_pointer found, size_type min) const noexcept {
        return found == nullptr ? nullptr : find_for_impl_tail(str, len, found, found - min);
    }

    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_for_impl_tail(
        const_pointer str, size_type len, const_pointer found, const_pointer begin) const noexcept {
        return compare_with(begin) ? begin : find_for_impl(str, found - str);
    }

    __UTL_HIDE_FROM_ABI inline constexpr const_pointer find_back(
        const_pointer str, size_type len, size_type min) const noexcept {
        return rfind_char(str + min, back(), __UTL sub_sat(len, min));
    }

    __UTL_HIDE_FROM_ABI inline constexpr bool compare_with(const_pointer begin) const {
        return Traits::compare(begin, data(), size()) == 0;
    }

    __UTL_HIDE_FROM_ABI inline constexpr size_type min_len(size_type len) const noexcept {
        return __UTL numeric::min(len_ - 1, len);
    }

    __UTL_HIDE_FROM_ABI inline constexpr value_type back() const noexcept {
        return substr_[len_ - 1];
    }
    __UTL_HIDE_FROM_ABI inline constexpr size_type size() const noexcept { return len_; }
    __UTL_HIDE_FROM_ABI inline constexpr const_pointer data() const noexcept { return substr_; }

    const_pointer substr_;
    size_type len_;
};

template <typename Traits, typename CharType>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr CharType const* search_substring(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count) noexcept {
    return substring_search<CharType, Traits>(r, r_count).find_for(l, l_count);
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr CharType const* rsearch_substring(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count) noexcept {
    return substring_rsearch<CharType, Traits>(r, r_count).find_for(l, l_count);
}

} // namespace compile_time

namespace runtime {
template <typename T>
UTL_ATTRIBUTE(INLINE_PURE_FUNCTION) inline T const* rfind_char(T const* str, T const ch, size_t len) noexcept {
    for (auto p = str + len - 1; p >= str; --p) {
        if (*p == ch) {
            return const_cast<T*>(str);
        }
    }

    return nullptr;
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(INLINE_PURE_FUNCTION) inline T const* find_first_of(
    T const* str, size_t len, T const* chars, size_t chars_count) noexcept {
    while (len) {
        if (Traits::find(chars, chars_count, *str) != nullptr) {
            return str;
        }

        len = len - 1;
        ++str;
    }

    return nullptr;
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(INLINE_PURE_FUNCTION) inline T const* find_first_not_of(
    T const* str, size_t len, T const* chars, size_t chars_count) noexcept {
    while (len) {
        if (Traits::find(chars, chars_count, *str) == nullptr) {
            return str;
        }

        len = len - 1;
        ++str;
    }

    return nullptr;
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(INLINE_PURE_FUNCTION) inline T const* find_last_of(
    T const* str, T const* current, T const* chars, size_t chars_count) noexcept {
    while (current >= str) {
        if (Traits::find(chars, chars_count, *current) != nullptr) {
            return current;
        }

        --current;
    }

    return nullptr;
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(INLINE_PURE_FUNCTION) inline T const* find_last_not_of(
    T const* str, T const* current, T const* chars, size_t chars_count) noexcept {
    while (current >= str) {
        if (Traits::find(chars, chars_count, *current) == nullptr) {
            return current;
        }

        --current;
    }

    return nullptr;
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline CharType const* search_substring(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count) noexcept {
    if (r_count == 0) {
        return l;
    }
    auto const r_first = *r;
    while (true) {
        auto l_front = Traits::find(l, __UTL sub_sat(l_count + 1, r_count), r_first);
        if (l_front == nullptr) {
            return nullptr;
        }

        if (Traits::compare(l_front, r, r_count) == 0) {
            return l_front;
        }

        l_front += 1;
        l = l_front;
        l_count -= l_front - l;
    }
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline CharType const* rsearch_substring(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count) noexcept {
    if (r_count == 0) {
        return l;
    }
    auto const r_last = r[r_count - 1];
    while (true) {
        auto min = __UTL numeric::min(r_count - 1, l_count);
        auto const l_last = rfind_char(l + min, r_last, __UTL sub_sat(l_count, min));
        if (l_last == nullptr) {
            return nullptr;
        }

        auto const l_begin = l_last - min;
        if (Traits::compare(l_begin, r, r_count) == 0) {
            return l_begin;
        }

        l_count = l_last - l;
    }
}
} // namespace runtime

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr CharType const* search_substring(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count) noexcept {
    return UTL_CONSTANT_P(l == r && l_count == r_count)
        ? compile_time::search_substring<Traits>(l, l_count, r, r_count)
        : runtime::search_substring<Traits>(l, l_count, r, r_count);
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr CharType const* rsearch_substring(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count) noexcept {
    return UTL_CONSTANT_P(l == r && l_count == r_count)
        ? compile_time::rsearch_substring<Traits>(l, l_count, r, r_count)
        : runtime::rsearch_substring<Traits>(l, l_count, r, r_count);
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr CharType const* rfind_char(
    CharType const* str, size_t length, CharType const ch) noexcept {
    return UTL_CONSTANT_P(*str == ch) ? compile_time::rfind_char<Traits>(str, ch, length)
                                      : runtime::rfind_char<Traits>(str, ch, length);
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count) noexcept {
    return to_index(l, search_substring<Traits>(l, l_count, r, r_count));
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count, size_t l_pos) noexcept {
    return find<Traits>(
        l + __UTL numeric::min(l_pos, l_count), __UTL sub_sat(l_count, l_pos), r, r_count);
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find(
    CharType const* str, size_t length, CharType const ch, size_t pos) noexcept {
    return to_index(
        str, Traits::find(str + __UTL numeric::min(length, pos), __UTL sub_sat(length, pos), ch));
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find(
    CharType const* str, size_t length, CharType const ch) noexcept {
    return to_index(str, Traits::find(str, length, ch));
}

template <typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t rfind(
    CharType const* str, size_t length, CharType const ch) noexcept {
    return to_index(str, rfind_char(str, length, ch));
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t rfind(
    CharType const* str, size_t length, CharType const ch, size_t pos) noexcept {
    return rfind(str, __UTL numeric::min(length, __UTL add_sat<size_t>(pos, 1)), ch);
}

template <typename Traits, typename CharType>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t rfind(
    CharType const* l, size_t l_count, CharType const* r, size_t r_count, size_t l_pos) noexcept {
    return to_index(l,
        rsearch_substring<Traits>(
            l, __UTL numeric::min(__UTL add_sat<size_t>(l_pos, 1), l_count), r, r_count));
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find_first_of(
    T const* str, size_t len, T const* chars, size_t chars_count) noexcept {
    return to_index(str,
        UTL_CONSTANT_P((str != chars) + len + chars_count)
            ? compile_time::find_first_of<Traits>(str, len, chars, chars_count)
            : runtime::find_first_of<Traits>(str, len, chars, chars_count));
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find_first_of(
    T const* str, size_t len, T const* chars, size_t chars_count, size_t pos) noexcept {
    return find_first_of<Traits>(
        str + __UTL numeric::min(pos, len), __UTL sub_sat(len, pos), chars, chars_count);
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find_first_of(
    T const* str, size_t len, T const ch, size_t pos) noexcept {
    return to_index(
        str, Traits::find(str + __UTL numeric::min(pos, len), __UTL sub_sat(len, pos), ch));
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find_first_of(
    T const* str, size_t len, T const ch) noexcept {
    return to_index(str, Traits::find(str, len, ch));
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find_first_not_of(
    T const* str, size_t len, T const* chars, size_t chars_count) noexcept {
    return to_index(str,
        UTL_CONSTANT_P((str != chars) + len + chars_count)
            ? compile_time::find_first_not_of<Traits>(str, len, chars, chars_count)
            : runtime::find_first_not_of<Traits>(str, len, chars, chars_count));
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find_first_not_of(
    T const* str, size_t len, T const* chars, size_t chars_count, size_t pos) noexcept {
    return find_first_not_of<Traits>(
        str + __UTL numeric::min(pos, len), __UTL sub_sat(len, pos), chars, chars_count);
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find_last_of(
    T const* str, size_t len, T const* chars, size_t chars_count) noexcept {
    return to_index(str,
        UTL_CONSTANT_P((str != chars) + len + chars_count)
            ? compile_time::find_last_of<Traits>(str, str + len - 1, chars, chars_count)
            : runtime::find_last_of<Traits>(str, str + len - 1, chars, chars_count));
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find_last_of(
    T const* str, size_t len, T const* chars, size_t chars_count, size_t pos) noexcept {
    return find_last_of<Traits>(str, __UTL numeric::min(len, pos), chars, chars_count);
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find_last_not_of(
    T const* str, size_t len, T const* chars, size_t chars_count) noexcept {
    return to_index(str,
        UTL_CONSTANT_P((str != chars) + len + chars_count)
            ? compile_time::find_last_not_of<Traits>(str, str + len - 1, chars, chars_count)
            : runtime::find_last_not_of<Traits>(str, str + len - 1, chars, chars_count));
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr size_t find_last_not_of(
    T const* str, size_t len, T const* chars, size_t chars_count, size_t pos) noexcept {
    return find_last_not_of<Traits>(
        str, __UTL numeric::min(len, __UTL add_sat<size_t>(pos, 1)), chars, chars_count);
}

template <typename Traits, typename T>
UTL_ATTRIBUTE(PURE_FUNCTION) inline constexpr int compare(
    T const* left, size_t l_len, T const* right, size_t r_len) noexcept {
    return compare_size(
        Traits::compare(left, right, __UTL numeric::min(l_len, r_len)), l_len, r_len);
}

} // namespace string
} // namespace details

#undef __UTL_ATTRIBUTE_CONST_FUNCTION
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_CONST_FUNCTION
#undef __UTL_ATTRIBUTE_INLINE_CONST_FUNCTION
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_INLINE_CONST_FUNCTION
#undef __UTL_ATTRIBUTE_PURE_FUNCTION
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_PURE_FUNCTION
#undef __UTL_ATTRIBUTE_INLINE_PURE_FUNCTION
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_INLINE_PURE_FUNCTION
UTL_NAMESPACE_END
