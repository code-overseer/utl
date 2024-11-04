// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/array/utl_array_fwd.h"

#include "utl/concepts/utl_same_as.h"
#include "utl/iterator/utl_const_iterator.h"
#include "utl/iterator/utl_contiguous_iterator.h"
#include "utl/iterator/utl_contiguous_iterator_base.h"
#include "utl/iterator/utl_reverse_iterator.h"
#include "utl/iterator/utl_sized_sentinel_for.h"
#include "utl/type_traits/utl_copy_cv.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_const.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_type_identity.h"

#include <initializer_list>

UTL_NAMESPACE_BEGIN

UTL_INLINE_CXX17 constexpr size_t dynamic_extent = static_cast<size_t>(-1);

template <typename T, size_t E = dynamic_extent>
class __UTL_PUBLIC_TEMPLATE span {
public:
    using element_type = T;
    using value_type = __UTL remove_cv_t<T>;
    using size_type = decltype(sizeof(0));
    using difference_type = decltype(static_cast<T*>(0) - static_cast<T*>(0));
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;
    class iterator;
    using const_iterator = __UTL const_iterator<iterator>;
    using reverse_iterator = __UTL reverse_iterator<iterator>;
    using const_reverse_iterator = __UTL const_iterator<reverse_iterator>;

    static constexpr size_t extent = E;

    class __UTL_ABI_PUBLIC iterator : __UTL contiguous_iterator_base<iterator, element_type> {
        using base_type UTL_NODEBUG = contiguous_iterator_base<iterator, element_type>;

    public:
        using typename base_type::difference_type;
        using typename base_type::iterator_concept;
        using typename base_type::pointer;
        using typename base_type::reference;
        using typename base_type::value_type;

        __UTL_HIDE_FROM_ABI inline constexpr iterator() noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator(iterator const& other) noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator(iterator&& other) noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator& operator=(
            iterator const& other) noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator& operator=(
            iterator&& other) noexcept = default;
        using base_type::operator*;
        using base_type::operator->;

    private:
        friend span;
        __UTL_HIDE_FROM_ABI inline constexpr iterator(pointer data) noexcept : base_type(data) {}
    };

private:
    static constexpr size_type byte_span_size =
        extent == dynamic_extent ? dynamic_extent : extent * sizeof(T);
    __UTL_HIDE_FROM_ABI static constexpr size_t ensure_size(size_t n) noexcept {
        return extent == dynamic_extent || n == extent ? n : (UTL_ASSERT(!"Invalid size"), 0);
    }
    template <size_type Count>
    using subspan_result UTL_NODEBUG = span<element_type,
        (Count != dynamic_extent           ? Count
                : extent != dynamic_extent ? extent - Offset
                                           : dynamic_extent)>;

public:
    __UTL_HIDE_FROM_ABI inline constexpr span() noexcept : data_(nullptr), size_(0) {}

    __UTL_HIDE_FROM_ABI inline constexpr span(span const& other) noexcept = default;

    template <size_t N UTL_CONSTRAINT_CXX11(extent == dynamic_extent || extent == N)>
    UTL_CONSTRAINT_CXX20(extent == dynamic_extent || extent == N)
    __UTL_HIDE_FROM_ABI inline constexpr span(type_identity_t<element_type> (&a)[N]) noexcept
        : data_(a)
        , size_(N) {}
    template <typename U, size_t N UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(copy_cv_t<element_type, U>, element_type) &&
        (extent == dynamic_extent || extent == N))>
    UTL_CONSTRAINT_CXX20(same_as<copy_cv_t<element_type, U>, element_type> &&
        (extent == dynamic_extent || extent == N))
    __UTL_HIDE_FROM_ABI inline constexpr span(::std::array<U, N>& a) noexcept
        : data_(a.data())
        , size_(N) {}

    template <typename U, size_t N UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(copy_cv_t<element_type, U const>, element_type) &&
        (extent == dynamic_extent || extent == N))>
    UTL_CONSTRAINT_CXX20(same_as<copy_cv_t<element_type, U const>, element_type> &&
        (extent == dynamic_extent || extent == N))
    __UTL_HIDE_FROM_ABI inline constexpr span(::std::array<U, N> const& a) noexcept
        : data_(a.data())
        , size_(N) {}

