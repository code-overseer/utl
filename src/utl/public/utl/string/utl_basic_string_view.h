// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/string/utl_string_fwd.h"

#include "utl/assert/utl_assert.h"
#include "utl/exception.h"
#include "utl/iterator/utl_contiguous_iterator.h"
#include "utl/iterator/utl_contiguous_iterator_base.h"
#include "utl/iterator/utl_distance.h"
#include "utl/iterator/utl_legacy_forward_iterator.h"
#include "utl/iterator/utl_legacy_input_iterator.h"
#include "utl/iterator/utl_reverse_iterator.h"
#include "utl/iterator/utl_sized_sentinel_for.h"
#include "utl/memory/utl_to_address.h"
#include "utl/numeric/utl_max.h"
#include "utl/numeric/utl_min.h"
#include "utl/string/utl_string_details.h"

#define __UTL_ATTRIBUTE_STRING_INLINE_PURE \
    (ALWAYS_INLINE)(PURE)(NODISCARD)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_STRING_INLINE_PURE
#define __UTL_ATTRIBUTE_STRING_PURE (PURE)(NODISCARD) __UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_STRING_PURE

UTL_NAMESPACE_BEGIN
template <typename CharType, typename Traits>
class __UTL_PUBLIC_TEMPLATE basic_string_view {
public:
    using value_type = CharType;
    using size_type = size_t;
    using difference_type = decltype((CharType*)0 - (CharType*)0);
    using traits_type = Traits;
    using pointer = CharType*;
    using const_pointer = CharType const*;
    using reference = CharType&;
    using const_reference = CharType const&;
    static constexpr size_type npos = details::string::npos;
    class const_iterator;
    using iterator = const_iterator;
    using const_reverse_iterator = UTL_SCOPE reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;

    class __UTL_ABI_PUBLIC const_iterator :
        UTL_SCOPE contiguous_iterator_base<const_iterator, value_type const> {
        using base_type = contiguous_iterator_base<const_iterator, value_type const>;

    public:
        using typename base_type::difference_type;
        using typename base_type::iterator_concept;
        using typename base_type::pointer;
        using typename base_type::reference;
        using typename base_type::value_type;

        __UTL_HIDE_FROM_ABI constexpr const_iterator() noexcept = default;
        __UTL_HIDE_FROM_ABI constexpr const_iterator(
            const_iterator const& other) noexcept = default;
        __UTL_HIDE_FROM_ABI constexpr const_iterator(const_iterator&& other) noexcept = default;
        __UTL_HIDE_FROM_ABI constexpr const_iterator& operator=(
            const_iterator const& other) noexcept = default;
        __UTL_HIDE_FROM_ABI constexpr const_iterator& operator=(
            const_iterator&& other) noexcept = default;
        using base_type::operator*;
        using base_type::operator->;

    private:
        friend basic_string_view;
        __UTL_HIDE_FROM_ABI constexpr const_iterator(pointer data) noexcept : base_type(data) {}
    };

    basic_string_view(decltype(nullptr)) = delete;
    __UTL_HIDE_FROM_ABI constexpr basic_string_view() noexcept : data_(), size_() {}
    __UTL_HIDE_FROM_ABI constexpr basic_string_view(basic_string_view const&) noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr basic_string_view(const_pointer data, size_type size) noexcept
        : data_(data)
        , size_(size) {}
    __UTL_HIDE_FROM_ABI constexpr basic_string_view(const_pointer data) noexcept(
        noexcept(traits_type::length(data)))
        : data_(data)
        , size_(traits_type::length(data)) {}

