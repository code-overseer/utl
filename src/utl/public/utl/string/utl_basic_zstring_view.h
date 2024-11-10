// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/string/utl_string_fwd.h"

#include "utl/exception.h"
#include "utl/iterator/utl_const_iterator.h"
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
#include "utl/string/utl_basic_string_view.h"
#include "utl/string/utl_string_details.h"

#define __UTL_ATTRIBUTE_STRING_PURE (PURE)(NODISCARD) __UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_STRING_PURE

UTL_NAMESPACE_BEGIN
template <typename CharType, typename Traits>
class __UTL_PUBLIC_TEMPLATE basic_zstring_view : private basic_string_view<CharType, Traits> {
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
    __UTL_HIDE_FROM_ABI inline constexpr basic_zstring_view() noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr basic_zstring_view(
        basic_zstring_view const&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_zstring_view(
        const_pointer data, size_type size) UTL_THROWS
        : base_type(data, size) {
        UTL_THROW_IF(data()[size] != 0,
            invalid_argument(UTL_MESSAGE_FORMAT(
                "zstring_view construction failed, Reason=[argument string not null-terminated]")));
    }
    __UTL_HIDE_FROM_ABI inline constexpr basic_zstring_view(const_pointer data)
        UTL_NOEXCEPT(noexcept(traits_type::length(data)))
        : base_type(data, traits_type::length(data)) {}

    template <UTL_CONCEPT_CXX20(contiguous_iterator) It,
        UTL_CONCEPT_CXX20(sized_sentinel_for<It>) E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_contiguous_iterator(It) && UTL_TRAIT_is_sized_sentinel_for(It, E))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_zstring_view(It begin, E end)
        UTL_NOEXCEPT(UTL_TRAIT_is_nothrow_dereferenceable(It)&& noexcept(end - begin))
        : basic_zstring_view(__UTL to_address(begin), end - begin) {}

    __UTL_HIDE_FROM_ABI explicit UTL_CONSTEXPR_CXX14 basic_zstring_view(base_type other) UTL_THROWS
        : basic_zstring_view(other.data(), other.size()) {}

    __UTL_HIDE_FROM_ABI inline constexpr operator base_type() const { return *this; }

