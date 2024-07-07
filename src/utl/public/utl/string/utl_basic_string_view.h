// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/algorithm/utl_max.h"
#include "utl/algorithm/utl_min.h"
#include "utl/exception.h"
#include "utl/iterator/utl_contiguous_iterator.h"
#include "utl/iterator/utl_contiguous_iterator_base.h"
#include "utl/iterator/utl_distance.h"
#include "utl/iterator/utl_legacy_forward_iterator.h"
#include "utl/iterator/utl_legacy_input_iterator.h"
#include "utl/iterator/utl_reverse_iterator.h"
#include "utl/iterator/utl_sized_sentinel_for.h"
#include "utl/memory/utl_to_address.h"

#define UTL_STRING_PURE UTL_ATTRIBUTES(NODISCARD, PURE)
#define UTL_STRING_CONST UTL_ATTRIBUTES(NODISCARD, CONST)

UTL_NAMESPACE_BEGIN
template <typename CharType, typename Traits>
class basic_string_view {
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

    class const_iterator : UTL_SCOPE contiguous_iterator_base<const_iterator, value_type const> {
        using base_type = contiguous_iterator_base<const_iterator, value_type const>;

    public:
        using typename base_type::difference_type;
        using typename base_type::iterator_concept;
        using typename base_type::pointer;
        using typename base_type::reference;
        using typename base_type::value_type;

        constexpr const_iterator() noexcept = default;
        constexpr const_iterator(const_iterator const& other) noexcept = default;
        constexpr const_iterator(const_iterator&& other) noexcept = default;
        constexpr const_iterator& operator=(const_iterator const& other) noexcept = default;
        constexpr const_iterator& operator=(const_iterator&& other) noexcept = default;
        using base_type::operator*;
        using base_type::operator->;

    private:
        friend basic_string_view;
        constexpr const_iterator(pointer data) noexcept : base_type(data) {}
        constexpr const_iterator(contiguous_iterator_base<iterator, value_type> other) noexcept
            : base_type(other) {}
    };

    basic_string_view(decltype(nullptr)) = delete;
    constexpr basic_string_view() noexcept : data_(), size_() {}
    constexpr basic_string_view(basic_string_view const&) noexcept = default;
    constexpr basic_string_view(const_pointer data, size_type size) noexcept
        : data_(data)
        , size_(size) {}
    constexpr basic_string_view(const_pointer data) noexcept(noexcept(traits_type::length(data)))
        : data_(data)
        , size_(traits_type::length(data)) {}

