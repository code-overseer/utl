// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/array/utl_array_fwd.h"
#include "utl/initializer_list/utl_initializer_list_fwd.h"
#include "utl/span/utl_span_fwd.h"

#include "utl/byte/utl_byte.h"
#include "utl/concepts/utl_equality_comparable.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/exception/utl_program_exception.h"
#include "utl/iterator/utl_const_iterator.h"
#include "utl/iterator/utl_contiguous_iterator.h"
#include "utl/iterator/utl_contiguous_iterator_base.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_reverse_iterator.h"
#include "utl/iterator/utl_sized_sentinel_for.h"
#include "utl/type_traits/utl_copy_cv.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_const.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_equality_comparable.h"
#include "utl/type_traits/utl_is_inequality_comparable.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_remove_reference.h"
#include "utl/type_traits/utl_type_identity.h"

UTL_NAMESPACE_BEGIN

template <typename T, size_t E>
class __UTL_PUBLIC_TEMPLATE span {
public:
    using element_type = T;
    using value_type = __UTL remove_cv_t<T>;
    using size_type = decltype(sizeof(0));
    using difference_type = decltype(static_cast<char*>(0) - static_cast<char*>(0));
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
        UTL_INHERIT_CONTIGUOUS_ITERATOR_MEMBERS(iterator, element_type);

        __UTL_HIDE_FROM_ABI inline constexpr iterator() noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator(iterator const& other) noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator(iterator&& other) noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator& operator=(
            iterator const& other) noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator& operator=(
            iterator&& other) noexcept = default;

    private:
        friend span;
        __UTL_HIDE_FROM_ABI inline constexpr iterator(span::pointer data) noexcept
            : base_type(data) {}
    };

private:
    __UTL_HIDE_FROM_ABI static constexpr size_t ensure_size(size_t n) noexcept {
        return extent == dynamic_extent || n == extent ? n : (UTL_ASSERT(!"Invalid size"), 0);
    }
    template <size_type Offset, size_type Count>
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
        It begin, End end) noexcept(noexcept(__UTL to_address(begin)) && noexcept(end - begin))
        : data_(__UTL to_address(begin))
        , size_(ensure_size(end - begin)) {}

    __UTL_HIDE_FROM_ABI inline constexpr span(::std::initializer_list<value_type> il) noexcept
    requires is_const_v<element_type>
        : data_(il.begin())
        , size_(ensure_size(il.size())) {}

    template <typename U, size_t N>
    requires (same_as<copy_cv_t<element_type, U>, element_type> &&
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
        noexcept(__UTL to_address(begin)) && noexcept(end - begin))
        : data_(__UTL to_address(begin))
        , size_(ensure_size(end - begin)) {}

    template <typename It, typename End,
        enable_if_t<extent == dynamic_extent && UTL_TRAIT_is_contiguous_iterator(It) &&
                UTL_TRAIT_is_sized_sentinel_for(It, End),
            int> = 1>
    __UTL_HIDE_FROM_ABI inline constexpr span(It begin, End end) noexcept(
        noexcept(__UTL to_address(begin)) && noexcept(end - begin))
        : data_(__UTL to_address(begin))
        , size_(ensure_size(end - begin)) {}

    template <typename U,
        enable_if_t<UTL_TRAIT_is_const(element_type) && UTL_TRAIT_is_same(U, value_type), int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr span(::std::initializer_list<U> il) noexcept
        : data_(il.begin())
        , size_(ensure_size(il.size())) {}

    template <typename U, size_t N,
        enable_if_t<(extent != dynamic_extent && N == dynamic_extent) &&
                UTL_TRAIT_is_same(copy_cv_t<element_type, U>, element_type) &&
                (extent == dynamic_extent || N == dynamic_extent || E == N),
            int> = 0>
    __UTL_HIDE_FROM_ABI explicit inline constexpr span(span<U, N> src) noexcept
        : data_(src.data())
        , size_(ensure_size(src.size())) {}

    template <typename U, size_t N,
        enable_if_t<!(extent != dynamic_extent && N == dynamic_extent) &&
                UTL_TRAIT_is_same(copy_cv_t<element_type, U>, element_type) &&
                (extent == dynamic_extent || N == dynamic_extent || E == N),
            int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr span(span<U, N> src) noexcept
        : data_(src.data())
        , size_(ensure_size(src.size())) {}