#if UTL_CXX20
    template <contiguous_iterator P>
    __UTL_HIDE_FROM_ABI explicit(extent != dynamic_extent) inline constexpr span(
        P first, size_type count) noexcept(noexcept(__UTL to_address(first)))
        : data_(__UTL to_address(first))
        , size_(ensure_size(count)) {}

    template <contiguous_iterator It, sized_sentinel_for<It> End>
    __UTL_HIDE_FROM_ABI explicit(extent != dynamic_extent) inline constexpr span(
        It begin, End end) noexcept(noexcept(__UTL to_address(first)) && noexcept(end - begin))
        : data_(__UTL to_address(first))
        , size_(ensure_size(end - begin)) {}

    __UTL_HIDE_FROM_ABI inline constexpr span(::std::initializer_list<value_type> il) noexcept
    requires is_const_v<element_type>
        : data_(il.begin())
        , size_(ensure_size(il.size())) {}

    template <typename U, size_t N>
    requires (same_as<copy_cv_t<element_type, U const>, element_type> &&
                 (extent == dynamic_extent || N == dynamic_extent || E == N))
    __UTL_HIDE_FROM_ABI explicit(extent != dynamic_extent &&
        N == dynamic_extent) inline constexpr span(span<U, N> src) noexcept
        : data_(src.data())
        , size_(ensure_size(src.size())) {}

#else

    template <typename P,
        enable_if_t<extent != dynamic_extent && UTL_TRAIT_is_contiguous_iterator(P), int> = 0>
    __UTL_HIDE_FROM_ABI explicit inline constexpr span(P first, size_type count) noexcept(
        noexcept(__UTL to_address(first)))
        : data_(__UTL to_address(first))
        , size_(ensure_size(count)) {}

    template <typename P,
        enable_if_t<extent == dynamic_extent && UTL_TRAIT_is_contiguous_iterator(P), int> = 1>
    __UTL_HIDE_FROM_ABI inline constexpr span(P first, size_type count) noexcept(
        noexcept(__UTL to_address(first)))
        : data_(__UTL to_address(first))
        , size_(ensure_size(count)) {}

    template <typename It, typename End,
        enable_if_t<extent != dynamic_extent && UTL_TRAIT_is_contiguous_iterator(It) &&
                UTL_TRAIT_is_sized_sentinel_for(It, End),
            int> = 0>
    __UTL_HIDE_FROM_ABI explicit inline constexpr span(It begin, End end) noexcept(
        noexcept(__UTL to_address(first)) && noexcept(end - begin))
        : data_(__UTL to_address(first))
        , size_(ensure_size(end - begin)) {}

    template <typename It, typename End,
        enable_if_t<extent == dynamic_extent && UTL_TRAIT_is_contiguous_iterator(It) &&
                UTL_TRAIT_is_sized_sentinel_for(It, End),
            int> = 1>
    __UTL_HIDE_FROM_ABI inline constexpr span(It begin, End end) noexcept(
        noexcept(__UTL to_address(first)) && noexcept(end - begin))
        : data_(__UTL to_address(first))
        , size_(ensure_size(end - begin)) {}

    template <typename U,
        enable_if_t<UTL_TRAIT_is_const(element_type) && UTL_TRAIT_is_same(U, value_type), int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr span(::std::initializer_list<U> il) noexcept
        : data_(il.begin())
        , size_(ensure_size(il.size())) {}

    template <typename U, size_t N,
        enable_if_t<(extent != dynamic_extent && N == dynamic_extent) &&
                UTL_TRAIT_is_same(copy_cv_t<element_type, U const>, element_type) &&
                (extent == dynamic_extent || N == dynamic_extent || E == N),
            int> = 0>
    __UTL_HIDE_FROM_ABI explicit inline constexpr span(span<U, N> src) noexcept
        : data_(src.data())
        , size_(ensure_size(src.size())) {}

    template <typename U, size_t N,
        enable_if_t<!(extent != dynamic_extent && N == dynamic_extent) &&
                UTL_TRAIT_is_same(copy_cv_t<element_type, U const>, element_type) &&
                (extent == dynamic_extent || N == dynamic_extent || E == N),
            int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr span(span<U, N> src) noexcept
        : data_(src.data())
        , size_(ensure_size(src.size())) {}