    template <UTL_CONCEPT_CXX20(contiguous_iterator) It,
        UTL_CONCEPT_CXX20(sized_sentinel_for<It>) E UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_contiguous_iterator(It) && UTL_TRAIT_is_sized_sentinel_for(E, It))>
    constexpr basic_string_view(It begin, E end) noexcept(
        UTL_TRAIT_is_nothrow_dereferenceable(It) && noexcept(end - begin))
        : data_(UTL_SCOPE to_address(begin))
        , size_(end - begin) {}

    // TODO: ranges ctor

    UTL_CONSTEXPR_CXX14 basic_string_view& operator=(basic_string_view const&) noexcept = default;

    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE) constexpr const_pointer data() const noexcept {
        return data_;
    }
    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE) constexpr bool empty() const noexcept {
        return !size();
    }
    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE) constexpr size_type size() const noexcept {
        return size_;
    }
    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE) constexpr size_type length() const noexcept {
        return size_;
    }
    UTL_STRING_CONST constexpr size_type max_size() const noexcept { return npos; }

    UTL_STRING_PURE constexpr const_iterator cbegin() const noexcept {
        return const_iterator(data());
    }
    UTL_STRING_PURE constexpr const_iterator begin() const noexcept {
        return const_iterator(data());
    }
    UTL_STRING_PURE constexpr const_iterator cend() const noexcept {
        return const_iterator(data() + size());
    }
    UTL_STRING_PURE constexpr const_iterator end() const noexcept {
        return const_iterator(data() + size());
    }
    UTL_STRING_PURE constexpr const_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    UTL_STRING_PURE constexpr const_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    UTL_STRING_PURE constexpr const_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }
    UTL_STRING_PURE constexpr const_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
    constexpr const_reference front() const noexcept { return *data_; }
    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE) constexpr const_reference back() const noexcept {
        return data_[size_ - 1];
    }
    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE)
    constexpr const_reference operator[](size_type idx) const noexcept { return data_[idx]; }

    UTL_STRING_PURE constexpr const_reference at(size_type idx) const UTL_THROWS {
        UTL_THROW_IF(idx >= size(),
            program_exception<void>("[UTL] basic_string_view::at operation failed, "
                                    "Reason=[index out of range]"));
        return *this[idx];
    }

    UTL_CONSTEXPR_CXX14 void remove_prefix(size_type n) noexcept {
        n = UTL_SCOPE numeric_min(n, size());
        data_ += n;
        size_ -= n;
    }

    UTL_CONSTEXPR_CXX14 void remove_suffix(size_type n) noexcept {
        n = UTL_SCOPE numeric_min(n, size());
        size_ -= n;
    }

    UTL_CONSTEXPR_CXX14 void swap(basic_string_view& other) noexcept {
        other = UTL_SCOPE exchange(*this, other);
    }

    UTL_CONSTEXPR_CXX14 void copy(pointer dest, size_type count, size_type pos = 0) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] basic_string_view::copy operation failed, "
                                    "Reason=[index out of range]"));
        traits_type::copy(dest, data() + pos, UTL_SCOPE numeric_min(count, size() - pos));
    }

    UTL_STRING_PURE
    constexpr basic_string_view substr(size_type pos = 0, size_type count = npos) const UTL_THROWS {
        return pos >= size()
            ? substr_throw()
            : basic_string_view(data() + pos, UTL_SCOPE numeric_min(count, size() - pos));
    }

    constexpr int compare(basic_string_view other) const noexcept {
        return details::string::compare<traits_type>(data(), size(), other.data(), other.size());
    }
    constexpr int compare(size_type pos, size_type count, basic_string_view other) const UTL_THROWS {
        return substr(pos, count).compare(other);
    }
    constexpr int compare(size_type pos, size_type count, basic_string_view other,
        size_type other_pos, size_type other_count) const UTL_THROWS {
        return substr(pos, count).compare(other.substr(other_pos, other_count));
    }
    constexpr int compare(const_pointer other) const { return compare(basic_string_view(other)); }
    constexpr int compare(size_type pos, size_type count, const_pointer other) const UTL_THROWS {
        return substr(pos, count).compare(basic_string_view(other));
    }
    constexpr int compare(
        size_type pos, size_type count, const_pointer other, size_type other_count) const UTL_THROWS {
        return substr(pos, count).compare(basic_string_view(other, other_count));
    }
    constexpr bool starts_with(basic_string_view other) const noexcept {
        return size() >= other.size() &&
            traits_type::compare(data(), other.data(), other.size()) == 0;
    }
    constexpr bool starts_with(value_type ch) const noexcept { return !empty() && front() == ch; }
    constexpr bool starts_with(const_pointer other) const noexcept {
        return starts_with(basic_string_view(other));
    }
    constexpr bool ends_with(basic_string_view other) const noexcept {
        return size() >= other.size() &&
            traits_type::compare(data() + size() - other.size(), other.data(), other.size()) == 0;
    }
    constexpr bool ends_with(value_type ch) const noexcept { return !empty() && back() == ch; }
    constexpr bool ends_with(const_pointer other) const noexcept {
        return ends_with(basic_string_view(other));
    }
    constexpr bool contains(basic_string_view other) const noexcept { return find(other) != npos; }
    constexpr bool contains(value_type ch) const noexcept { return find(other) != npos; }
    constexpr bool contains(const_pointer other) const UTL_THROWS { return find(other) != npos; }
    constexpr size_type find(basic_string_view other, size_type pos = 0) const noexcept {
        return details::string::find<traits_type>(data(), size(), str.data(), str.size(), pos);
    }

private:
    [[noreturn]] static basic_string_view substr_throw() const UTL_THROWS {
        UTL_THROW(program_exception<void>("[UTL] basic_string_view::substr operation failed, "
                                          "Reason=[index out of range]"));
    }

    pointer data_;
    size_type size_;
};

UTL_NAMESPACE_END

#undef UTL_STRING_PURE
#undef UTL_STRING_CONST