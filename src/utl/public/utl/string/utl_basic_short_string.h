// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts.h" // integral, convertible_to
#include "utl/exception.h"
#include "utl/iterator/utl_contiguous_iterator_base.h"
#include "utl/iterator/utl_legacy_forward_iterator.h"
#include "utl/iterator/utl_legacy_input_iterator.h"
#include "utl/memory/utl_allocator_traits.h"
#include "utl/preprocessor/utl_config.h"
// #include "utl/string/utl_string_details.h"
#include "utl/string/utl_string_fwd.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/utility/utl_as_const.h"
#include "utl/utility/utl_compressed_pair.h"

#include <limits.h>

#include <initializer_list>
#include <iterator>

#if UTL_WITH_EXCEPTIONS
#  define UTL_CONSTEXPR_WITH_TRY UTL_CONSTEXPR_CXX20
#else
#  define UTL_CONSTEXPR_WITH_TRY UTL_CONSTEXPR_CXX14
#endif

#define UTL_STRING_PURE UTL_ATTRIBUTES(NODISCARD, PURE)
#define UTL_STRING_CONST UTL_ATTRIBUTES(NODISCARD, CONST)

UTL_NAMESPACE_BEGIN
template <typename CharType, size_t ShortSize, typename Traits, typename Alloc>
class basic_short_string {
public:
    using allocator_type = Alloc;
    using value_type = CharType;
    using size_type = size_t;
    using difference_type = decltype((CharType*)0 - (CharType*)0);
    using traits_type = Traits;
    using pointer = CharType*;
    using const_pointer = CharType const*;
    using reference = CharType&;
    using const_reference = CharType const&;
    using alloc_traits = allocator_traits<allocator_type>;
    using view_type = basic_string_view<value_type, traits_type>;
    static constexpr size_type npos = (size_type)-1;
    class iterator;
    class const_iterator;

private:
    UTL_STRING_CONST
    static constexpr size_type min(size_type x, size_type y) noexcept { return x < y ? x : y; }
    UTL_STRING_CONST
    static constexpr size_type max(size_type x, size_type y) noexcept { return x > y ? x : y; }
    struct alignas(16) short_type {
        value_type data_[max(ShortSize, 15) + 1];
    };

    struct heap_type {
        pointer data_;
        /**
         * Capacity includes null-chacacter
         */
        size_type capacity_;

        constexpr heap_type(heap_type const&) noexcept = default;
        UTL_CONSTEXPR_CXX14 heap_type& operator=(heap_type const&) noexcept = default;

        constexpr heap_type(pointer data, size_type capacity) noexcept
            : data_(data)
            , capacity_(capacity) {}

        UTL_CONSTEXPR_CXX14 heap_type& operator=(
            allocation_result<pointer, size_type> const& other) noexcept {
            data_ = other.ptr;
            capacity_ = other.size;
        }
        constexpr operator allocation_result<pointer, size_type>() const noexcept {
            return {data_, capacity_};
        }
    };

    union storage_union {
        short_type short_;
        heap_type heap_;
    };

    using storage_type = compressed_pair<storage_union, allocator_type>;

public:
    class iterator : UTL_SCOPE contiguous_iterator_base<iterator, value_type> {
        using base_type = contiguous_iterator_base<iterator, value_type>;

    public:
        using value_type = base_type::value_type;
        using pointer = base_type::pointer;
        using reference = base_type::reference;
        using difference_type = base_type::difference_type;
        using iterator_concept = base_type::iterator_concept;

        constexpr iterator() noexcept = default;
        constexpr iterator(iterator const& other) noexcept = default;
        constexpr iterator(iterator&& other) noexcept = default;
        constexpr iterator& operator=(iterator const& other) noexcept = default;
        constexpr iterator& operator=(iterator&& other) noexcept = default;
        using base_type::operator*;
        using base_type::operator->;

    private:
        friend basic_short_string;
        constexpr iterator(pointer data) noexcept : contiguous_iterator_base(data) {}
        constexpr iterator(contiguous_iterator_base<iterator, value_type const> other) noexcept
            : iterator(const_cast<pointer>(other.operator->())) {}
    };

    class const_iterator : UTL_SCOPE contiguous_iterator_base<iterator, value_type const> {
        using base_type = contiguous_iterator_base<iterator, value_type>;

    public:
        using value_type = base_type::value_type;
        using pointer = base_type::pointer;
        using reference = base_type::reference;
        using difference_type = base_type::difference_type;
        using iterator_concept = base_type::iterator_concept;

        constexpr const_iterator() noexcept = default;
        constexpr const_iterator(const_iterator const& other) noexcept = default;
        constexpr const_iterator(const_iterator&& other) noexcept = default;
        constexpr const_iterator& operator=(const_iterator const& other) noexcept = default;
        constexpr const_iterator& operator=(const_iterator&& other) noexcept = default;
        using base_type::operator*;
        using base_type::operator->;

    private:
        friend basic_short_string;
        constexpr const_iterator(pointer data) noexcept : base_type(data) {}
        constexpr const_iterator(
            contiguous_iterator_base<iterator, basic_short_string::value_type> other) noexcept
            : base_type(other) {}
    };

    using reverse_iterator = ::std::reverse_iterator<iterator>;
    using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

    basic_short_string(decltype(nullptr)) = delete;
    constexpr basic_short_string() noexcept(noexcept(allocator_type()))
        : basic_short_string(allocator_type()) {}

    explicit constexpr basic_short_string(allocator_type const& a) noexcept
        : storage_(details::compressed_pair::default_initialize, a)
        , size_()
        , is_heap_() {}

    UTL_CONSTEXPR_CXX14 basic_short_string(
        const_pointer str, size_type len, allocator_type const& a = allocator_type()) UTL_THROWS
        : basic_short_string(a) {
        assign(str, len);
    }