#endif
    // TODO Ranges ctor

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 span& operator=(span const&) noexcept = default;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr iterator begin() const noexcept {
        return iterator(data());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr const_iterator cbegin() const noexcept {
        return const_iterator(data());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr const_iterator cend() const noexcept {
        return const_iterator(data() + size());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr iterator end() const noexcept {
        return iterator(data() + size());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr iterator rbegin() const noexcept {
        return reverse_iterator(end());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr const_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr iterator rend() const noexcept {
        return reverse_iterator(begin());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr const_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr reference front() const noexcept {
        return *data();
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr reference back() const noexcept {
        return data()[size() - 1];
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr reference operator[](
        size_type idx) noexcept {
        return data()[idx];
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr pointer data() const noexcept {
        return data_;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 reference at(size_type idx) const UTL_THROWS {
        UTL_THROW_IF(idx >= size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] span::at operation failed, "
                                            "Reason=[index out of range], idx=[%zu], size=[%zu]"),
                idx, size()));
        return data()[idx];
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr size_type size() const noexcept {
        return size_;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr size_type size_bytes() const noexcept {
        return size_ * sizeof(element_type);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr bool empty() const noexcept {
        return size_ == 0;
    }

    template <size_type Count UTL_CONSTRAINT_CXX11(
        Count != dynamic_extent && (extent == dynamic_extent || Count < extent))>
    UTL_CONSTRAINT_CXX20(Count != dynamic_extent && (extent == dynamic_extent || Count < extent))
    __UTL_HIDE_FROM_ABI inline constexpr span<element_type, Count> first() const noexcept {
        return span<element_type, Count>{data_, Count};
    }

    __UTL_HIDE_FROM_ABI inline constexpr span<element_type, dynamic_extent> first(
        size_type count) const noexcept {
        return span<element_type, dynamic_extent>{data_, count};
    }

    template <size_type Count UTL_CONSTRAINT_CXX11(
        Count != dynamic_extent && (extent == dynamic_extent || Count < extent))>
    UTL_CONSTRAINT_CXX20(Count != dynamic_extent && (extent == dynamic_extent || Count < extent))
    __UTL_HIDE_FROM_ABI inline constexpr span<element_type, Count> last() const noexcept {
        return span<element_type, Count>{
            data_ + (UTL_ASSERT(count < size()), (size() - count)), Count};
    }

    __UTL_HIDE_FROM_ABI inline constexpr span<element_type, dynamic_extent> last(
        size_type count) const noexcept {
        return count < size()
            ? span<element_type, dynamic_extent>{data_ + size() - count, count}
            : (UTL_ASSERT(!"Out of range"),
                  span<element_type, dynamic_extent>{data_ + size() - count, count});
    }

    template <size_type Offset, size_type Count = dynamic_extent UTL_CONSTRAINT_CXX11(
        (Offset > extent) || (Count != dynamic_extent && Count > (extent - Offset))) >
    UTL_CONSTRAINT_CXX20(Offset > extent || (Count != dynamic_extent && Count > (extent - Offset)))
    __UTL_HIDE_FROM_ABI inline constexpr subspan_result<Count> subspan() const noexcept {
        return Offset <= size() && (Count == dynamic_extent || Count <= (size() - Offset))
            ? subspan_result<Count>{data_ + Offset,
                  Count == dynamic_extent ? size() - Offset : Count}
            : (UTL_ASSERT(!"Out of range"),
                  subspan_result<Count>{
                      data_ + Offset, Count == dynamic_extent ? size() - Offset : Count});
    }

    __UTL_HIDE_FROM_ABI inline constexpr span<element_type, dynamic_extent> subspan(
        size_type offset, size_type count) const noexcept {
        using result_type UTL_NODEBUG = span<element_type, dynamic_extent>;
        return offset <= size() || (count <= (size() - offset))
            ? result_type{data_ + offset, count}
            : (UTL_ASSERT(!"Out of range"), result_type{data_ + offset, count});
    }

    __UTL_HIDE_FROM_ABI friend inline span<std::byte const, byte_span_size> as_bytes(
        span s) noexcept {
        using result_type UTL_NODEBUG = span<std::byte const, byte_span_size>;
        return result_type{reinterpret_cast<std::byte const*>(s.data()), s.size_bytes()};
    }

    __UTL_HIDE_FROM_ABI friend inline span<std::byte, byte_span_size> as_writable_bytes(
        span s) noexcept {
        using result_type UTL_NODEBUG = span<std::byte, byte_span_size>;
        return result_type{reinterpret_cast<std::byte*>(s.data()), s.size_bytes()};
    }

private:
    pointer data_;
    size_type size_;
};

UTL_NAMESPACE_END