    template <UTL_CONCEPT_CXX20(contiguous_iterator) It,
        UTL_CONCEPT_CXX20(sized_sentinel_for<It>) E UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_contiguous_iterator(It) && UTL_TRAIT_is_sized_sentinel_for(E, It))>
    __UTL_HIDE_FROM_ABI constexpr basic_string_view(It begin, E end) noexcept(
        UTL_TRAIT_is_nothrow_dereferenceable(It) && noexcept(end - begin))
        : data_(UTL_SCOPE to_address(begin))
        , size_(end - begin) {}

    // TODO: ranges ctor

    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 basic_string_view& operator=(
        basic_string_view const&) noexcept = default;

    UTL_ATTRIBUTE(STRING_INLINE_PURE) constexpr const_pointer data() const noexcept UTL_LIFETIMEBOUND {
        return data_;
    }
    UTL_ATTRIBUTE(STRING_INLINE_PURE) constexpr bool empty() const noexcept { return !size(); }
    UTL_ATTRIBUTE(STRING_INLINE_PURE) constexpr size_type size() const noexcept { return size_; }
    UTL_ATTRIBUTE(STRING_INLINE_PURE) constexpr size_type length() const noexcept { return size_; }

    UTL_ATTRIBUTES(CONST, NODISCARD, _HIDE_FROM_ABI) constexpr size_type max_size() const noexcept {
        return npos;
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr const_iterator cbegin() const noexcept UTL_LIFETIMEBOUND {
        return const_iterator(data());
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr const_iterator begin() const noexcept UTL_LIFETIMEBOUND {
        return const_iterator(data());
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr const_iterator cend() const noexcept UTL_LIFETIMEBOUND {
        return const_iterator(data() + size());
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr const_iterator end() const noexcept UTL_LIFETIMEBOUND {
        return const_iterator(data() + size());
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr const_iterator crbegin() const noexcept UTL_LIFETIMEBOUND {
        return const_reverse_iterator(end());
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr const_iterator rbegin() const noexcept UTL_LIFETIMEBOUND {
        return const_reverse_iterator(end());
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr const_iterator crend() const noexcept UTL_LIFETIMEBOUND {
        return const_reverse_iterator(begin());
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr const_iterator rend() const noexcept UTL_LIFETIMEBOUND {
        return const_reverse_iterator(begin());
    }
    UTL_ATTRIBUTE(STRING_INLINE_PURE) constexpr const_reference front() const noexcept UTL_LIFETIMEBOUND {
        return *data_;
    }
    UTL_ATTRIBUTE(STRING_INLINE_PURE) constexpr const_reference back() const noexcept UTL_LIFETIMEBOUND {
        return data_[size_ - 1];
    }
    UTL_ATTRIBUTE(STRING_INLINE_PURE) constexpr const_reference operator[](size_type idx) const noexcept UTL_LIFETIMEBOUND {
        return data_[idx];
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr const_reference at(size_type idx) const UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_THROW_IF(idx > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_string_view::at` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                idx, size()));
        return *this[idx];
    }

    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void remove_prefix(size_type n) noexcept {
        n = UTL_SCOPE numeric::min(n, size());
        data_ += n;
        size_ -= n;
    }

    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void remove_suffix(size_type n) noexcept {
        n = UTL_SCOPE numeric::min(n, size());
        size_ -= n;
    }

    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void swap(basic_string_view& other) noexcept {
        other = UTL_SCOPE exchange(*this, other);
    }

    __UTL_HIDE_FROM_ABI friend UTL_CONSTEXPR_CXX14 void swap(
        basic_string_view& l, basic_string_view& r) noexcept {
        l.swap(r);
    }

    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 size_t copy(
        pointer dest, size_type count, size_type pos = 0) const UTL_THROWS {
        UTL_ASSERT(dest != nullptr);
        UTL_THROW_IF(pos > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_string_view::copy` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                pos, size()));
        auto const copied = UTL_SCOPE numeric::min(count, size() - pos);
        traits_type::copy(dest, data() + pos, copied);
        return copied;
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr basic_string_view substr(
        size_type pos = 0, size_type count = npos) const UTL_THROWS {
        return pos >= size()
            ? substr_throw(UTL_SOURCE_LOCATION(), pos, size())
            : basic_string_view(data() + pos, UTL_SCOPE numeric::min(count, size() - pos));
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr int compare(basic_string_view other) const noexcept {
        return details::string::compare<traits_type>(data(), size(), other.data(), other.size());
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr int compare(
        size_type pos, size_type count, basic_string_view other) const UTL_THROWS {
        return substr(pos, count).compare(other);
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr int compare(size_type pos, size_type count, basic_string_view other,
        size_type other_pos, size_type other_count) const UTL_THROWS {
        return substr(pos, count).compare(other.substr(other_pos, other_count));
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr int compare(const_pointer other) const noexcept {
        return compare(basic_string_view(other));
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr int compare(
        size_type pos, size_type count, const_pointer other) const UTL_THROWS {
        return substr(pos, count).compare(basic_string_view(other));
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr int compare(
        size_type pos, size_type count, const_pointer other, size_type other_count) const UTL_THROWS {
        return substr(pos, count).compare(basic_string_view(other, other_count));
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr bool starts_with(basic_string_view other) const noexcept {
        return size() >= other.size() &&
            traits_type::compare(data(), other.data(), other.size()) == 0;
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr bool starts_with(value_type ch) const noexcept {
        return !empty() && front() == ch;
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr bool starts_with(const_pointer other) const noexcept {
        // CONSTEXPR_ASSERT(other != nullptr);
        return starts_with(basic_string_view(other));
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr bool ends_with(basic_string_view other) const noexcept {
        return size() >= other.size() &&
            traits_type::compare(data() + size() - other.size(), other.data(), other.size()) == 0;
    }
    UTL_ATTRIBUTE(STRING_PURE) constexpr bool ends_with(value_type ch) const noexcept {
        return !empty() && back() == ch;
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr bool ends_with(const_pointer other) const noexcept {
        // CONSTEXPR_ASSERT(other != nullptr);
        return ends_with(basic_string_view(other));
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr bool contains(basic_string_view other) const noexcept {
        return find(other) != npos;
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr bool contains(value_type ch) const noexcept {
        return find(ch) != npos;
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr bool contains(const_pointer other) const noexcept {
        return find(other) != npos;
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find(
        basic_string_view other, size_type pos = 0) const noexcept {
        return find(other.data(), pos, other.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find(value_type ch, size_type pos = 0) const noexcept {
        return details::string::find<traits_type>(data(), size(), ch, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find(
        const_pointer str, size_type pos, size_type str_len) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return details::string::find<traits_type>(data(), size(), str, str_len, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find(const_pointer str, size_type pos = 0) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return find(str, pos, traits_type::length(str));
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type rfind(
        basic_string_view other, size_type pos = npos) const noexcept {
        return rfind(other.data(), pos, other.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type rfind(
        const_pointer str, size_type pos, size_type str_len) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return details::string::rfind<traits_type>(data(), size(), str, str_len, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type rfind(
        const_pointer str, size_type pos = npos) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return rfind(str, pos, traits_type::length(str));
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type rfind(value_type ch, size_type pos = npos) const noexcept {
        return details::string::rfind<traits_type>(data(), size(), ch, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_first_of(
        basic_string_view other, size_type pos = 0) const noexcept {
        return find_first_of(other.data(), pos, other.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_first_of(
        const_pointer chars, size_type pos, size_type chars_count) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return details::string::find_first_of<traits_type>(data(), size(), chars, chars_count, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_first_of(
        const_pointer chars, size_type pos = 0) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return find_first_of(chars, pos, traits_type::length(chars));
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_first_of(
        value_type ch, size_type pos = 0) const noexcept {
        return details::string::find_first_of<traits_type>(data(), size(), ch, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_first_not_of(
        basic_string_view other, size_type pos = 0) const noexcept {
        return find_first_not_of(other.data(), pos, other.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_first_not_of(
        const_pointer chars, size_type pos, size_type chars_count) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return details::string::find_first_not_of<traits_type>(
            data(), size(), chars, chars_count, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_first_not_of(
        const_pointer chars, size_type pos = 0) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return find_first_not_of(chars, pos, traits_type::length(chars));
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_first_not_of(
        value_type ch, size_type pos = 0) const noexcept {
        return find_first_not_of(&ch, pos, 1);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_last_of(
        basic_string_view other, size_type pos = npos) const noexcept {
        return find_last_of(other.data(), pos, other.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_last_of(
        const_pointer chars, size_type pos, size_type chars_count) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return details::string::find_last_of<traits_type>(data(), size(), chars, chars_count, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_last_of(
        const_pointer chars, size_type pos = npos) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return find_last_of(chars, pos, traits_type::length(chars));
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_last_of(
        value_type ch, size_type pos = npos) const noexcept {
        return find_last_of(&ch, pos, 1);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_last_not_of(
        basic_string_view other, size_type pos = npos) const noexcept {
        return find_last_not_of(other.data(), pos, other.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_last_not_of(
        const_pointer chars, size_type pos, size_type count) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return details::string::find_last_not_of<traits_type>(data(), size(), chars, count, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_last_not_of(
        const_pointer chars, size_type pos = npos) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return find_last_not_of(chars, pos, traits_type::length(chars));
    }

    UTL_ATTRIBUTE(STRING_PURE) constexpr size_type find_last_not_of(
        value_type ch, size_type pos = npos) const noexcept {
        return find_last_not_of(&ch, pos, 1);
    }

private:
    UTL_ATTRIBUTES(NORETURN, NOINLINE, _HIDE_FROM_ABI) static basic_string_view substr_throw(
        UTL_SCOPE source_location src, size_t pos, size_t size) UTL_THROWS {
        exceptions::message_format format = {"[UTL] `basic_string_view::substr` operation failed, "
                                             "Reason=[index out of range], pos=[%zu], size=[%zu]",
            src};
        UTL_THROW(out_of_range(format, pos, size));
    }

    const_pointer data_;
    size_type size_;
};

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) constexpr bool operator==(
    basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs) noexcept {
    return lhs.size() == rhs.size() && (lhs.data() == rhs.data() || lhs.compare(rhs) == 0);
}

UTL_NAMESPACE_END

#if UTL_CXX20

#  include "utl/compare/utl_strong_ordering.h"

UTL_NAMESPACE_BEGIN

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) constexpr typename Traits::comparison_category operator<=>(
    basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs) noexcept {
    using result_type = typename Traits::comparison_category;
    return static_cast<result_type>(lhs.compare(rhs) <=> 0);
}

UTL_NAMESPACE_END

#else

UTL_NAMESPACE_BEGIN

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) constexpr bool operator<(
    basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs) noexcept {
    return lhs.compare(rhs) < 0;
}

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) constexpr bool operator>(
    basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs) noexcept {
    return rhs < lhs;
}

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) constexpr bool operator>=(
    basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs) noexcept {
    return !(lhs < rhs);
}

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) constexpr bool operator<=(
    basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs) noexcept {
    return !(rhs < lhs);
}

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) constexpr bool operator!=(
    basic_string_view<CharType, Traits> lhs, basic_string_view<CharType, Traits> rhs) noexcept {
    return !(lhs == rhs);
}

UTL_NAMESPACE_END

#endif

#undef __UTL_ATTRIBUTE_STRING_PURE
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_STRING_PURE