    template <UTL_CONCEPT_CXX20(UTL_SCOPE legacy_input_iterator) It UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_legacy_input_iterator(It))>
    UTL_CONSTEXPR_CXX14 basic_short_string(
        It first, It last, allocator_type const& a = allocator_type()) UTL_THROWS
        : basic_short_string(a) {
        assign(first, last);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string(
        const_pointer str, allocator_type const& a = allocator_type()) UTL_THROWS
        : basic_short_string(str, traits_type::length(str), a) {}

    UTL_CONSTEXPR_CXX14 basic_short_string(::std::initializer_list<value_type> ilist,
        allocator_type const& a = allocator_type()) UTL_THROWS
        : basic_short_string(ilist.begin(), ilist.size(), a) {}

    UTL_CONSTEXPR_CXX14 basic_short_string(
        size_type count, value_type ch, allocator_type const& a = allocator_type()) UTL_THROWS
        : basic_short_string(a) {
        resize(count, ch);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string(View const& view,
        allocator_type const& a = allocator_type()) noexcept(!utl::with_exceptions &&
        is_nothrow_convertible<View, view_type>::value)
        : basic_short_string(a) {
        assign(view);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string(View const& view, size_type pos, size_type n,
        allocator_type const& a = allocator_type()) noexcept(!utl::with_exceptions &&
        is_nothrow_convertible<View, view_type>::value)
        : basic_short_string(a) {
        assign(view, pos, n);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string(basic_short_string const& other) UTL_THROWS
        : storage_(other.storage_.first(),
              alloc_traits::select_on_container_copy_construction(other.allocator_ref()))
        , size_(other.size_)
        , is_heap_(other.is_heap_) {

        if (other.is_heap_) {
            storage_.get_heap() = clone_heap(allocator_ref(), other);
        }
    }

    UTL_CONSTEXPR_CXX14 basic_short_string(basic_short_string const& other, size_type pos, size_type count,
        allocator_type const& alloc = allocator_type()) UTL_THROWS
        : storage_(other.storage_.first(), alloc)
        , size_(other.size_)
        , is_heap_(other.is_heap_) {
        UTL_THROW(pos > other.size(),
            program_exception<void>("[UTL] basic_short_string construction failed, "
                                    "Reason=[index out of range]"));

        if (other.is_heap_) {
            storage_.get_heap() = clone_heap(allocator_ref(), other);
        }
        auto const new_size = min(other.size() - pos, count);
        traits_type::move(this->data(), this->data() + pos, new_size);
        this->resize(new_size);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string(basic_short_string const& other, size_type pos,
        allocator_type const& alloc = allocator_type()) UTL_THROWS
        : basic_short_string(other, pos, other.size(), alloc) {}

    UTL_CONSTEXPR_CXX14 basic_short_string& operator=(basic_short_string const& other) UTL_THROWS {
        if (this != addressof(other)) {
            destroy();
            storage_.first() = other.storage_.first();
            alloc_traits::assign(allocator_ref(), other.allocator_ref());
            size_ = other.size_;
            is_heap_ = other.is_heap_;

            if (other.is_heap_) {
                storage_.get_heap() = clone_heap(allocator_ref(), other);
            }
        }

        return *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string(basic_short_string&& other) noexcept
        : storage_(other.storage_)
        , size_(other.size_)
        , is_heap_(other.is_heap_) {
        other.reset_to_short();
    }

    UTL_CONSTEXPR_CXX14 basic_short_string(basic_short_string&& other, size_type pos, size_type count,
        allocator_type const& alloc = allocator_type()) UTL_THROWS
        : storage_(other.storage_)
        , size_(other.size_)
        , is_heap_(other.is_heap_) {
        UTL_THROW(pos > other.size(),
            program_exception<void>("[UTL] basic_short_string construction failed, "
                                    "Reason=[index out of range]"));
        auto const new_size = min(other.size() - pos, count);
        traits_type::move(this->data(), this->data() + pos, new_size);
        this->resize(new_size);

        other.reset_to_short();
    }

    UTL_CONSTEXPR_CXX14 basic_short_string(basic_short_string&& other, size_type pos,
        allocator_type const& alloc = allocator_type()) UTL_THROWS
        : basic_short_string(move(other), pos, other.size(), alloc) {}

    UTL_CONSTEXPR_CXX20 ~basic_short_string() noexcept { destroy(); }

    UTL_CONSTEXPR_CXX14 basic_short_string& operator=(basic_short_string&& other) noexcept(
        (alloc_traits::propagate_on_container_move_assignment::value &&
            alloc_traits::is_always_equal::value) ||
        !utl::with_exceptions) {

        static constexpr bool can_move =
            alloc_traits::propagate_on_container_move_assignment::value &&
            alloc_traits::is_always_equal::value;

        move_assign(other, bool_constant<can_move>());
        return *this;
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 pointer data() noexcept {
        return !is_heap_ ? get_short().data_ : get_heap().data_;
    }

    UTL_STRING_PURE constexpr const_pointer data() const noexcept {
        return !is_heap_ ? get_short().data_ : get_heap().data_;
    }

    UTL_STRING_PURE constexpr const_pointer c_str() const noexcept { return data(); }

    UTL_STRING_PURE constexpr bool empty() const noexcept { return !size_; }
    UTL_STRING_PURE constexpr size_type size() const noexcept { return size_; }
    UTL_STRING_PURE constexpr size_type length() const noexcept { return size_; }

    UTL_STRING_CONST constexpr size_type max_size() const noexcept {
        constexpr size_type max_bytes = ~(size_type(1) << (sizeof(size_type) * CHAR_BIT - 1));
        constexpr size_type max_characters = max_bytes / sizeof(value_type);
        return max_characters - 1;
    }

    UTL_STRING_PURE constexpr size_type capacity() const noexcept {
        constexpr size_type short_capacity = sizeof(short_type::data_) / sizeof(value_type);
        size_type const buffer_capacity = !is_heap_ ? short_capacity : get_heap().capacity_;
        return buffer_capacity - 1;
    }

    UTL_STRING_PURE
    constexpr allocator_type get_allocator() const noexcept { return allocator_ref(); }

    UTL_CONSTEXPR_CXX14 void reserve(size_type new_capacity) UTL_THROWS {
        if (new_capacity <= this->capacity()) {
            return;
        }

        UTL_THROW_IF(new_capacity > max_size(),
            program_exception<void>("[UTL] basic_short_string::reserve operation failed, "
                                    "Reason=[Requested capacity exceeds maximum size]"));
        reserve_impl(new_capacity);
    }

    UTL_CONSTEXPR_CXX14 void resize(size_type new_size) UTL_THROWS { resize(new_size, value_type()); }

    UTL_CONSTEXPR_CXX14 void resize(size_type new_size, value_type ch) UTL_THROWS {
        reserve(new_size);
        traits_type::assign(data() + size(), max(new_size, size()) - size(), ch);
        data()[new_size] = value_type();
        size_ = new_size;
    }

    template <typename Op UTL_REQUIRES_CXX11(
        is_integral < decltype(UTL_SCOPE declval<Op>()(pointer{}, size_type{})) > ::value)>
    UTL_REQUIRES_CXX20(requires(Op op, pointer p, size_type s) {
        { op(p, s) } -> integral;
    })
    UTL_CONSTEXPR_WITH_TRY void resize_and_overwrite(size_type new_size, Op operation) UTL_THROWS {
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

    UTL_CONSTEXPR_CXX14 void shrink_to_fit() UTL_THROWS {
        if (is_heap_ && size() < capacity()) {
            get_heap() = alloc_traits::reallocate(allocator_, get_heap(), size() + 1);
        }
    }

    UTL_ATTRIBUTE(REINITIALIZE) UTL_CONSTEXPR_CXX14 void clear() noexcept {
        *data() = 0;
        size_ = 0;
    }

    UTL_STRING_PURE constexpr iterator begin() noexcept { return iterator(data()); }
    UTL_STRING_PURE constexpr const_iterator cbegin() const noexcept {
        return const_iterator(data());
    }
    UTL_STRING_PURE constexpr const_iterator begin() const noexcept {
        return const_iterator(data());
    }
    UTL_STRING_PURE constexpr iterator end() noexcept { return iterator(data() + size()); }
    UTL_STRING_PURE constexpr const_iterator cend() const noexcept {
        return const_iterator(data() + size());
    }
    UTL_STRING_PURE constexpr const_iterator end() const noexcept {
        return const_iterator(data() + size());
    }
    UTL_STRING_PURE constexpr iterator rbegin() noexcept { return reverse_iterator(end()); }
    UTL_STRING_PURE constexpr const_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    UTL_STRING_PURE constexpr const_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    UTL_STRING_PURE constexpr iterator rend() noexcept { return reverse_iterator(begin()); }
    UTL_STRING_PURE constexpr const_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }
    UTL_STRING_PURE constexpr const_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    UTL_STRING_PURE constexpr const_reference front() const noexcept { return *data(); }
    UTL_STRING_PURE constexpr reference front() noexcept { return *data(); }
    UTL_STRING_PURE constexpr const_reference back() const noexcept { return data()[size() - 1]; }
    UTL_STRING_PURE constexpr reference back() noexcept { return return data()[size() - 1]; }
    UTL_STRING_PURE constexpr reference operator[](size_type idx) noexcept { return data()[idx]; }
    UTL_STRING_PURE constexpr const_reference operator[](size_type idx) const noexcept {
        return data()[idx];
    }
    UTL_STRING_PURE constexpr reference at(size_type idx) UTL_THROWS {
        return const_cast<reference>(as_const(*this).data()[idx]);
    }
    UTL_STRING_PURE constexpr const_reference at(size_type idx) const UTL_THROWS {
        UTL_THROW_IF(idx >= size(),
            program_exception<void>("[UTL] basic_short_string::at operation failed, "
                                    "Reason=[index out of range]"));
        return data()[idx];
    }

    UTL_CONSTEXPR_CXX14 void push_back(value_type ch) UTL_THROWS {
        reserve(size() + 1);
        data()[size_++] = ch;
        data()[size_] = value_type();
    }

    UTL_CONSTEXPR_CXX14 void pop_back() noexcept { data()[--size_] = value_type(); }

    basic_short_string& assign(decltype(nullptr)) = delete;

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(size_type count, value_type ch) UTL_THROWS {
        UTL_ATTRIBUTE(MAYBE_UNUSED) size_type const old_size = exchange(size_, 0);
        UTL_TRY {
            resize(count, ch);
            return *this;
        } UTL_CATCH(...) {
            size_ = old_size;
            UTL_RETRHOW;
        }
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(basic_short_string const& other) UTL_THROWS {
        return *this = other;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        basic_short_string const& other, size_type pos, size_type count = npos) UTL_THROWS {
        if (this == addressof(other)) {
            return *this;
        }

        UTL_THROW_IF(pos > other.size(),
            program_exception<void>("[UTL] basic_short_string::assign operation failed, "
                                    "Reason=[substring argument index is out of range]"));
        count = count > other.size() ? other.size() : count;
        resize(count);
        traits_type::copy(data(), other.data() + pos, count + 1);
        return *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(basic_short_string&& other) UTL_THROWS {
        return *this = move(other);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(const_pointer str, size_type count) UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        reserve(count);
        traits_type::copy(data(), str, count)[count] = value_type();
        size_ = count;
        return *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(const_pointer str) UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        assign(str, traits_type::length(str));
    }

    template <UTL_CONCEPT_CXX20(UTL_SCOPE legacy_input_iterator) It UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_legacy_input_iterator(It) && !UTL_TRAIT_is_legacy_forward_iterator(It))>
    UTL_CONSTEXPR_CXX14 basic_short_string& assign(It begin, It end) UTL_THROWS {
        size_ = 0;
        while (begin != end) {
            push_back(*begin);
            ++begin;
        }
    }

    template <UTL_CONCEPT_CXX20(UTL_SCOPE legacy_forward_iterator) It UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_legacy_forward_iterator(It))>
    UTL_CONSTEXPR_CXX17 basic_short_string& assign(It begin, It end) UTL_THROWS {
        auto const diff = ::std::distance(begin, end);
        if (diff < 0)
            UTL_ATTRIBUTE(UNLIKELY) {
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

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(::std::initializer_list<value_type> list)
        UTL_THROWS {
        assign(list.begin(), list.end());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& assign(View const& view) UTL_THROWS {
        view_type const v(view);
        return assign(v.data(), v.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        View const& view, size_type subidx, size_type subcount = npos) UTL_THROWS {
        view_type const v(view);
        UTL_THROW_IF(pos > v.size(),
            program_exception<void>("[UTL] `basic_short_string::assign` operation failed, "
                                    "Reason=[substring argument index is out of range]"));
        return assign(v.substr(subidx, subcount));
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& insert(size_type pos, size_type count, value_type ch)
        UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::insert` operation failed, "
                                    "Reason=[index out of range]"));
        reserve(size() + count);
        auto const src = data() + pos;
        auto const dst = src + count;
        traits_type::move(dst, src, size() + 1 - pos);
        traits_type::assign(src, count, ch);
        size_ = size() + count;
        return begin() + pos;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& insert(
        size_type pos, const_pointer str, size_type length) UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] basic_short_string::insert operation failed, "
                                    "Reason=[index out of range]"));
        reserve(size() + length);
        auto const src = data() + pos;
        auto const dst = src + length;
        traits_type::move(dst, src, size() + 1 - pos);
        traits_type::move(src, str, length);
        size_ = size() + length;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& insert(size_type pos, const_pointer str) UTL_THROWS {
        return insert(pos, str, traits_type::length(str));
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& insert(size_type pos, basic_short_string const& str)
        UTL_THROWS {
        return insert(pos, str.data(), str.size());
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& insert(size_type pos, basic_short_string const& str,
        size_type idx, size_type count = npos) UTL_THROWS {
        UTL_THROW_IF(idx > str.size(),
            program_exception<void>("[UTL] `basic_short_string::insert` operation failed, "
                                    "Reason=[argument substring index out of range]"));
        auto const copy_size = min(str.size() - idx, count);
        return insert(pos, str.data() + idx, copy_size);
    }

    UTL_CONSTEXPR_CXX14 iterator insert(iterator pos, size_type count, value_type ch) UTL_THROWS {
        return insert(pos - begin(), count, ch);
    }

    UTL_CONSTEXPR_CXX14 iterator insert(const_iterator pos, size_type count, value_type ch)
        UTL_THROWS {
        return insert(pos - cbegin(), count, ch);
    }

    UTL_CONSTEXPR_CXX14 iterator insert(iterator pos, value_type ch) UTL_THROWS { return insert(pos, 1, ch); }

    UTL_CONSTEXPR_CXX14 iterator insert(const_iterator pos, value_type ch) UTL_THROWS {
        return insert(pos, 1, ch);
    }

    template <UTL_CONCEPT_CXX20(UTL_SCOPE legacy_input_iterator) It UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_legacy_input_iterator(It) && !UTL_TRAIT_is_legacy_forward_iterator(It))>
    UTL_CONSTEXPR_CXX14 iterator insert(const_iterator pos, It first, It last) UTL_THROWS {
        while (first != last) {
            insert(pos, *first);
            ++first;
        }
    }

    template <UTL_CONCEPT_CXX20(UTL_SCOPE legacy_forward_iterator) It UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_legacy_forward_iterator(It))>
    UTL_CONSTEXPR_CXX14 iterator insert(const_iterator pos, It first, It last) UTL_THROWS {
        auto const length = ::std::distance(first, last);
        if (length < 0)
            UTL_ATTRIBUTE(UNLIKELY) {
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
    }

    UTL_CONSTEXPR_CXX14 iterator insert(
        const_iterator pos, ::std::initializer_list<value_type> list) UTL_THROWS {
        return insert(pos, list.begin(), list.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& insert(size_type pos, View const& view) UTL_THROWS {
        view_type const v(view);
        return insert(pos, v.data(), v.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& insert(
        size_type pos, View const& view, size_type subidx, size_type subcount = npos) UTL_THROWS {
        view_type const v(view);
        UTL_THROW_IF(idx > v.size(),
            program_exception<void>("[UTL] `basic_short_string::insert` operation failed, "
                                    "Reason=[argument substring index is out of range]"));
        return insert(pos, v.substr(subidx, subcount));
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& erase(size_type idx = 0, size_type count = npos)
        UTL_THROWS {
        UTL_THROW_IF(idx > size(),
            program_exception<void>("[UTL] `basic_short_string::erase` operation failed, "
                                    "Reason=[index out of range]"));
        auto const length = min(size() - idx, count);
        auto const first = begin() + idx;
        return erase(first, first + length), *this;
    }

    UTL_CONSTEXPR_CXX14 iterator erase(iterator pos) noexcept { return erase(pos, pos + 1); }

    UTL_CONSTEXPR_CXX14 iterator erase(const_iterator pos) noexcept { return erase(pos, pos + 1); }

    UTL_CONSTEXPR_CXX14 iterator erase(const_iterator first, const_iterator last) noexcept {
        return erase(iterator(first), iterator(last));
    }

    UTL_CONSTEXPR_CXX14 iterator erase(iterator first, iterator last) noexcept {
        auto const length = last - first;
        auto const src = last.operator->();
        auto const dst = first.operator->();
        auto const move_length = (end() - last) + (length > 0);
        traits_type::move(dst, src, move_length);
        size_ = size() - length;
        return iterator(dst + length);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& append(size_type count, value_type ch) UTL_THROWS {
        return insert(end(), count, ch), *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& append(basic_short_string const& str) UTL_THROWS {
        return insert(end(), str), *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& append(
        basic_short_string const& str, size_type idx, size_type count = npos) UTL_THROWS {
        UTL_THROW_IF(idx > str.size(),
            program_exception<void>("[UTL] `basic_short_string::append` operation failed, "
                                    "Reason=[argument substring index out of range]"));
        return insert(end(), str, idx, count), *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& append(const_pointer str) UTL_THROWS {
        return insert(end(), str), *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& append(const_pointer str, size_type count) UTL_THROWS {
        return insert(end(), str, count), *this;
    }

    template <UTL_CONCEPT_CXX20(UTL_SCOPE legacy_input_iterator) It UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_legacy_input_iterator(It))>
    UTL_CONSTEXPR_CXX14 basic_short_string& append(It first, It last) UTL_THROWS {
        return insert(end(), first, last), *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& append(::std::initializer_list<value_type> list)
        UTL_THROWS {
        return insert(end(), list), *this;
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& append(View const& view) UTL_THROWS {
        return insert(end(), view), *this;
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& append(
        View const& view, size_type subidx, size_type subcount = npos) UTL_THROWS {
        view_type const v(view);
        UTL_THROW_IF(subidx > v.size(),
            program_exception<void>("[UTL] `basic_short_string::append` operation failed, "
                                    "Reason=[argument substring index out of range]"));
        return insert(end(), v.substr(subidx, subcount));
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& operator+=(basic_short_string const& str) UTL_THROWS {
        return append(str);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& operator+=(value_type ch) UTL_THROWS { return append(1, ch); }

    UTL_CONSTEXPR_CXX14 basic_short_string& operator+=(const_pointer str) UTL_THROWS { return append(str); }

    UTL_CONSTEXPR_CXX14 basic_short_string& operator+=(::std::initializer_list<value_type> list)
        UTL_THROWS {
        return append(list);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& operator+=(View const& view) UTL_THROWS {
        return append(view);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        size_type pos, size_type count, basic_short_string const& str) UTL_THROWS {
        return replace(pos, count, str.data(), str.size());
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, basic_short_string const& str) UTL_THROWS {
        return replace(first, last, str.data(), str.size());
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& replace(size_type pos, size_type count,
        basic_short_string const& str, size_type subidx, size_type subcount = npos) UTL_THROWS {
        UTL_THROW_IF(subidx > str.size(),
            program_exception<void>("[UTL] `basic_short_string::replace` operation failed, "
                                    "Reason=[argument substring index out of range]"));
        view_type const view(str).substr(subidx, subcount);
        return replace(pos, count, view.data(), view.size());
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        size_type pos, size_type count, const_pointer str, size_type length) UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::replace` operation failed, "
                                    "Reason=[index out of range]"));
        auto const first = begin() + pos;
        return replace(first, first + count, str, length);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, const_pointer str, size_type length) UTL_THROWS {
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

    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        size_type pos, size_type count, const_pointer str) UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        return replace(pos, count, str, traits_type::length(str));
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, const_pointer str) UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        return replace(first, last, str, traits_type::length(str));
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        size_type pos, size_type count, size_type char_count, value_type ch) UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::replace` operation failed, "
                                    "Reason=[index out of range]"));
        auto const first = cbegin() + pos;
        return replace(first, first + count, char_count, ch);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, size_type char_count, value_type ch) UTL_THROWS {
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

    template <UTL_CONCEPT_CXX20(UTL_SCOPE legacy_input_iterator) It UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_legacy_input_iterator(It))>
    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, It in_first, It in_last) UTL_THROWS {
        while (first != last && in_first != in_last) {
            const_cast<reference>(*first) = *in_first;
            ++first;
            ++in_last;
        }

        erase(first, last);
        insert(last, in_first, in_last);
        return *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& replace(const_iterator first, const_iterator last,
        ::std::initializer_list<value_type> list) UTL_THROWS {
        return replace(first, last, list.begin(), list.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        const_iterator first, const_iterator last, View const& view) UTL_THROWS {
        view_type const v(view);
        return replace(first, last, v.data(), v.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& replace(const_iterator first, const_iterator last,
        View const& view, size_type subidx, size_type subcount = npos) UTL_THROWS {
        view_type const v(view);
        UTL_THROW_IF(subidx > v.size(),
            program_exception<void>("[UTL] `basic_short_string::replace` operation failed, "
                                    "Reason=[argument substring index out of range]"));
        return replace(first, last, v.substr(subidx, subcount));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& replace(
        size_type idx, size_type count, View const& view) UTL_THROWS {
        auto const first = cbegin() + idx;
        return replace(first, first + count, view);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(
        is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& replace(size_type idx, size_type count, View const& view,
        size_type subidx, size_type subcount = npos) UTL_THROWS {
        UTL_THROW_IF(subidx > view.size(),
            program_exception<void>("[UTL] `basic_short_string::replace` operation failed, "
                                    "Reason=[argument substring index out of range]"));
        return replace(idx, count, view_type(view).substr(subidx, subcount));
    }

    UTL_CONSTEXPR_CXX14 size_type copy(pointer dst, size_type count, size_type pos = 0) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::copy` operation failed, "
                                    "Reason=[index out of range]"));

        auto const copied = min(count, size() - pos);
        traits_type::copy(dst, data(), copied);
        return copied;
    }

    UTL_CONSTEXPR_CXX14 void swap(basic_short_string& other) noexcept(
        alloc_traits::propagate_on_container_swap::value || alloc_traits::is_always_equal::value) {
        using swap_alloc_t = bool_constant<alloc_traits::propagate_on_container_swap::value ||
            !alloc_traits::is_always_equal::value>;
        swap(other, swap_alloc_t{});
    }

    friend void swap(basic_short_string& l, basic_short_string& r) noexcept(noexcept(l.swap(r))) {
        l.swap(r);
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find(
        basic_short_string const& str, size_type pos = 0) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::find` operation failed, "
                                    "Reason=[index out of range]"));
        return index(data(),
            UTL_SCOPE details::string::find<traits_type>(
                data(), size(), str.data(), str.size(), pos));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find(
        const_pointer str, size_type pos, size_type count) const UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::find` operation failed, "
                                    "Reason=[index out of range]"));
        return index(data(), details::string::find<traits_type>(data(), size(), str, count, pos));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find(const_pointer str, size_type pos = 0) const UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        return find(str, pos, traits_type::length(str));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find(value_type ch, size_type pos = 0) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::find` operation failed, "
                                    "Reason=[index out of range]"));
        view_type const v(*this).substr(pos);
        return index(data(), traits_type::find(v.data(), v.size(), ch));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 size_type find(View const& view, size_type pos = 0) const
        noexcept(!utl::with_exceptions && is_nothrow_convertible<View, view_type>::value) {
        view_type const v(view);
        return find(v.data(), pos, v.size());
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type rfind(
        basic_short_string const& str, size_type pos = npos) const noexcept {
        return index(data(),
            details::string::rfind<traits_type>(
                data(), size_clamp(last_to_size(pos)), str.data(), str.size()));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type rfind(
        const_pointer str, size_type pos, size_type count) const noexcept {
        return index(data(),
            details::string::rfind<traits_type>(data(), size_clamp(last_to_size(pos)), str, count));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type rfind(
        const_pointer str, size_type pos = npos) const noexcept {
        return index(data(),
            details::string::rfind<traits_type>(
                data(), size_clamp(last_to_size(pos)), str, Traits::length(str)));
    }

    UTL_STRING_PURE constexpr size_type rfind(value_type ch, size_type pos = npos) const noexcept {
        return index(data(), details::string::rfind(data(), size_clamp(last_to_size(pos)), ch));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 size_type rfind(View const& view, size_type pos = npos) const
        noexcept(is_nothrow_convertible<View, view_type>::value) {
        view_type const v(view);
        return rfind(v.data(), pos, v.size());
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_first_of(
        basic_short_string const& str, size_type pos = 0) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::find_first_of` operation failed, "
                                    "Reason=[index out of range]"));
        return index(data(),
            details::string::find_first_of<traits_type>(
                data() + pos, size() - pos, str.data(), str.size()));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_first_of(
        const_pointer str, size_type pos, size_type count) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::find_first_of` operation failed, "
                                    "Reason=[index out of range]"));
        return index(data(),
            details::string::find_first_of<traits_type>(data() + pos, size() - pos, str, count));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_first_of(const_pointer str, size_type pos = 0) const {
        UTL_ASSERT(str != nullptr);
        return find_first_of(str, pos, traits_type::length(str));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_first_of(value_type ch, size_type pos = 0) const {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::find_first_of` operation failed, "
                                    "Reason=[index out of range]"));
        return index(data(), traits_type::find(data() + pos, size() - pos, ch));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 size_type find_first_of(View const& view, size_type pos = 0) const
        noexcept(!utl::with_exceptions && is_nothrow_convertible<View, view_type>::value) {
        view_type const v(view);
        return find_first_of(v.data(), pos, v.size());
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_first_not_of(
        basic_short_string const& str, size_type pos = 0) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>(
                "[UTL] `basic_short_string::find_first_not_of` operation failed, "
                "Reason=[index out of range]"));
        return index(data(),
            details::string::find_first_not_of<traits_type>(
                data() + pos, size() - pos, str.data(), str.size()));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_first_not_of(
        const_pointer str, size_type pos, size_type count) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>(
                "[UTL] `basic_short_string::find_first_not_of` operation failed, "
                "Reason=[index out of range]"));
        return index(data(),
            details::string::find_first_not_of<traits_type>(
                data() + pos, size() - pos, str, count));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_first_not_of(
        const_pointer str, size_type pos = 0) const noexcept(!with_exceptions) {
        return find_first_not_of(str, pos, traits_type::length(str));
    }

    UTL_STRING_PURE
    UTL_CONSTEXPR_CXX14 size_type find_first_not_of(value_type ch, size_type pos = 0) const
        noexcept(!with_exceptions) {
        return find_first_not_of(&ch, pos, 1);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 size_type find_first_not_of(View const& view, size_type pos = 0) const
        noexcept(!utl::with_exceptions && is_nothrow_convertible<View, view_type>::value) {
        view_type const v(view);
        return find_first_not_of(v.data(), pos, v.size());
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_last_of(
        basic_short_string const& str, size_type pos = npos) const noexcept {
        return index(data(),
            details::string::find_last_of<traits_type>(
                data(), size_clamp(last_to_size(pos)), str.data(), str.size()));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_last_of(
        const_pointer str, size_type pos, size_type count) const noexcept {
        UTL_ASSERT(str != nullptr);
        return index(data(),
            details::string::find_last_of<traits_type>(
                data(), size_clamp(last_to_size(pos)), str, count));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_last_of(
        const_pointer str, size_type pos = npos) const noexcept {
        UTL_ASSERT(str != nullptr);
        return find_last_of(str, pos, traits_type::length(str));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_last_of(
        value_type ch, size_type pos = npos) const noexcept {
        return find_last_of(&ch, pos, 1);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 size_type find_last_of(View const& view, size_type pos = npos) const
        noexcept(is_nothrow_convertible<View, view_type>::value) {
        view_type const v(view);
        return find_last_of(v.data(), pos, v.size());
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_last_not_of(
        basic_short_string const& str, size_type pos = npos) const noexcept {
        return index(data(),
            details::string::find_last_not_of<traits_type>(
                data(), size_clamp(last_to_size(pos)), str.data(), str.size()));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_last_not_of(
        const_pointer str, size_type pos, size_type count) const noexcept {
        return index(data(),
            details::string::find_last_not_of<traits_type>(
                data(), size_clamp(last_to_size(pos)), str, count));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_last_not_of(
        const_pointer str, size_type pos = npos) const noexcept {
        UTL_ASSERT(str != nullptr);
        return find_last_not_of(str, pos, traits_type::length(str));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 size_type find_last_not_of(
        value_type ch, size_type pos = npos) const noexcept {
        return find_last_not_of(&ch, pos, 1);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 size_type find_last_not_of(View const& view, size_type pos = npos) const
        noexcept(is_nothrow_convertible<View, view_type>::value) {
        view_type const v(view);
        return find_last_not_of(v.data(), pos, v.size());
    }

    UTL_STRING_PURE constexpr int compare(basic_short_string const& other) const noexcept {
        return compare_str(view_type(*this), view_type(other));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 int compare(
        size_type pos, size_type count, basic_short_string const& other) const UTL_THROWS {
        return compare(pos, count, other, 0);
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 int compare(size_type pos, size_type count,
        basic_short_string const& other, size_type pos2, size_type count2 = npos) const UTL_THROWS {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::compare` operation failed, "
                                    "Reason=[index out of range]"));
        UTL_THROW_IF(pos2 > other.size(),
            program_exception<void>("[UTL] `basic_short_string::compare` operation failed, "
                                    "Reason=[argument index out of range]"));
        return compare_str(
            view_type(*this).substr(pos, count), view_type(other).substr(pos2, count2));
    }

    UTL_STRING_PURE constexpr int compare(const_pointer str) const noexcept {
        UTL_ASSERT_CXX14(str != nullptr);
        return compare_str(view_type(*this), view_type(str));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 int compare(
        size_type pos, size_type count, const_pointer str, size_type count2) const UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::compare` operation failed, "
                                    "Reason=[index out of range]"));
        return compare_str(view_type(*this).substr(pos, count), view_type(str, count2));
    }

    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 int compare(
        size_type pos, size_type count, const_pointer str) const UTL_THROWS {
        UTL_ASSERT(str != nullptr);
        return compare(pos, count, str, traits_type::length(str));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_STRING_PURE constexpr int compare(View const& view) const
        noexcept(!utl::with_exceptions && is_nothrow_convertible<View, view_type>::value) {
        return compare_str(view_type(*this), view);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 int compare(size_type pos, size_type count, View const& view) const
        noexcept(!utl::with_exceptions && is_nothrow_convertible<View, view_type>::value) {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::compare` operation failed, "
                                    "Reason=[index out of range]"));
        return compare_str(view_type(*this).substr(pos, count), view);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>) View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_STRING_PURE UTL_CONSTEXPR_CXX14 int compare(size_type pos, size_type count, View const& view,
        size_type pos2, size_type count2 = npos) const
        noexcept(!utl::with_exceptions && is_nothrow_convertible<View, view_type>::value) {
        UTL_THROW_IF(pos > size(),
            program_exception<void>("[UTL] `basic_short_string::compare` operation failed, "
                                    "Reason=[index out of range]"));
        return compare_str(view_type(*this).substr(pos, count), view, pos2, count2);
    }

    constexpr bool starts_with(view_type view) const noexcept {
        return size() >= view.size() && traits_type::compare(data(), view.data(), view.size()) == 0;
    }

    constexpr bool starts_with(value_type ch) const noexcept { return !empty() && front() == ch; }

    constexpr bool starts_with(const_pointer str) const noexcept {
        UTL_ASSERT_CXX14(str != nullptr);
        return starts_with(view_type(str));
    }

    constexpr bool ends_with(view_type view) const noexcept {
        return size() >= view.size() &&
            traits_type::compare(view_type(*this).substr(size() - view.size()).data(), view.data(),
                view.size()) == 0;
    }

    constexpr bool ends_with(value_type ch) const noexcept { return !empty() && back() == ch; }

    constexpr bool ends_with(const_pointer str) const noexcept {
        UTL_ASSERT_CXX14(str != nullptr);
        return ends_with(view_type(str));
    }

    constexpr bool contains(view_type view) const noexcept { return find(view) != npos; }

    constexpr bool contains(value_type ch) const noexcept {
        return traits_type::find(data(), size(), ch) != nullptr;
    }

    constexpr bool contains(const_pointer str) const noexcept {
        UTL_ASSERT_CXX14(str != nullptr);
        return contains(view_type(str));
    }

    UTL_CONSTEXPR_CXX14 basic_short_string substr(size_type pos = 0, size_type count = npos) const& {
        return basic_short_string(*this, pos, count);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string substr(size_type pos = 0, size_type count = npos) const&& {
        return basic_short_string(move(*this), pos, count);
    }

private:
    UTL_STRING_PURE static constexpr size_type index(
        const_pointer base, const_pointer ptr) noexcept {
        return ptr ? ptr - base : npos;
    }
    UTL_STRING_PURE constexpr size_type last_to_size(size_type val) noexcept {
        return val + (val < size());
    }

    UTL_STRING_PURE constexpr size_type size_clamp(size_type val) const noexcept {
        return val > size() ? size() : val;
    }

    UTL_STRING_PURE static UTL_CONSTEXPR_CXX14 int compare_str(
        view_type left, view_type right, size_type pos2, size_type count2) UTL_THROWS {
        UTL_THROW_IF(pos2 > right.size(),
            program_exception<void>("[UTL] `basic_short_string::compare` operation failed, "
                                    "Reason=[index out of range]"));
        return compare_str(left, right.substr(pos2, count2));
    }

    UTL_STRING_PURE static constexpr int compare_str(view_type left, view_type right) noexcept {
        return compare_size(
            traits_type::compare(left.data(), right.data(), min(left.size(), right.size())),
            left.size(), right.size());
    }
    UTL_STRING_CONST static constexpr int compare_size(
        int strcmp, size_type left, size_type right) noexcept {
        return strcmp != 0 ? strcmp : left == right ? 0 : left < right ? -1 : 1;
    }

    UTL_CONSTEXPR_CXX14 void swap(basic_short_string& other, false_type) noexcept {
        swap(storage_.first(), other_.storage_.first());
        auto size_tmp = size_;
        auto heap_tmp = is_heap_;
        size_ = other.size_;
        is_heap_ = other.is_heap_;
        other.size_ = size_tmp;
        other.is_heap_ = heap_tmp;
    }

    UTL_CONSTEXPR_CXX14 void swap(basic_short_string& other, true_type) noexcept {
        static constexpr false_type constant = {};
        swap(other, constant);
        swap(allocator_ref(), other_.allocator_ref());
    }

    UTL_CONSTEXPR_CONSTRUCTS_AT void transfer_to_heap(size_t new_capacity) UTL_THROWS {
        UTL_ASSERT(!is_heap_);
        auto const result = alloc_traits::allocate_at_least(allocator_, new_capacity + 1);
        heap_type new_heap{result.ptr, result.count};
        traits_type::copy(new_heap.data_, data(), size());
        construct_at(addressof(get_heap()), new_heap);
        is_heap_ = true;
    }

    UTL_CONSTEXPR_CXX20 void grow_heap(size_t new_capacity) UTL_THROWS {
        UTL_ASSERT(is_heap_);
        get_heap() = alloc_traits::reallocate_at_least(allocator_, get_heap(), new_capacity + 1);
    }

    UTL_CONSTEXPR_CXX20 void reserve_impl(size_type new_capacity) UTL_THROWS {
        UTL_ASSERT(new_capacity > this->capacity());
        UTL_TRY {
            if (!is_heap_) {
                transfer_to_heap(new_capacity);
            } else {
                grow_heap(new_capacity);
            }
        } UTL_CATCH(...) {
            UTL_THROW(program_exception<void>("[UTL] basic_short_string::reserve operation failed, "
                                              "Reason=[Allocation failed]"));
        }
    }

    UTL_CONSTEXPR_CXX14 void destroy() noexcept {
        if (is_heap_) {
            alloc_traits::deallocate(allocator_ref(), get_heap().data_, get_heap().capacity_);
        }
    }

    static UTL_CONSTEXPR_CXX20 heap_type clone_heap(
        allocator_type& alloc, basic_short_string const& src) UTL_THROWS {
        UTL_ASSERT(src.is_heap_);
        UTL_TRY {
            size_type const buffer_capacity = src.capacity() + 1;
            heap_type heap{alloc_traits::allocate(alloc, buffer_capacity), buffer_capacity};
            traits_type::copy(heap.data_, src.data(), src.size() + 1);
            return heap;
        } UTL_CATCH(...) {
            UTL_THROW(program_exception<void>("[UTL] basic_short_string copy operation failed, "
                                              "Reason=[Allocation failed]"));
        }
    }

    UTL_CONSTEXPR_CXX14 void reset_to_short() {
        if (is_heap_) {
            size_ = 0;
            is_heap_ = false;
            ::new (addressof(get_short())) short_type;
        }
    }

    UTL_CONSTEXPR_CXX14 void move_assign(basic_short_string& other, false_type) UTL_THROWS { *this = other; }

    UTL_CONSTEXPR_CXX14 void move_assign(basic_short_string& other, true_type) noexcept {
        destroy();
        storage_.first() = other.storage_.first();
        alloc_traits::assign(allocator_ref(), move(other.allocator_ref()));
        size_ = other.size_;
        is_heap_ = other.is_heap_;

        other.reset_to_short();
    }

    UTL_STRING_CONST UTL_CONSTEXPR_CXX14 short_type& get_short() noexcept { return storage_.first().short_; }
    UTL_STRING_CONST UTL_CONSTEXPR_CXX14 heap_type& get_heap() noexcept { return storage_.first().heap_; }
    UTL_STRING_CONST UTL_CONSTEXPR_CXX14 allocator_type& allocator_ref() noexcept {
        return storage_.second();
    }
    UTL_STRING_CONST constexpr short_type const& get_short() const noexcept {
        return storage_.first().short_;
    }
    UTL_STRING_CONST constexpr heap_type const& get_heap() const noexcept {
        return storage_.first().heap_;
    }
    UTL_STRING_CONST constexpr allocator_type const& allocator_ref() const noexcept {
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

// template <typename CharT, size_t N, typename Traits, typename Alloc, typename U>
// UTL_CONSTEXPR_CXX14 basic_short_string<CharT, N, Traits, Alloc>::size_type erase(
//     basic_short_string<CharT, N, Traits, Alloc>& c, U const& value) {
//     auto const it = remove(c.begin(), c.end(), value);
//     auto const result = c.end() - it;
//     c.erase(it, c.end());
//     return result;
// }

// template <typename CharT, size_t N, typename Traits, typename Alloc, typename Pred>
// UTL_CONSTEXPR_CXX14 basic_short_string<CharT, N, Traits, Alloc>::size_type erase_if(
//     basic_short_string<CharT, N, Traits, Alloc>& c, Pred const& pred) {
//     auto const it = remove_if(c.begin(), c.end(), pred);
//     auto const result = c.end() - it;
//     c.erase(it, c.end());
//     return result;
// }

#undef UTL_STRING_PURE
#undef UTL_STRING_CONST

UTL_NAMESPACE_END
