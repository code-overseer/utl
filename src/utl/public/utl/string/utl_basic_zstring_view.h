// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/string/utl_string_fwd.h"

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
#include "utl/string/utl_string_details.h"

#define UTL_STRING_PURE UTL_ATTRIBUTES(NODISCARD, PURE)
#define UTL_STRING_CONST UTL_ATTRIBUTES(NODISCARD, CONST)

UTL_NAMESPACE_BEGIN
template <typename CharType, typename Traits>
class basic_zstring_view : private basic_string_view<CharType, Traits> {
    using base_type = basic_string_view<CharType, Traits>;

public:
    using base_type::npos;
    using typename base_type::const_iterator;
    using typename base_type::const_pointer;
    using typename base_type::const_reference;
    using typename base_type::const_reverse_iterator;
    using typename base_type::difference_type;
    using typename base_type::iterator;
    using typename base_type::pointer;
    using typename base_type::reference;
    using typename base_type::reverse_iterator;
    using typename base_type::size_type;
    using typename base_type::traits_type;
    using typename base_type::value_type;

    basic_zstring_view(decltype(nullptr)) = delete;
    constexpr basic_zstring_view() noexcept : data_(), size_() {}
    constexpr basic_zstring_view(basic_zstring_view const&) noexcept = default;
    UTL_CONSTEXPR_CXX14 basic_zstring_view(const_pointer data, size_type size) UTL_THROWS
        : base_type(data, size) {
        UTL_THROW_IF(data_[size] != 0,
            invalid_argument(UTL_MESSAGE_FORMAT(
                "zstring_view construction failed, Reason=[argument string not null-terminated]")));
    }
    constexpr basic_zstring_view(const_pointer data)
        UTL_NOEXCEPT(noexcept(traits_type::length(data)))
        : base_type(data, traits_type::length(data)) {}

    template <UTL_CONCEPT_CXX20(contiguous_iterator) It,
        UTL_CONCEPT_CXX20(sized_sentinel_for<It>) E UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_contiguous_iterator(It) && UTL_TRAIT_is_sized_sentinel_for(E, It))>
    UTL_CONSTEXPR_CXX14 basic_zstring_view(It begin, E end)
        UTL_NOEXCEPT(UTL_TRAIT_is_nothrow_dereferenceable(It)&& noexcept(end - begin))
        : basic_zstring_view(UTL_SCOPE to_address(begin), end - begin) {}

    explicit UTL_CONSTEXPR_CXX14 basic_zstring_view(base_type other) UTL_THROWS
        : basic_zstring_view(other.data(), other.size()) {}

    constexpr operator base_type() const { return *this; }

    // TODO: ranges ctor

    UTL_CONSTEXPR_CXX14 basic_zstring_view& operator=(basic_zstring_view const&) noexcept = default;

    using base_type::back;
    using base_type::begin;
    using base_type::cbegin;
    using base_type::cend;
    using base_type::crbegin;
    using base_type::crend;
    using base_type::data;
    using base_type::empty;
    using base_type::end;
    using base_type::front;
    using base_type::length;
    using base_type::max_size;
    using base_type::rbegin;
    using base_type::rend;
    using base_type::size;
    using base_type::operator[];

    UTL_STRING_PURE constexpr const_reference at(size_type idx) const UTL_THROWS {
        UTL_THROW_IF(idx > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_zstring_view::at` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                idx, size()));
        return *this[idx];
    }

    UTL_CONSTEXPR_CXX14 void remove_prefix(size_type n) noexcept {
        n = UTL_SCOPE numeric_min(n, size());
        data_ += n;
        size_ -= n;
    }

    UTL_CONSTEXPR_CXX14 void swap(basic_zstring_view& other) noexcept { base_type::swap(other); }

    UTL_CONSTEXPR_CXX14 size_type copy(pointer dest, size_type count, size_type pos = 0) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_zstring_view::copy` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                pos, size()));
        auto const copied = UTL_SCOPE numeric_min(count, size() - pos);
        traits_type::copy(dest, data() + pos, copied);
        return copied;
    }

    constexpr base_type substr(size_type pos = 0, size_type count = npos) const UTL_THROWS {
        return pos >= size() ? substr_throw(UTL_SOURCE_LOCATION(), pos, size())
                             : base_type(data() + pos, UTL_SCOPE numeric_min(count, size() - pos));
    }

    using base_type::compare;
    using base_type::contains;
    using base_type::ends_with;
    using base_type::find;
    using base_type::find_first_not_of;
    using base_type::find_first_of;
    using base_type::find_last_not_of;
    using base_type::find_last_of;
    using base_type::rfind;
    using base_type::starts_with;

private:
    [[noreturn]] static basic_zstring_view substr_throw(
        UTL_SCOPE source_location src, size_t pos, size_t size) UTL_THROWS {
        exceptions::message_format format = {"[UTL] `basic_zstring_view::substr` operation failed, "
                                             "Reason=[index out of range], pos=[%zu], size=[%zu]",
            src};
        UTL_THROW(out_of_range(format, pos, size));
    }

    pointer data_;
    size_type size_;
};

template <typename CharType, typename Traits>
UTL_STRING_CONST constexpr bool operator==(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return lhs.size() == rhs.size() && (lhs.data() == rhs.data() || lhs.compare(rhs) == 0);
}

UTL_NAMESPACE_END

#if UTL_CXX20

#  include "utl/compare/utl_strong_ordering.h"

UTL_NAMESPACE_BEGIN

template <typename CharType, typename Traits>
UTL_STRING_CONST constexpr typename Traits::comparison_category operator<=>(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    using result_type = typename Traits::comparison_category;
    return static_cast<result_type>(lhs.compare(rhs) <=> 0);
}

UTL_NAMESPACE_END

#else

UTL_NAMESPACE_BEGIN

template <typename CharType, typename Traits>
UTL_STRING_CONST constexpr bool operator<(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return lhs.compare(rhs) < 0;
}

template <typename CharType, typename Traits>
UTL_STRING_CONST constexpr bool operator>(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return rhs < lhs;
}

template <typename CharType, typename Traits>
UTL_STRING_CONST constexpr bool operator>=(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return !(lhs < rhs);
}

template <typename CharType, typename Traits>
UTL_STRING_CONST constexpr bool operator<=(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return !(rhs < lhs);
}

template <typename CharType, typename Traits>
UTL_STRING_CONST constexpr bool operator!=(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return !(lhs == rhs);
}

UTL_NAMESPACE_END

#endif

#undef UTL_STRING_PURE
#undef UTL_STRING_CONST