#endif
    // TODO Ranges ctor

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 span& operator=(span const&) noexcept = default;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr iterator begin() const noexcept {
        return iterator(data());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr const_iterator
    cbegin() const noexcept {
        return const_iterator(data());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr const_iterator
    cend() const noexcept {
        return const_iterator(data() + size());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr iterator end() const noexcept {
        return iterator(data() + size());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr iterator rbegin() const noexcept {
        return reverse_iterator(end());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr const_iterator
    crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr iterator rend() const noexcept {
        return reverse_iterator(begin());
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr const_iterator
    crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr reference front() const noexcept {
        return *data();
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr reference back() const noexcept {
        return data()[size() - 1];
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr reference operator[](
        size_type idx) noexcept {
        return data()[idx];
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr pointer data() const noexcept {
        return data_;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline UTL_CONSTEXPR_CXX14 reference at(size_type idx) const UTL_THROWS {
        UTL_THROW_IF(idx >= size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] span::at operation failed, "
                                            "Reason=[index out of range], idx=[%zu], size=[%zu]"),
                idx, size()));
        return data()[idx];
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr size_type size() const noexcept {
        return size_;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr size_type
    size_bytes() const noexcept {
        return size_ * sizeof(element_type);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr bool empty() const noexcept {
        return size_ == 0;
    }

    template <size_type Count UTL_CONSTRAINT_CXX11(
        Count != dynamic_extent && (extent == dynamic_extent || Count < extent))>
    UTL_CONSTRAINT_CXX20(Count != dynamic_extent && (extent == dynamic_extent || Count < extent))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr span<element_type, Count>
    first() const noexcept {
        return span<element_type, Count>{data_, Count};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr span<element_type, dynamic_extent> first(
        size_type count) const noexcept {
        return span<element_type, dynamic_extent>{data_, count};
    }

    template <size_type Count UTL_CONSTRAINT_CXX11(
        Count != dynamic_extent && (extent == dynamic_extent || Count < extent))>
    UTL_CONSTRAINT_CXX20(Count != dynamic_extent && (extent == dynamic_extent || Count < extent))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr span<element_type, Count> last() const noexcept {
        return span<element_type, Count>{
            data_ + (UTL_ASSERT(Count < size()), (size() - Count)), Count};
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr span<element_type, dynamic_extent> last(
        size_type count) const noexcept {
        return count < size()
            ? span<element_type, dynamic_extent>{data_ + size() - count, count}
            : (UTL_ASSERT(!"Out of range"),
                  span<element_type, dynamic_extent>{data_ + size() - count, count});
    }

    template <size_type Offset, size_type Count = dynamic_extent UTL_CONSTRAINT_CXX11(
        (Offset > extent) || (Count != dynamic_extent && Count > (extent - Offset))) >
    UTL_CONSTRAINT_CXX20(Offset > extent || (Count != dynamic_extent && Count > (extent - Offset)))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr subspan_result<Offset, Count>
    subspan() const noexcept {
        return Offset <= size() && (Count == dynamic_extent || Count <= (size() - Offset))
            ? subspan_result<Offset, Count>{data_ + Offset,
                  Count == dynamic_extent ? size() - Offset : Count}
            : (UTL_ASSERT(!"Out of range"),
                  subspan_result<Offset, Count>{
                      data_ + Offset, Count == dynamic_extent ? size() - Offset : Count});
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr span<element_type, dynamic_extent> subspan(
        size_type offset, size_type count) const noexcept {
        using result_type UTL_NODEBUG = span<element_type, dynamic_extent>;
        return offset <= size() || (count <= (size() - offset))
            ? result_type{data_ + offset, count}
            : (UTL_ASSERT(!"Out of range"), result_type{data_ + offset, count});
    }

private:
    pointer data_;
    size_type size_;
};

namespace details {
namespace span {
template <typename T, size_t E>
inline UTL_CONSTEVAL size_t bytes_size() noexcept {
    return E == dynamic_extent ? dynamic_extent : E * sizeof(T);
}

#if UTL_CXX20
template <typename T>
concept integral_constant_like = is_integral_v<decltype(T::value)> &&
    !is_same_v<bool, remove_const_t<decltype(T::value)>> && convertible_to<T, decltype(T::value)> &&
    equality_comparable_with<T, decltype(T::value)> && bool_constant<T() == T::value>::value &&
    bool_constant<static_cast<decltype(T::value)>(T()) == T::value>::value;

template <class T>
UTL_INLINE_CXX17 constexpr size_t maybe_static_extent = dynamic_extent; // exposition only
template <UTL_CONCEPT_CXX20(integral_constant_like) T>
UTL_INLINE_CXX17 constexpr size_t maybe_static_extent<T> = {T::value};

#  define __UTL_MAYBE_STATIC_EXTENT(TYPE) __UTL details::span::maybe_static_extent<TYPE>

#else
template <typename T>
__UTL_HIDE_FROM_ABI auto integral_constant_like_impl(float) noexcept -> false_type;

template <typename T>
__UTL_HIDE_FROM_ABI auto integral_constant_like_impl(int) noexcept
    -> conjunction<is_integral<decltype(T::value)>,
        negation<is_same<bool, remove_const_t<decltype(T::value)>>>,
        is_convertible<T, decltype(T::value)>, is_equality_comparable_with<T, decltype(T::value)>,
        is_inequality_comparable_with<T, decltype(T::value)>, bool_constant<T() == T::value>,
        bool_constant<static_cast<decltype(T::value)>(T()) == T::value>>;

template <typename T>
using is_integral_constant_like = decltype(integral_constant_like_impl<T>(0));

template <typename T>
__UTL_HIDE_FROM_ABI auto maybe_static_extent_impl(float) noexcept -> size_constant<dynamic_extent>;
template <typename T>
__UTL_HIDE_FROM_ABI auto maybe_static_extent_impl(int) noexcept
    -> enable_if_t<is_integral_constant_like<T>::value, size_constant<T::value>>;

template <typename T>
using maybe_static_extent = decltype(maybe_static_extent_impl<T>(0));

#  define __UTL_MAYBE_STATIC_EXTENT(TYPE) __UTL details::span::maybe_static_extent<TYPE>::value

#endif

} // namespace span
} // namespace details

#if UTL_CXX17

template <typename It, typename E>
span(It, E) -> span<remove_reference_t<iter_reference_t<It>>, __UTL_MAYBE_STATIC_EXTENT(E)>;
template <typename T, size_t N>
span(T (&)[N]) -> span<T, N>;
template <typename T, size_t N>
span(::std::array<T, N>&) -> span<T, N>;
template <typename T, size_t N>
span(::std::array<T, N> const&) -> span<T const, N>;
// template<typename R>
// span(R&&) -> span<remove_reference_t<ranges::range_reference_t<R>>>;
#endif

template <typename T, size_t N>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline span<__UTL byte const, details::span::bytes_size<T, N>()>
as_bytes(span<T, N> s) noexcept {
    using result_type UTL_NODEBUG = span<__UTL byte const, details::span::bytes_size<T, N>()>;
    return result_type{reinterpret_cast<__UTL byte const*>(s.data()), s.size_bytes()};
}

template <typename T, size_t N UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_const(T))>
UTL_CONSTRAINT_CXX20(!is_const_v<T>)
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline span<__UTL byte, details::span::bytes_size<T, N>()>
as_writable_bytes(span<T, N> s) noexcept {
    using result_type UTL_NODEBUG = span<__UTL byte, details::span::bytes_size<T, N>()>;
    return result_type{reinterpret_cast<__UTL byte*>(s.data()), s.size_bytes()};
}

UTL_NAMESPACE_END
