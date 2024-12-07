// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/initializer_list/utl_initializer_list_fwd.h"
#include "utl/string/utl_string_fwd.h"

#include "utl/algorithm/utl_remove.h"
#include "utl/algorithm/utl_remove_if.h"
#include "utl/assert/utl_assert.h"
#include "utl/bit/utl_bit_ceil.h"
#include "utl/concepts/utl_convertible_to.h"
#include "utl/concepts/utl_integral.h"
#include "utl/exception.h"
#include "utl/iterator/utl_const_iterator.h"
#include "utl/iterator/utl_contiguous_iterator_base.h"
#include "utl/iterator/utl_distance.h"
#include "utl/iterator/utl_iterator_traits.h"
#include "utl/iterator/utl_legacy_forward_iterator.h"
#include "utl/iterator/utl_legacy_input_iterator.h"
#include "utl/iterator/utl_reverse_iterator.h"
#include "utl/memory/utl_allocator.h"
#include "utl/memory/utl_allocator_traits.h"
#include "utl/memory/utl_construct_at.h"
#include "utl/memory/utl_to_address.h"
#include "utl/numeric/utl_limits.h"
#include "utl/numeric/utl_max.h"
#include "utl/numeric/utl_min.h"
#include "utl/string/utl_basic_string_view.h"
#include "utl/string/utl_basic_zstring_view.h"
#include "utl/string/utl_string_details.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/type_traits/utl_type_identity.h"
#include "utl/utility/utl_as_const.h"
#include "utl/utility/utl_compressed_pair.h"

#include <climits>

#if UTL_WITH_EXCEPTIONS
#  define UTL_CONSTEXPR_WITH_TRY UTL_CONSTEXPR_CXX20
#else
#  define UTL_CONSTEXPR_WITH_TRY UTL_CONSTEXPR_CXX14
#endif

#define __UTL_ATTRIBUTE_STRING_PURE (PURE)(NODISCARD) __UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_STRING_PURE
#define __UTL_ATTRIBUTE_STRING_CONST (CONST)(NODISCARD) __UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_STRING_CONST

/**
 * Option to make string member definitions inline for C++ modules
 * To be used in conjunction with basic_string_abi
 */
#ifndef UTL_STRING_NO_INLINE
#  define __UTL_STRING_INLINE inline
#else
#  define __UTL_STRING_INLINE
#endif

UTL_NAMESPACE_BEGIN

template <typename CharType, size_t ShortSize, typename Traits, typename Alloc>
class __UTL_PUBLIC_TEMPLATE basic_short_string {
    static_assert(ShortSize >= details::string::default_inline_size<CharType, Alloc>::value,
        "Inline size must be longer than the default value");

public:
    using allocator_type = Alloc;
    using value_type = CharType;
    using size_type = typename allocator_traits<allocator_type>::size_type;
    using difference_type = typename allocator_traits<allocator_type>::difference_type;
    using traits_type = Traits;
    using pointer = typename allocator_traits<allocator_type>::pointer;
    using const_pointer = typename allocator_traits<allocator_type>::const_pointer;
    using reference = CharType&;
    using const_reference = CharType const&;
    using view_type = basic_string_view<value_type, traits_type>;
    __UTL_PUBLIC_TEMPLATE_DATA static constexpr size_type npos = -1;

    class iterator;

private:
    using alloc_traits = allocator_traits<allocator_type>;
    static constexpr size_type inline_size = ShortSize + 1;

    struct short_type {
        value_type data_[inline_size];
    };

    struct heap_type {
        pointer data_;
        /**
         * Capacity includes null-chacacter
         */
        size_type capacity_;

        __UTL_HIDE_FROM_ABI inline constexpr heap_type() noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr heap_type(heap_type const&) noexcept = default;
        __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 heap_type& operator=(
            heap_type const&) noexcept = default;

        __UTL_HIDE_FROM_ABI inline constexpr heap_type(pointer data, size_type capacity) noexcept
            : data_(data)
            , capacity_(capacity) {}

        __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 heap_type& operator=(
            allocation_result<pointer, size_type>&& other) noexcept {
            data_ = __UTL move(other.ptr);
            capacity_ = other.size;
            return *this;
        }
        __UTL_HIDE_FROM_ABI inline constexpr
        operator allocation_result<pointer, size_type>() const noexcept {
            return {data_, capacity_};
        }
    };

    union data_union {
        short_type short_;
        heap_type heap_;
    };

    using storage_type = compressed_pair<data_union, allocator_type>;

public:
    class __UTL_ABI_PUBLIC iterator : __UTL contiguous_iterator_base<iterator, value_type> {
        using base_type = contiguous_iterator_base<iterator, value_type>;
        friend basic_short_string;

    public:
        UTL_INHERIT_CONTIGUOUS_ITERATOR_MEMBERS(iterator, value_type);

        __UTL_HIDE_FROM_ABI inline constexpr iterator() noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator(iterator const& other) noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator(iterator&& other) noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator& operator=(
            iterator const& other) noexcept = default;
        __UTL_HIDE_FROM_ABI inline constexpr iterator& operator=(
            iterator&& other) noexcept = default;

    private:
        __UTL_HIDE_FROM_ABI inline constexpr iterator(value_type* data) noexcept
            : base_type(data) {}
        template <typename It>
        __UTL_HIDE_FROM_ABI inline constexpr iterator(It other) noexcept
            : iterator(const_cast<pointer>(__UTL to_address(other))) {}
    };

    using const_iterator = __UTL const_iterator<iterator>;
    using reverse_iterator = __UTL reverse_iterator<iterator>;
    using const_reverse_iterator = __UTL const_iterator<reverse_iterator>;

    basic_short_string(decltype(nullptr)) = delete;
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE constexpr basic_short_string() noexcept(
        noexcept(allocator_type()))
        : basic_short_string(allocator_type()) {}

    __UTL_HIDE_FROM_ABI explicit __UTL_STRING_INLINE constexpr basic_short_string(
        allocator_type const& a) noexcept
        : storage_(details::compressed_pair::default_initialize, a)
        , size_()
        , is_heap_() {}

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(
        value_type const* str, size_type len, allocator_type const& a = allocator_type())
        UTL_THROWS
        : basic_short_string(a) {
        assign(str, len);
    }

