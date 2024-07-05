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
#include "utl/memory/utl_pointer_traits.h"

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
    static constexpr size_type npos = (size_type)-1;
    class iterator;
    class const_iterator;

    class iterator : UTL_SCOPE contiguous_iterator_base<iterator, value_type> {
        using base_type = contiguous_iterator_base<iterator, value_type>;

    public:
        using typename base_type::difference_type;
        using typename base_type::iterator_concept;
        using typename base_type::pointer;
        using typename base_type::reference;
        using typename base_type::value_type;

        constexpr iterator() noexcept = default;
        constexpr iterator(iterator const& other) noexcept = default;
        constexpr iterator(iterator&& other) noexcept = default;
        constexpr iterator& operator=(iterator const& other) noexcept = default;
        constexpr iterator& operator=(iterator&& other) noexcept = default;
        using base_type::operator*;
        using base_type::operator->;

    private:
        friend basic_string_view;
        constexpr iterator(pointer data) noexcept : base_type(data) {}
        constexpr iterator(
            contiguous_iterator_base<const_iterator, value_type const> other) noexcept
            : iterator(const_cast<pointer>(other.operator->())) {}
    };

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

    using reverse_iterator = UTL_SCOPE reverse_iterator<iterator>;
    using const_reverse_iterator = UTL_SCOPE reverse_iterator<const_iterator>;

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
        UTL_CONCEPT_CXX20(sized_sentinel_for<It>) E>
    constexpr basic_string_view(It begin, E end) noexcept(
        UTL_TRAIT_is_nothrow_dereferenceable(It) && noexcept(end - begin))
        : data_(UTL_SCOPE to_address(begin))
        , size_(end - begin) {}

    // TODO: ranges ctor

private:
    pointer data_;
    size_type size_;
};

UTL_NAMESPACE_END

#undef UTL_STRING_PURE
#undef UTL_STRING_CONST