    // TODO: ranges ctor

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_zstring_view& operator=(
        basic_zstring_view const&) noexcept = default;

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
    using base_type::remove_prefix;

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr const_reference at(size_type idx) const UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_THROW_IF(idx > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_zstring_view::at` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                idx, size()));
        return *this[idx];
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(basic_zstring_view& other) noexcept {
        base_type::swap(other);
    }

    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void swap(
        basic_zstring_view& l, basic_zstring_view& r) noexcept {
        l.swap(r);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 size_type copy(
        pointer dest, size_type count, size_type pos = 0) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_zstring_view::copy` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                pos, size()));
        auto const copied = __UTL numeric::min(count, size() - pos);
        traits_type::copy(dest, data() + pos, copied);
        return copied;
    }

    __UTL_HIDE_FROM_ABI inline constexpr base_type substr(
        size_type pos = 0, size_type count = npos) const UTL_THROWS {
        return pos >= size() ? substr_throw(UTL_SOURCE_LOCATION(), pos, size())
                             : base_type(data() + pos, __UTL numeric::min(count, size() - pos));
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

    template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    __UTL_HIDE_FROM_ABI friend inline constexpr basic_zstring_view remove_prefix(
        basic_zstring_view str, T count) noexcept {
        return basic_zstring_view(
            str.substr(__UTL numeric::min<size_type>(__UTL to_unsigned(count), str.size())));
    }

    template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    __UTL_HIDE_FROM_ABI friend inline constexpr base_type remove_suffix(
        basic_zstring_view str, T count) noexcept {
        return str.substr(0, __UTL sub_sat<size_type>(str.size(), __UTL to_unsigned(count)));
    }

    template <UTL_CONCEPT_CXX20(invocable<base_type>) F>
    __UTL_HIDE_FROM_ABI friend inline constexpr UTL_ENABLE_IF_CXX11(
        size_type, UTL_TRAIT_is_invocable(F, base_type))
    split_all(basic_zstring_view str, value_type delimiter, F callable) noexcept {
        return split_all(base_type(str), delimiter, __UTL move(callable));
    }

    template <UTL_CONCEPT_CXX20(invocable<base_type>) F>
    __UTL_HIDE_FROM_ABI friend inline constexpr UTL_ENABLE_IF_CXX11(
        size_type, UTL_TRAIT_is_invocable(F, base_type))
    split_all(basic_zstring_view str, const_pointer delimiter, F callable) noexcept {
        return split_all(base_type(str), base_type(delimiter), __UTL move(callable));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<base_type>) ViewLike,
        UTL_CONCEPT_CXX20(predicate<base_type>) F>
    __UTL_HIDE_FROM_ABI friend inline constexpr UTL_ENABLE_IF_CXX11(size_type,
        UTL_TRAIT_conjunction(is_predicate<F, base_type>, is_convertible<ViewLike, base_type>))
    split_all(basic_zstring_view str, ViewLike const& delimiter, F callable) noexcept(
        UTL_TRAIT_is_nothrow_convertible(ViewLike, base_type)) {
        return split_all(base_type(str), base_type(delimiter), __UTL move(callable));
    }

    template <UTL_CONCEPT_CXX20(predicate<base_type>) F>
    __UTL_HIDE_FROM_ABI friend inline constexpr UTL_ENABLE_IF_CXX11(
        size_type, UTL_TRAIT_is_predicate(F, base_type))
    split_while(basic_zstring_view str, value_type delimiter, F callable) noexcept {
        return split_while(base_type(str), delimiter, __UTL move(callable));
    }

    template <UTL_CONCEPT_CXX20(predicate<base_type>) F>
    __UTL_HIDE_FROM_ABI friend inline constexpr UTL_ENABLE_IF_CXX11(
        size_type, UTL_TRAIT_is_predicate(F, base_type))
    split_while(basic_zstring_view str, const_pointer delimiter, F callable) noexcept {
        return split_while(base_type(str), base_type(delimiter), __UTL move(callable));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<base_type>) ViewLike,
        UTL_CONCEPT_CXX20(predicate<base_type>) F>
    __UTL_HIDE_FROM_ABI friend inline constexpr UTL_ENABLE_IF_CXX11(size_type,
        UTL_TRAIT_conjunction(is_predicate<F, base_type>, is_convertible<ViewLike, base_type>))
    split_while(basic_zstring_view str, ViewLike const& delimiter, F callable) noexcept(
        UTL_TRAIT_is_nothrow_convertible(ViewLike, base_type)) {
        return split_while(base_type(str), base_type(delimiter), __UTL move(callable));
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr base_type rstrip(basic_zstring_view str) noexcept {
        return rstrip(base_type(str));
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr basic_zstring_view lstrip(
        basic_zstring_view str) noexcept {
        return basic_zstring_view(rstrip(base_type(str)));
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr base_type strip(basic_zstring_view str) noexcept {
        return rstrip(lstrip(str));
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr base_type unwrap(
        basic_zstring_view str, value_type enclosure) noexcept {
        return unwrap(base_type(str), enclosure);
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr base_type unwrap(
        basic_zstring_view str, value_type left, value_type right) noexcept {
        return unwrap(base_type(str), left, right);
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr base_type unwrap(
        basic_zstring_view str, const_pointer enclosure) noexcept {
        return unwrap(base_type(str), enclosure);
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr base_type unwrap(
        basic_zstring_view str, const_pointer left, const_pointer right) noexcept {
        return unwrap(base_type(str), left, right);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<base_type>) ViewLike,
        UTL_CONCEPT_CXX20(predicate<base_type>) F>
    __UTL_HIDE_FROM_ABI friend inline constexpr UTL_ENABLE_IF_CXX11(base_type,
        UTL_TRAIT_conjunction(is_predicate<F, base_type>, is_convertible<ViewLike, base_type>))
    unwrap(basic_zstring_view str, ViewLike const& enclosure) noexcept(
        UTL_TRAIT_is_nothrow_convertible(ViewLike, base_type)) {
        return unwrap(base_type(str), enclosure);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<base_type>) V0,
        UTL_CONCEPT_CXX20(convertible_to<base_type>) V1,
        UTL_CONCEPT_CXX20(predicate<base_type>) F>
    __UTL_HIDE_FROM_ABI friend inline constexpr UTL_ENABLE_IF_CXX11(base_type,
        UTL_TRAIT_conjunction(is_predicate<F, base_type>, is_convertible<V0, base_type>,
            is_convertible<V1, base_type>))
    unwrap(basic_zstring_view str, V0 const& left, V1 const& right) noexcept(
        UTL_TRAIT_is_nothrow_convertible(V0, base_type) &&
        UTL_TRAIT_is_nothrow_convertible(V1, base_type)) {
        return unwrap(base_type(str), left, right);
    }

private:
    UTL_ATTRIBUTES(NORETURN, NOINLINE) static basic_zstring_view substr_throw(
        __UTL source_location src, size_t pos, size_t size) UTL_THROWS {
        exceptions::message_vformat format = {
            "[UTL] `basic_zstring_view::substr` operation failed, "
            "Reason=[index out of range], pos=[%zu], size=[%zu]",
            src};
        UTL_THROW(out_of_range(format, pos, size));
    }
};

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator==(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return lhs.size() == rhs.size() && (lhs.data() == rhs.data() || lhs.compare(rhs) == 0);
}

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator!=(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return !(lhs == rhs);
}

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator<(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return lhs.compare(rhs) < 0;
}

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator>(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return rhs < lhs;
}

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator>=(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return !(lhs < rhs);
}

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator<=(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    return !(rhs < lhs);
}

UTL_NAMESPACE_END

#if UTL_CXX20

#  include "utl/compare/utl_compare_fwd.h"

UTL_NAMESPACE_BEGIN

template <typename CharType, typename Traits>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr typename Traits::comparison_category operator<=>(
    basic_zstring_view<CharType, Traits> lhs, basic_zstring_view<CharType, Traits> rhs) noexcept {
    using result_type = typename Traits::comparison_category;
    return static_cast<result_type>(lhs.compare(rhs) <=> 0);
}

UTL_NAMESPACE_END

#endif

#undef __UTL_ATTRIBUTE_STRING_PURE
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_STRING_PURE