    template <UTL_CONCEPT_CXX20(__UTL legacy_input_iterator) It UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_legacy_input_iterator(It))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(
        It first, It last, allocator_type const& a = allocator_type()) UTL_THROWS
        : basic_short_string(a) {
        assign(first, last);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(
        value_type const* str, allocator_type const& a = allocator_type()) UTL_THROWS
        : basic_short_string(str, traits_type::length(str), a) {}

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(
        ::std::initializer_list<value_type> ilist,
        allocator_type const& a = allocator_type()) UTL_THROWS
        : basic_short_string(ilist.begin(), ilist.size(), a) {}

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(
        size_type count, value_type ch, allocator_type const& a = allocator_type()) UTL_THROWS
        : basic_short_string(a) {
        resize(count, ch);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI explicit __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(View const& view,
        allocator_type const& a = allocator_type()) noexcept(!utl::with_exceptions &&
        UTL_TRAIT_is_nothrow_convertible(View, view_type))
        : basic_short_string(a) {
        assign(view);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(View const& view,
        size_type pos, size_type n,
        allocator_type const& a = allocator_type()) noexcept(!utl::with_exceptions &&
        UTL_TRAIT_is_nothrow_convertible(View, view_type))
        : basic_short_string(a) {
        assign(view, pos, n);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(
        basic_short_string const& other) UTL_THROWS
        : storage_(other.get_union(),
              alloc_traits::select_on_container_copy_construction(other.allocator_ref()))
        , size_(other.size_)
        , is_heap_(other.is_heap_) {
        if (other.is_heap_) {
            storage_.get_heap() = clone_heap(allocator_ref(), other);
        }
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(
        basic_short_string const& other, size_type pos, size_type count,
        allocator_type const& alloc = allocator_type()) UTL_THROWS
        : storage_(other.get_union(), alloc)
        , size_(other.size_)
        , is_heap_(other.is_heap_) {
        UTL_THROW_IF(pos > other.size(),
            out_of_range(UTL_MESSAGE_FORMAT(
                             "[UTL] basic_short_string construction failed, Reason=[index out of "
                             "range], pos=[%zu], size=[%zu]"),
                pos, other.size()));

        if (other.is_heap_) {
            storage_.get_heap() = clone_heap(allocator_ref(), other);
        }
        inline_substring(pos, count);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(
        basic_short_string const& other, size_type pos,
        allocator_type const& alloc = allocator_type()) UTL_THROWS
        : basic_short_string(other, pos, other.size(), alloc) {}

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& operator=(
        basic_short_string const& other) UTL_THROWS {
        if (this != __UTL addressof(other)) {
            destroy();
            other.get_union() = other.get_union();
            alloc_traits::assign(allocator_ref(), other.allocator_ref());
            size_ = other.size_;
            is_heap_ = other.is_heap_;

            if (other.is_heap_) {
                storage_.get_heap() = clone_heap(allocator_ref(), other);
            }
        }

        return *this;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(
        basic_short_string&& other) noexcept
        : storage_(other.storage_)
        , size_(other.size_)
        , is_heap_(other.is_heap_) {
        __UTL move(other).reset_to_short();
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(basic_short_string&& other,
        size_type pos, size_type count, allocator_type const& alloc = allocator_type()) UTL_THROWS
        : storage_(other.get_union(), alloc)
        , size_(other.size_)
        , is_heap_(other.is_heap_) {
        UTL_THROW_IF(pos > other.size(),
            out_of_range(UTL_MESSAGE_FORMAT(
                             "[UTL] basic_short_string construction failed, Reason=[index out of "
                             "range], pos=[%zu], size=[%zu]"),
                pos, other.size()));

        on_move_construct_with_alloc(other, pos, count, alloc_traits::is_always_equal);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string(basic_short_string&& other,
        size_type pos, allocator_type const& alloc = allocator_type()) UTL_THROWS
        : basic_short_string(__UTL move(other), pos, other.size(), alloc) {}

    // TODO: container-compatible-ranges ctor

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE constexpr operator view_type() const noexcept UTL_LIFETIMEBOUND {
        return view_type{data(), size()};
    }
    __UTL_HIDE_FROM_ABI explicit __UTL_STRING_INLINE constexpr
    operator basic_zstring_view<value_type, traits_type>() const noexcept UTL_LIFETIMEBOUND {
        return basic_zstring_view<value_type, traits_type>{data(), size()};
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string&
    operator=(basic_short_string&& other) noexcept(
        (alloc_traits::propagate_on_container_move_assignment::value &&
            alloc_traits::is_always_equal::value) ||
        !utl::with_exceptions) {
        using can_move_t =
            conjunction<typename alloc_traits::propagate_on_container_move_assignment,
                typename alloc_traits::is_always_equal>;

        move_assign(other, can_move_t());
        return *this;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX20 ~basic_short_string() noexcept { destroy(); }

    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 value_type* data() noexcept UTL_LIFETIMEBOUND {
        return !is_heap_ ? get_short().data_ : __UTL to_address(get_heap().data_);
    }

    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr value_type const* data() const noexcept UTL_LIFETIMEBOUND {
        return !is_heap_ ? get_short().data_ : __UTL to_address(get_heap().data_);
    }

    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_pointer c_str() const noexcept UTL_LIFETIMEBOUND {
        return data();
    }

    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr bool empty() const noexcept { return !size_; }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr size_type size() const noexcept { return size_; }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr size_type length() const noexcept {
        return size_;
    }

    UTL_ATTRIBUTE(STRING_CONST) __UTL_STRING_INLINE constexpr size_type max_size() const noexcept {
        return numeric::maximum<size_type>::value >> 1;
    }

    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr size_type capacity() const noexcept {
        size_type const short_capacity = sizeof(short_type::data_) / sizeof(value_type);
        size_type const buffer_capacity = !is_heap_ ? short_capacity : get_heap().capacity_;
        return buffer_capacity - 1;
    }

    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr allocator_type get_allocator() const noexcept {
        return allocator_ref();
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 void reserve(
        size_type new_capacity) UTL_THROWS {
        if (new_capacity <= this->capacity()) {
            return;
        }

        UTL_THROW_IF(new_capacity > max_size(),
            length_error(UTL_MESSAGE_FORMAT("[UTL] basic_short_string::reserve operation failed, "
                                            "Reason=[Requested capacity exceeds maximum size], "
                                            "capacity=[%zu], limit=[%zu]"),
                new_capacity, max_size()));
        reserve_impl(new_capacity);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 void resize(size_type new_size)
        UTL_THROWS {
        resize(new_size, value_type());
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 void resize(
        size_type new_size, value_type ch) UTL_THROWS {
        reserve(new_size);
        traits_type::assign(data() + size(), __UTL numeric::max(new_size, size()) - size(), ch);
        data()[new_size] = value_type();
        size_ = new_size;
    }

    template <typename Op UTL_CONSTRAINT_CXX11(is_integral<decltype(__UTL declval<Op>()(
            static_cast<value_type*>(0), size_type{}))>::value)>
    UTL_CONSTRAINT_CXX20(requires(Op op, value_type* p, size_type s) {
        { op(p, s) } -> integral;
    })
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_WITH_TRY void resize_and_overwrite(
        size_type new_size, Op operation) UTL_THROWS {
        reserve(new_size);
        UTL_TRY {
            size_ = move(operation)(data(), new_size);
            data()[size_] = value_type();
            // Thrown exception is UB so assert
            UTL_ASSERT(size_ < new_size);
        } UTL_CATCH(...) {
            UTL_ASSERT(false);
        }
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 void shrink_to_fit() UTL_THROWS {
        if (is_heap_ && size() < capacity()) {
            get_heap() = alloc_traits::reallocate(allocator_ref(), get_heap(), size() + 1);
        }
    }

    UTL_ATTRIBUTES(REINITIALIZES, _HIDE_FROM_ABI) __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 void clear() noexcept {
        *data() = 0;
        size_ = 0;
    }

    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 iterator begin() noexcept UTL_LIFETIMEBOUND {
        return iterator(data());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_iterator cbegin() const noexcept UTL_LIFETIMEBOUND {
        return const_iterator(data());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_iterator begin() const noexcept UTL_LIFETIMEBOUND {
        return const_iterator(data());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 iterator end() noexcept UTL_LIFETIMEBOUND {
        return iterator(data() + size());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_iterator cend() const noexcept UTL_LIFETIMEBOUND {
        return const_iterator(data() + size());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_iterator end() const noexcept UTL_LIFETIMEBOUND {
        return const_iterator(data() + size());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 iterator rbegin() noexcept UTL_LIFETIMEBOUND {
        return reverse_iterator(end());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_iterator crbegin() const noexcept UTL_LIFETIMEBOUND {
        return const_reverse_iterator(end());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_iterator rbegin() const noexcept UTL_LIFETIMEBOUND {
        return const_reverse_iterator(end());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 iterator rend() noexcept UTL_LIFETIMEBOUND {
        return reverse_iterator(begin());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_iterator crend() const noexcept UTL_LIFETIMEBOUND {
        return const_reverse_iterator(begin());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_iterator rend() const noexcept UTL_LIFETIMEBOUND {
        return const_reverse_iterator(begin());
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_reference front() const noexcept UTL_LIFETIMEBOUND {
        return *data();
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 reference front() noexcept UTL_LIFETIMEBOUND {
        return *data();
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_reference back() const noexcept UTL_LIFETIMEBOUND {
        return data()[size() - 1];
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 reference back() noexcept UTL_LIFETIMEBOUND {
        return data()[size() - 1];
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_reference operator[](
        size_type idx) const noexcept UTL_LIFETIMEBOUND {
        return data()[idx];
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 reference operator[](size_type idx) noexcept UTL_LIFETIMEBOUND {
        return data()[idx];
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 reference at(size_type idx) UTL_THROWS UTL_LIFETIMEBOUND {
        return const_cast<reference>(as_const(*this).data()[idx]);
    }
    UTL_ATTRIBUTE(STRING_PURE) __UTL_STRING_INLINE constexpr const_reference at(size_type idx) const UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_THROW_IF(idx >= size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] basic_short_string::at operation failed, "
                                            "Reason=[index out of range], idx=[%zu], size=[%zu]"),
                idx, size()));
        return data()[idx];
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 void push_back(value_type ch)
        UTL_THROWS {
        reserve(size() + 1);
        data()[size_++] = ch;
        data()[size_] = value_type();
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 void pop_back() noexcept {
        data()[--size_] = value_type();
    }

    basic_short_string& assign(decltype(nullptr)) = delete;

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_WITH_TRY basic_short_string& assign(
        size_type count, value_type ch) UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_ATTRIBUTE(MAYBE_UNUSED) size_type const old_size = exchange(size_, 0);
        UTL_TRY {
            resize(count, ch);
            return *this;
        } UTL_CATCH(...) {
            size_ = old_size;
            UTL_RETHROW(
                "[UTL] `utl::basic_short_string::assign` failed, Reason=[allocation failure]");
        }
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        basic_short_string const& other) UTL_THROWS UTL_LIFETIMEBOUND {
        return *this = other;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        basic_short_string const& other, size_type pos, size_type count = npos)
        UTL_THROWS UTL_LIFETIMEBOUND {
        if (this == addressof(other)) {
            return *this;
        }

        UTL_THROW_IF(pos > other.size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] basic_short_string::assign operation failed, "
                                            "Reason=[substring argument index is out of range], "
                                            "pos=[%zu], size=[%zu], str=[%.*s%s"),
                pos, other.size()));

        count = count > other.size() ? other.size() : count;
        resize(count);
        traits_type::copy(data(), other.data() + pos, count + 1);
        return *this;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        basic_short_string&& other) UTL_THROWS UTL_LIFETIMEBOUND {
        return *this = move(other);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        value_type const* str, size_type count) UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_ASSERT(str != nullptr);
        reserve(count);
        traits_type::copy(data(), str, count)[count] = value_type();
        size_ = count;
        return *this;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        value_type const* str) UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        assign(str, traits_type::length(str));
    }

    template <UTL_CONCEPT_CXX20(__UTL legacy_input_iterator) It UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_legacy_input_iterator(It) && !UTL_TRAIT_is_legacy_forward_iterator(It))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        It begin, It end) UTL_THROWS UTL_LIFETIMEBOUND {
        size_ = 0;
        while (begin != end) {
            push_back(*begin);
            ++begin;
        }
    }

    template <UTL_CONCEPT_CXX20(__UTL legacy_forward_iterator) It UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_legacy_forward_iterator(It))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX17 basic_short_string& assign(
        It begin, It end) UTL_THROWS UTL_LIFETIMEBOUND {
        auto const diff = __UTL distance(begin, end);
        if (diff < 0) UTL_ATTRIBUTE(UNLIKELY) {
            return *this;
        }

        reserve(diff);
        auto ptr = data();
        while (begin != end) {
            *ptr = *begin;
            ++begin;
            ++ptr;
        }
        size_ = diff;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        ::std::initializer_list<value_type> list) UTL_THROWS UTL_LIFETIMEBOUND {
        assign(list.begin(), list.end());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        View const& view) UTL_THROWS UTL_LIFETIMEBOUND {
        view_type const v(view);
        return assign(v.data(), v.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& assign(View const& view,
        size_type subidx, size_type subcount = npos) UTL_THROWS UTL_LIFETIMEBOUND {
        view_type const v(view);
        UTL_THROW_IF(subidx > v.size(),
            out_of_range(
                UTL_MESSAGE_FORMAT(
                    "[UTL] `basic_short_string::assign` operation failed, "
                    "Reason=[argument substring index out of range], idx=[%zu], size=[%zu]"),
                subidx, v.size()));
        return assign(v.substr(subidx, subcount));
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& insert(
        size_type pos, size_type count, value_type ch) UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_THROW_IF(pos > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_short_string::insert` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                pos, size()));
        reserve(size() + count);
        auto const src = data() + pos;
        auto const dst = src + count;
        traits_type::move(dst, src, size() + 1 - pos);
        traits_type::assign(src, count, ch);
        size_ = size() + count;
        return begin() + pos;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& insert(
        size_type pos, value_type const* str, size_type length) UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_THROW_IF(pos > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_short_string::insert` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                pos, size()));
        reserve(size() + length);
        auto const src = data() + pos;
        auto const dst = src + length;
        traits_type::move(dst, src, size() + 1 - pos);
        traits_type::move(src, str, length);
        size_ = size() + length;
        return *this;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& insert(
        size_type pos, value_type const* str) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(pos, str, traits_type::length(str));
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& insert(
        size_type pos, basic_short_string const& str) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(pos, str.data(), str.size());
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& insert(size_type pos,
        basic_short_string const& str, size_type idx, size_type count = npos)
        UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_THROW_IF(idx > str.size(),
            out_of_range(
                UTL_MESSAGE_FORMAT(
                    "[UTL] `basic_short_string::insert` operation failed, "
                    "Reason=[argument substring index out of range], idx=[%zu], size=[%zu]"),
                idx, str.size()));
        auto const copy_size = __UTL numeric::min(str.size() - idx, count);
        return insert(pos, str.data() + idx, copy_size);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 iterator insert(
        const_iterator pos, size_type count, value_type ch) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(pos - cbegin(), count, ch);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 iterator insert(
        const_iterator pos, value_type ch) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(pos, 1, ch);
    }

    template <UTL_CONCEPT_CXX20(__UTL legacy_input_iterator) It UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_legacy_input_iterator(It) && !UTL_TRAIT_is_legacy_forward_iterator(It))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 iterator insert(
        const_iterator pos, It first, It last) UTL_THROWS UTL_LIFETIMEBOUND {
        auto const idx = pos - begin();
        while (first != last) {
            insert(pos, *first);
            ++first;
        }

        return iterator(begin() + idx);
    }

    template <UTL_CONCEPT_CXX20(__UTL legacy_forward_iterator) It UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_legacy_forward_iterator(It))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 iterator insert(
        const_iterator pos, It first, It last) UTL_THROWS UTL_LIFETIMEBOUND {
        auto const length = __UTL distance(first, last);
        if (length < 0) UTL_ATTRIBUTE(UNLIKELY) {
            return *this;
        }

        auto const idx = pos - cbegin();
        reserve(size() + length);
        auto const src = data() + idx;
        auto const dst = src + length;
        traits_type::move(dst, src, size() + 1 - idx);
        while (first != last) {
            *pos = *first;
            ++first;
            ++pos;
        }

        size_ = size() + length;
        return iterator(data() + idx);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 iterator insert(const_iterator pos,
        ::std::initializer_list<value_type> list) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(pos, list.begin(), list.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& insert(
        size_type pos, View const& view) UTL_THROWS UTL_LIFETIMEBOUND {
        view_type const v(view);
        return insert(pos, v.data(), v.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_short_string& insert(size_type pos, View const& view,
        size_type subidx, size_type subcount = npos) UTL_THROWS UTL_LIFETIMEBOUND {
        view_type const v(view);
        UTL_THROW_IF(subidx > v.size(),
            out_of_range(
                UTL_MESSAGE_FORMAT(
                    "[UTL] `basic_short_string::insert` operation failed, "
                    "Reason=[argument substring index out of range], idx=[%zu], size=[%zu]"),
                subidx, v.size()));
        return insert(pos, v.substr(subidx, subcount));
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& erase(
        size_type idx = 0, size_type count = npos) UTL_THROWS UTL_LIFETIMEBOUND {
        auto const length = __UTL numeric::min(size() - idx, count);
        auto const first = begin() + idx;
        return erase(first, first + length), *this;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 iterator erase(iterator pos) noexcept UTL_LIFETIMEBOUND {
        return erase(pos, pos + 1);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 iterator erase(const_iterator pos) noexcept UTL_LIFETIMEBOUND {
        return erase(pos, pos + 1);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 iterator erase(
        const_iterator first, const_iterator last) noexcept UTL_LIFETIMEBOUND {
        return erase(iterator(first), iterator(last));
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 iterator erase(iterator first, iterator last) noexcept UTL_LIFETIMEBOUND {
        auto const length = last - first;
        auto const src = last.operator->();
        auto const dst = first.operator->();
        auto const move_length = (end() - last) + (length > 0);
        traits_type::move(dst, src, move_length);
        size_ = size() - length;
        return iterator(dst + length);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& append(
        size_type count, value_type ch) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(size(), count, ch);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& append(
        basic_short_string const& str) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(size(), str);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& append(
        basic_short_string const& str, size_type idx, size_type count = npos)
        UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(size(), str, idx, count);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& append(
        value_type const* str) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(size(), str);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& append(
        value_type const* str, size_type count) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(size(), str, count);
    }

    template <UTL_CONCEPT_CXX20(__UTL legacy_input_iterator) It UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_legacy_input_iterator(It))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& append(
        It first, It last) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(end(), first, last), *this;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& append(
        ::std::initializer_list<value_type> list) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(size(), list.begin(), list.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& append(
        View const& view) UTL_THROWS UTL_LIFETIMEBOUND {
        return insert(size(), view);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& append(View const& view,
        size_type subidx, size_type subcount = npos) UTL_THROWS UTL_LIFETIMEBOUND {
        view_type const v(view);
        UTL_THROW_IF(subidx > v.size(),
            out_of_range(
                UTL_MESSAGE_FORMAT(
                    "[UTL] `basic_short_string::append` operation failed, "
                    "Reason=[argument substring index out of range], idx=[%zu], size=[%zu]"),
                subidx, v.size()));
        return insert(size(), v.substr(subidx, subcount));
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_short_string& operator+=(
        basic_short_string const& str) UTL_THROWS UTL_LIFETIMEBOUND {
        return append(str);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_short_string& operator+=(value_type ch)
        UTL_THROWS UTL_LIFETIMEBOUND {
        return append(1, ch);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_short_string& operator+=(
        value_type const* str) UTL_THROWS UTL_LIFETIMEBOUND {
        return append(str);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_short_string& operator+=(
        ::std::initializer_list<value_type> list) UTL_THROWS UTL_LIFETIMEBOUND {
        return append(list);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_short_string& operator+=(View const& view)
        UTL_THROWS UTL_LIFETIMEBOUND {
        return append(view);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(size_type pos,
        size_type count, basic_short_string const& str) UTL_THROWS UTL_LIFETIMEBOUND {
        return replace(pos, count, str.data(), str.size());
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, basic_short_string const& str)
        UTL_THROWS UTL_LIFETIMEBOUND {
        return replace(first, last, str.data(), str.size());
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(size_type pos,
        size_type count, basic_short_string const& str, size_type subidx,
        size_type subcount = npos) UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_THROW_IF(subidx > str.size(),
            out_of_range(
                UTL_MESSAGE_FORMAT(
                    "[UTL] `basic_short_string::replace` operation failed, "
                    "Reason=[argument substring index out of range], idx=[%zu], size=[%zu]"),
                subidx, str.size()));
        view_type const view = view_type(str).substr(subidx, subcount);
        return replace(pos, count, view.data(), view.size());
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(size_type pos,
        size_type count, value_type const* str, size_type length) UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_THROW_IF(pos > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_short_string::replace` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                pos, size()));
        auto const first = begin() + pos;
        return replace(first, first + count, str, length);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, value_type const* str, size_type length)
        UTL_THROWS UTL_LIFETIMEBOUND {
        auto const replaced_count = last - first;
        auto copy_str = [p = const_cast<pointer>(first.operator->()), str](
                            size_type count) { return traits_type::move(p, str, count); };

        if (replaced_count < length) {
            copy_str(replaced_count);
            insert(first + replaced_count, str + replaced_count, length - replaced_count);
            return *this;
        }

        copy_str(length);
        erase(first + length, first + replaced_count);
        return *this;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        size_type pos, size_type count, value_type const* str) UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        return replace(pos, count, str, traits_type::length(str));
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, value_type const* str)
        UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_ASSERT(str != nullptr);
        return replace(first, last, str, traits_type::length(str));
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(size_type pos,
        size_type count, size_type char_count, value_type ch) UTL_THROWS UTL_LIFETIMEBOUND {
        UTL_THROW_IF(pos > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_short_string::replace` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                pos, size()));
        auto const first = cbegin() + pos;
        return replace(first, first + count, char_count, ch);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, size_type char_count, value_type ch)
        UTL_THROWS UTL_LIFETIMEBOUND {
        auto const replaced_count = last - first;
        auto const assign_ch = [p = const_cast<pointer>(first.operator->()), ch](
                                   size_type cnt) { return traits_type::assign(p, cnt, ch); };

        if (replaced_count < char_count) {
            assign_ch(replaced_count);
            insert(first + replaced_count, char_count - replaced_count, ch);
            return *this;
        }

        assign_ch(char_count);
        erase(first + char_count, first + replaced_count);
        return *this;
    }

    template <UTL_CONCEPT_CXX20(__UTL legacy_input_iterator) It UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_legacy_input_iterator(It))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, It in_first, It in_last)
        UTL_THROWS UTL_LIFETIMEBOUND {
        while (first != last && in_first != in_last) {
            const_cast<reference>(*first) = *in_first;
            ++first;
            ++in_last;
        }

        erase(first, last);
        insert(last, in_first, in_last);
        return *this;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last,
        ::std::initializer_list<value_type> list) UTL_THROWS UTL_LIFETIMEBOUND {
        return replace(first, last, list.begin(), list.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, View const& view) UTL_THROWS UTL_LIFETIMEBOUND {
        view_type const v(view);
        return replace(first, last, v.data(), v.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, View const& view, size_type subidx,
        size_type subcount = npos) UTL_THROWS UTL_LIFETIMEBOUND {
        view_type const v(view);
        UTL_THROW_IF(subidx > v.size(),
            out_of_range(
                UTL_MESSAGE_FORMAT(
                    "[UTL] `basic_short_string::insert` operation failed, "
                    "Reason=[argument substring index out of range], idx=[%zu], size=[%zu]"),
                subidx, v.size()));
        return replace(first, last, v.substr(subidx, subcount));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        size_type idx, size_type count, View const& view) UTL_THROWS UTL_LIFETIMEBOUND {
        auto const first = cbegin() + idx;
        return replace(first, first + count, view);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 basic_short_string& replace(size_type idx,
        size_type count, View const& view, size_type subidx, size_type subcount = npos)
        UTL_THROWS UTL_LIFETIMEBOUND {
        view_type const v(view);
        UTL_THROW_IF(subidx > v.size(),
            out_of_range(
                UTL_MESSAGE_FORMAT(
                    "[UTL] `basic_short_string::insert` operation failed, "
                    "Reason=[argument substring index out of range], idx=[%zu], size=[%zu]"),
                subidx, v.size()));
        return replace(idx, count, v.substr(subidx, subcount));
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 size_type copy(
        pointer dst, size_type count, size_type pos = 0) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_short_string::replace` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                pos, size()));

        auto const copied = __UTL numeric::min(count, size() - pos);
        traits_type::copy(dst, data(), copied);
        return copied;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(basic_short_string& other) noexcept(
        alloc_traits::propagate_on_container_swap::value || alloc_traits::is_always_equal::value) {
        using swap_alloc_t = bool_constant<alloc_traits::propagate_on_container_swap::value ||
            !alloc_traits::is_always_equal::value>;
        swap(other, swap_alloc_t{});
    }

    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void swap(
        basic_short_string& l, basic_short_string& r) noexcept(noexcept(l.swap(r))) {
        l.swap(r);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find(
        basic_short_string const& str, size_type pos = 0) const noexcept {
        return find(str.data(), pos, str.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find(
        value_type const* str, size_type pos, size_type str_len) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return details::string::find<traits_type>(data(), size(), str, str_len, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find(
        value_type const* str, size_type pos = 0) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return find(str, pos, traits_type::length(str));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find(
        value_type ch, size_type pos = 0) const noexcept {
        return details::string::find<traits_type>(data(), size(), ch, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find(
        view_type view, size_type pos = 0) const noexcept {
        return find(view.data(), pos, view.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) inline constexpr size_type find(View const& view,
        size_type pos = 0) const noexcept(UTL_TRAIT_is_nothrow_convertible(View, view_type)) {
        return find(view_type(view), pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type rfind(
        basic_short_string const& str, size_type pos = npos) const noexcept {
        return rfind(str.data(), pos, str.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type rfind(
        value_type const* str, size_type pos, size_type str_len) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return details::string::rfind<traits_type>(data(), size(), str, str_len, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type rfind(
        value_type const* str, size_type pos = npos) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return rfind(str, pos, traits_type::length(str));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type rfind(
        value_type ch, size_type pos = npos) const noexcept {
        return details::string::rfind<traits_type>(data(), size(), ch, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type rfind(
        view_type view, size_type pos = npos) const noexcept {
        return rfind(view.data(), pos, view.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    UTL_ATTRIBUTES(NODISCARD, _ABI_PRIVATE) inline constexpr size_type rfind(View const& view,
        size_type pos = npos) const noexcept(UTL_TRAIT_is_nothrow_convertible(View, view_type)) {
        return rfind(view_type(view), pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_first_of(
        basic_short_string const& chars, size_type pos = 0) const noexcept {
        return find_first_of(chars.data(), pos, chars.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_first_of(
        value_type const* chars, size_type pos, size_type chars_count) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return details::string::find_first_of<traits_type>(data(), size(), chars, chars_count, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_first_of(
        value_type const* chars, size_type pos = 0) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return find_first_of(chars, pos, traits_type::length(chars));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_first_of(
        value_type ch, size_type pos = 0) const noexcept {
        return details::string::find_first_of<traits_type>(data(), size(), ch, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_first_of(
        view_type view, size_type pos = 0) const noexcept {
        return find_first_of(view.data(), pos, view.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    UTL_ATTRIBUTES(NODISCARD, _ABI_PRIVATE) inline constexpr size_type find_first_of(View const& view,
        size_type pos = 0) const noexcept(UTL_TRAIT_is_nothrow_convertible(View, view_type)) {
        return find_first_of(view_type(view), pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_first_not_of(
        basic_short_string const& chars, size_type pos = 0) const noexcept {
        return find_first_not_of(chars.data(), pos, chars.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_first_not_of(
        value_type const* chars, size_type pos, size_type chars_count) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return details::string::find_first_not_of<traits_type>(
            data(), size(), chars, chars_count, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_first_not_of(
        value_type const* chars, size_type pos = 0) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return find_first_not_of(chars, pos, traits_type::length(chars));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_first_not_of(
        value_type ch, size_type pos = 0) const noexcept {
        return find_first_not_of(&ch, pos, 1);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_first_not_of(
        view_type view, size_type pos = 0) const noexcept {
        return find_first_not_of(view.data(), pos, view.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) inline constexpr size_type find_first_not_of(View const& view,
        size_type pos = 0) const noexcept(UTL_TRAIT_is_nothrow_convertible(View, view_type)) {
        return find_first_not_of(view_type(view), pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_last_of(
        basic_short_string const& chars, size_type pos = npos) const noexcept {
        return find_last_of(chars.data(), pos, chars.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_last_of(
        value_type const* chars, size_type pos, size_type chars_count) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return details::string::find_last_of<traits_type>(data(), size(), chars, chars_count, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_last_of(
        value_type const* chars, size_type pos = npos) const noexcept {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return find_last_of(chars, pos, traits_type::length(chars));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_last_of(
        value_type ch, size_type pos = npos) const noexcept {
        return find_last_of(&ch, pos, 1);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_last_of(
        view_type view, size_type pos = npos) const noexcept {
        return find_last_of(view.data(), pos, view.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    UTL_ATTRIBUTES(NODISCARD, _ABI_PRIVATE) inline constexpr size_type find_last_of(View const& view,
        size_type pos = npos) const noexcept(UTL_TRAIT_is_nothrow_convertible(View, view_type)) {
        return find_last_of(view_type(view), pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_last_not_of(
        basic_short_string const& chars, size_type pos = npos) const noexcept {
        return find_last_not_of(chars.data(), pos, chars.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_last_not_of(
        value_type const* chars, size_type pos, size_type count) const UTL_THROWS {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return details::string::find_last_not_of<traits_type>(data(), size(), chars, count, pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_last_not_of(
        value_type const* chars, size_type pos = npos) const UTL_THROWS {
        // CONSTEXPR_ASSERT(chars != nullptr);
        return find_last_not_of(chars, pos, traits_type::length(chars));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_last_not_of(
        value_type ch, size_type pos = npos) const UTL_THROWS {
        return find_last_not_of(&ch, pos, 1);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr size_type find_last_not_of(
        view_type view, size_type pos = npos) const UTL_THROWS {
        return find_last_not_of(view.data(), pos, view.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    UTL_ATTRIBUTES(NODISCARD, _ABI_PRIVATE) inline constexpr size_type find_last_not_of(View const& view,
        size_type pos = npos) const UTL_NOEXCEPT(UTL_TRAIT_is_nothrow_convertible(View,
        view_type)) {
        return find_last_not_of(view_type(view), pos);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr int compare(basic_short_string const& other) const noexcept {
        return details::string::compare<traits_type>(data(), size(), other.data(), other.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr int compare(value_type const* str) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return details::string::compare<traits_type>(data(), size(), str, traits_type::length(str));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr int compare(view_type view) const noexcept {
        return details::string::compare<traits_type>(data(), size(), view.data(), view.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_convertible(View, view_type))>
    UTL_ATTRIBUTES(NODISCARD, _ABI_PRIVATE) inline UTL_CONSTEXPR_CXX14 int compare(View const& view) const
        noexcept(UTL_TRAIT_is_nothrow_convertible(View, view_type)) {
        return compare(view_type(view));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline UTL_CONSTEXPR_CXX14 int compare(
        size_type pos, size_type count, basic_short_string const& other) const UTL_THROWS {
        return compare(pos, count, other.data(), other.size());
    }

    UTL_ATTRIBUTE(STRING_PURE) inline UTL_CONSTEXPR_CXX14 int compare(
        size_type pos, size_type count, value_type const* str, size_type str_len) const UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        UTL_THROW_IF(pos > size(),
            out_of_range(UTL_MESSAGE_FORMAT("[UTL] `basic_short_string::replace` operation failed, "
                                            "Reason=[index out of range], pos=[%zu], size=[%zu]"),
                pos, size()));
        return details::string::compare<traits_type>(
            data() + pos, __UTL numeric::min(count, size() - pos), str, str_len);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline UTL_CONSTEXPR_CXX14 int compare(
        size_type pos, size_type count, value_type const* str) const UTL_THROWS {
        return compare(pos, count, str, traits_type::length(str));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline UTL_CONSTEXPR_CXX14 int compare(
        size_type pos, size_type count, view_type view) const UTL_THROWS {
        return compare(pos, count, view.data(), view.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 int compare(size_type pos, size_type count,
        View const& view) const UTL_NOEXCEPT(UTL_TRAIT_is_nothrow_convertible(View, view_type)) {
        return compare(pos, count, view_type(view));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline UTL_CONSTEXPR_CXX14 int compare(size_type pos, size_type count,
        basic_short_string const& other, size_type pos2, size_type count2 = npos) const UTL_THROWS {
        UTL_THROW_IF(pos2 > other.size(),
            out_of_range(
                UTL_MESSAGE_FORMAT("[UTL] `basic_short_string::compare` operation failed, "
                                   "Reason=[argument index out of range], pos=[%zu], size=[%zu]"),
                pos2, other.size()));
        return compare(pos, count, view_type(other).substr(pos2, count2));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline UTL_CONSTEXPR_CXX14 int compare(size_type pos, size_type count, view_type view,
        size_type pos2, size_type count2 = npos) const UTL_THROWS {
        UTL_THROW_IF(pos2 > view.size(),
            out_of_range(
                UTL_MESSAGE_FORMAT("[UTL] `basic_short_string::compare` operation failed, "
                                   "Reason=[argument index out of range], pos=[%zu], size=[%zu]"),
                pos2, view.size()));
        return compare(pos, count, view.substr(pos2, count2));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, view_type))>
    UTL_ATTRIBUTES(NODISCARD, PURE, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 int compare(size_type pos, size_type count,
        View const& view, size_type pos2, size_type count2 = npos) const UTL_NOEXCEPT(UTL_TRAIT_is_nothrow_convertible(View,
        view_type)) {
        return compare(pos, count, view_type(view), pos2, count2);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool starts_with(view_type view) const noexcept {
        return size() >= view.size() && traits_type::compare(data(), view.data(), view.size()) == 0;
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool starts_with(value_type ch) const noexcept {
        return !empty() && front() == ch;
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool starts_with(value_type const* str) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return starts_with(view_type(str));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool ends_with(view_type view) const noexcept {
        return size() >= view.size() &&
            traits_type::compare(view_type(*this).substr(size() - view.size()).data(), view.data(),
                view.size()) == 0;
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool ends_with(value_type ch) const noexcept {
        return !empty() && back() == ch;
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool ends_with(value_type const* str) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return ends_with(view_type(str));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool contains(view_type view) const noexcept {
        return find(view) != npos;
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool contains(value_type ch) const noexcept {
        return traits_type::find(data(), size(), ch) != nullptr;
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool contains(value_type const* str) const noexcept {
        // CONSTEXPR_ASSERT(str != nullptr);
        return contains(view_type(str));
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr basic_short_string substr(
        size_type pos = 0, size_type count = npos) const& {
        return basic_short_string(*this, pos, count);
    }

    UTL_ATTRIBUTE(STRING_PURE) inline constexpr basic_short_string substr(
        size_type pos = 0, size_type count = npos) && {
        return basic_short_string(__UTL move(*this), pos, count);
    }

private:
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(basic_short_string& other, false_type) noexcept {
        ranges::swap(get_union(), other.get_union());
        auto size_tmp = size_;
        auto heap_tmp = is_heap_;
        size_ = other.size_;
        is_heap_ = other.is_heap_;
        other.size_ = size_tmp;
        other.is_heap_ = heap_tmp;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(basic_short_string& other, true_type) noexcept {
        swap(other, false_type{});
        ranges::swap(allocator_ref(), other.allocator_ref());
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CONSTRUCTS_AT void transfer_to_heap(
        size_type new_capacity) UTL_THROWS {
        UTL_ASSERT(!is_heap_);
        auto const result = alloc_traits::allocate_at_least(allocator_ref(), new_capacity + 1);
        heap_type new_heap{__UTL move(result.ptr), result.size};
        traits_type::copy(__UTL to_address(new_heap.data_), data(), size());
        __UTL construct_at(__UTL addressof(get_heap()), new_heap);
        is_heap_ = true;
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX20 void grow_heap(
        size_type new_capacity) UTL_THROWS {
        UTL_ASSERT(is_heap_);
        get_heap() =
            alloc_traits::reallocate_at_least(allocator_ref(), get_heap(), new_capacity + 1);
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX20 void reserve_impl(
        size_type new_capacity) UTL_THROWS {
        UTL_ASSERT(new_capacity > this->capacity());
        UTL_TRY {
            if (!is_heap_) {
                transfer_to_heap(new_capacity);
            } else {
                grow_heap(new_capacity);
            }
        } UTL_CATCH(program_exception& exception) {
            exception.emplace_messagef(
                UTL_MESSAGE_FORMAT("[UTL] `basic_short_string::reserve` operation failed, "
                                   "Reason=[Allocation failed]"));
            UTL_RETHROW();
        }
    }

    __UTL_HIDE_FROM_ABI __UTL_STRING_INLINE UTL_CONSTEXPR_CXX14 void destroy() noexcept {
        if (is_heap_) {
            alloc_traits::deallocate(allocator_ref(), get_heap().data_, get_heap().capacity_);
        }
    }

    __UTL_HIDE_FROM_ABI static __UTL_STRING_INLINE UTL_CONSTEXPR_CXX20 heap_type clone_heap(
        allocator_type& alloc, basic_short_string const& src) UTL_THROWS {
        UTL_ASSERT(src.is_heap_);
        UTL_TRY {
            size_type const buffer_capacity = src.capacity() + 1;
            heap_type heap{alloc_traits::allocate(alloc, buffer_capacity), buffer_capacity};
            traits_type::copy(heap.data_, src.data(), src.size() + 1);
            return heap;
        } UTL_CATCH(program_exception& exception) {
            exception.emplace_messagef(UTL_MESSAGE_FORMAT(
                "[UTL] basic_short_string operation failed, Reason=[Allocation failed]"));
            UTL_RETHROW();
        }
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, REINITIALIZES) inline UTL_CONSTEXPR_CXX14 void reset_to_short() && noexcept {
        // The && qualifier is not needed but added to prevent accidental use
        // constexpr only if `is_heap_` is `false` because constexpr placement new is not portable
        if (is_heap_) {
            size_ = 0;
            is_heap_ = false;
            ::new (__UTL addressof(get_short())) short_type;
        }
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void move_assign(
        basic_short_string& other, false_type) UTL_THROWS {
        *this = other;
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void move_assign(
        basic_short_string& other, true_type) noexcept {
        destroy();
        get_union() = other.get_union();
        alloc_traits::assign(allocator_ref(), __UTL move(other.allocator_ref()));
        size_ = other.size_;
        is_heap_ = other.is_heap_;

        __UTL move(other).reset_to_short();
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void inline_substring(
        size_type pos, size_type count) noexcept {
        auto const new_size = __UTL numeric::min(this->size() - pos, count);
        traits_type::move(this->data(), this->data() + pos, new_size);
        this->resize(new_size);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void on_move_construct_with_alloc(
        basic_short_string& other, size_type pos, size_type count, true_type) UTL_THROWS {
        if (!alloc_traits::equals(allocator_ref(), other.allocator_ref()) && other.is_heap_) {
            storage_.get_heap() = clone_heap(allocator_ref(), other);

            inline_substring(pos, count);
        } else {
            on_move_construct_with_alloc(other, pos, count, false_type{});
        }
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void on_move_construct_with_alloc(
        basic_short_string& other, size_type pos, size_type count, false_type) noexcept {
        inline_substring(pos, count);
        __UTL move(other).reset_to_short();
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 data_union& get_union() noexcept {
        return storage_.first();
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 short_type& get_short() noexcept {
        return storage_.first().short_;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 heap_type& get_heap() noexcept {
        return storage_.first().heap_;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 allocator_type& allocator_ref() noexcept {
        return storage_.second();
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr data_union const&
    get_union() const noexcept {
        return storage_.first();
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr short_type const&
    get_short() const noexcept {
        return storage_.first().short_;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr heap_type const& get_heap() const noexcept {
        return storage_.first().heap_;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr allocator_type const&
    allocator_ref() const noexcept {
        return storage_.second();
    }

    storage_type storage_;
    /**
     * Size of the string, excluding the null-terminator
     */
    size_type size_ : sizeof(size_type) * CHAR_BIT - 1;
    /**
     * Whether the string data is allocated on the heap
     */
    size_type is_heap_ : 1;
};

template <typename CharT, size_t N, typename Traits, typename Alloc, typename U>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 typename basic_short_string<CharT, N, Traits, Alloc>::size_type
erase(basic_short_string<CharT, N, Traits, Alloc>& c, U const& value) {
    auto const it = __UTL remove(c.begin(), c.end(), value);
    auto const result = c.end() - it;
    c.erase(it, c.end());
    return result;
}

template <typename CharT, size_t N, typename Traits, typename Alloc, typename Pred>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 typename basic_short_string<CharT, N, Traits, Alloc>::size_type
erase_if(basic_short_string<CharT, N, Traits, Alloc>& c, Pred const& pred) {
    auto const it = __UTL remove_if(c.begin(), c.end(), pred);
    auto const result = c.end() - it;
    c.erase(it, c.end());
    return result;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(STRING_PURE) inline constexpr bool operator==(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs,
    basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(STRING_PURE) inline constexpr bool operator==(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs, CharT const* rhs) noexcept {
    return lhs.compare(rhs) == 0;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    basic_short_string<CharT, N, Traits, Alloc> const& l,
    basic_short_string<CharT, N, Traits, Alloc> const& r) UTL_THROWS {
    basic_short_string<CharT, N, Traits, Alloc> output;
    output.reserve(l.size() + r.size());
    output += l;
    output += r;
    return output;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    basic_short_string<CharT, N, Traits, Alloc> const& l, CharT const* r) UTL_THROWS {
    return l + basic_string_view<CharT, Traits>(r);
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    basic_short_string<CharT, N, Traits, Alloc> const& l, CharT r) UTL_THROWS {
    basic_short_string<CharT, N, Traits, Alloc> output;
    output.reserve(l.size() + 1);
    output += l;
    output += r;
    return output;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    basic_short_string<CharT, N, Traits, Alloc> const& l,
    type_identity_t<basic_string_view<CharT, Traits>> r) UTL_THROWS {
    basic_short_string<CharT, N, Traits, Alloc> output;
    output.reserve(l.size() + r.size());
    output += l;
    output += r;
    return output;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    CharT const* l, basic_short_string<CharT, N, Traits, Alloc> const& r) UTL_THROWS {
    return basic_string_view<CharT, Traits>(l) + r;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    CharT l, basic_short_string<CharT, N, Traits, Alloc> const& r) UTL_THROWS {
    basic_short_string<CharT, N, Traits, Alloc> output;
    output.reserve(l.size() + 1);
    output += l;
    output += r;
    return output;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    type_identity_t<basic_string_view<CharT, Traits>> l,
    basic_short_string<CharT, N, Traits, Alloc> const& r) UTL_THROWS {
    basic_short_string<CharT, N, Traits, Alloc> output;
    output.reserve(l.size() + r.size());
    output.append(l).append(r);
    return output;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    basic_short_string<CharT, N, Traits, Alloc>&& l,
    basic_short_string<CharT, N, Traits, Alloc>&& r) UTL_THROWS {
    return __UTL move(l.append(r));
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    basic_short_string<CharT, N, Traits, Alloc>&& l,
    basic_short_string<CharT, N, Traits, Alloc> const& r) UTL_THROWS {
    return __UTL move(l.append(r));
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    basic_short_string<CharT, N, Traits, Alloc>&& l, CharT const* r) UTL_THROWS {
    return __UTL move(l.append(r));
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    basic_short_string<CharT, N, Traits, Alloc>&& l, CharT r) UTL_THROWS {
    return __UTL move(l.append(r));
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    basic_short_string<CharT, N, Traits, Alloc>&& l,
    type_identity_t<basic_string_view<CharT, Traits>> r) UTL_THROWS {
    return __UTL move(l.append(r));
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    basic_short_string<CharT, N, Traits, Alloc> const& l,
    basic_short_string<CharT, N, Traits, Alloc>&& r) UTL_THROWS {
    return __UTL move(r.insert(0, l));
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    CharT const* l, basic_short_string<CharT, N, Traits, Alloc>&& r) UTL_THROWS {
    return __UTL move(r.insert(0, l));
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    CharT l, basic_short_string<CharT, N, Traits, Alloc>&& r) UTL_THROWS {
    return __UTL move(r.insert(0, l));
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTES(NODISCARD,_HIDE_FROM_ABI) inline constexpr basic_short_string<CharT, N, Traits, Alloc> operator+(
    type_identity_t<basic_string_view<CharT, Traits>> l,
    basic_short_string<CharT, N, Traits, Alloc>&& r) UTL_THROWS {
    return __UTL move(r.insert(0, l));
}

UTL_NAMESPACE_END

#if UTL_CXX20

#  include "utl/compare/utl_compare_fwd.h"

UTL_NAMESPACE_BEGIN

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr typename Traits::comparison_category operator<=>(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs,
    basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    using result_type = typename Traits::comparison_category;
    return static_cast<result_type>(lhs.compare(rhs) <=> 0);
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr typename Traits::comparison_category operator<=>(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs, CharT const* rhs) noexcept {
    using result_type = typename Traits::comparison_category;
    return static_cast<result_type>(lhs.compare(rhs) <=> 0);
}

UTL_NAMESPACE_END

#else

UTL_NAMESPACE_BEGIN

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator<(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs,
    basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return lhs.compare(rhs) < 0;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator<(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs, CharT const* rhs) noexcept {
    return lhs.compare(rhs) < 0;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator<(
    CharT const* lhs, basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return rhs.compare(lhs) > 0;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator>(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs,
    basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return rhs < lhs;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator>(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs, CharT const* rhs) noexcept {
    return rhs < lhs;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator>(
    CharT const* lhs, basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return rhs < lhs;
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator>=(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs,
    basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return !(lhs < rhs);
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator>=(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs, CharT const* rhs) noexcept {
    return !(lhs < rhs);
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator>=(
    CharT const* lhs, basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return !(lhs < rhs);
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator<=(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs,
    basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return !(rhs < lhs);
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator<=(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs, CharT const* rhs) noexcept {
    return !(rhs < lhs);
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator<=(
    CharT const* lhs, basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return !(rhs < lhs);
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator!=(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs,
    basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return !(lhs == rhs);
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator!=(
    basic_short_string<CharT, N, Traits, Alloc> const& lhs, CharT const* rhs) noexcept {
    return !(lhs == rhs);
}

template <typename CharT, size_t N, typename Traits, typename Alloc>
UTL_ATTRIBUTE(STRING_PURE) inline constexpr bool operator!=(
    CharT const* lhs, basic_short_string<CharT, N, Traits, Alloc> const& rhs) noexcept {
    return !(lhs == rhs);
}

UTL_NAMESPACE_END

#endif

#undef __UTL_ATTRIBUTE_STRING_PURE
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_STRING_PURE
#undef __UTL_ATTRIBUTE_STRING_CONST
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_STRING_CONST

#if !UTL_CXX17

UTL_NAMESPACE_BEGIN

template <typename CharT, size_t N, typename Traits, typename Alloc>
__UTL_ABI_PUBLIC constexpr typename basic_short_string<CharT, N, Traits, Alloc>::size_type
    basic_short_string<CharT, N, Traits, Alloc>::npos;

UTL_NAMESPACE_END

#endif
