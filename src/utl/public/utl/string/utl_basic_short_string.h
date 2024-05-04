// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts.h" // integral
#include "utl/exception.h"
#include "utl/iterator/utl_contiguous_iterator_base.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/string/utl_string_fwd.h"
#include "utl/utility/utl_compressed_pair.h"

#include <limits.h>

#include <iterator>

#ifdef UTL_WITH_EXCEPTIONS
#  define UTL_NOTRY_CONSTEXPR_CXX14 UTL_CONSTEXPR_CXX20
#else
#  define UTL_NOTRY_CONSTEXPR_CXX14 UTL_CONSTEXPR_CXX14
#endif

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
    struct default_construct_t {};
    struct short_type {
        value_type data_[ShortSize + 1];
        constexpr short_type() noexcept = default;
        constexpr short_type(default_construct_t) noexcept {}
    };

    struct heap_type {
        pointer data_;
        /**
         * Capacity of the buffer in number of characters
         */
        size_type capacity_;

        constexpr heap_type() noexcept = default;
        constexpr heap_type(heap_type const&) noexcept = default;
        UTL_CONSTEXPR_CXX14 heap_type& operator=(heap_type const&) noexcept = default;

        constexpr heap_type(pointer data, size_type capacity) noexcept
            : data_(data)
            , capacity_(capacity) {}
        heap_type& operator=(allocator_result<pointer, size_type> const& other) noexcept {
            data_ = other.ptr;
            capacity_ = other.size;
        }
        constexpr operator allocator_result<pointer, size_type>() const noexcept {
            return {data_, capacity_};
        }
    };

    union storage_union {
        short_type short_;
        heap_type heap_;
    };

    using storage_type = compressed_pair<storage_union, allocator_type>;

public:
    class iterator : contiguous_iterator_base<iterator, value_type> {
    public:
        constexpr iterator() noexcept = default;
        constexpr iterator(iterator const& other) noexcept = default;
        constexpr iterator(iterator&& other) noexcept = default;
        constexpr iterator& operator=(iterator const& other) noexcept = default;
        constexpr iterator& operator=(iterator&& other) noexcept = default;

    private:
        friend basic_short_string;
        constexpr iterator(pointer data) noexcept : iterator_base(data) {}
        constexpr iterator(contiguous_iterator_base<iterator, value_type const> other) noexcept
            : iterator(const_cast<pointer>(as_ptr(other))) {}
    };

    class const_iterator : contiguous_iterator_base<iterator, value_type const> {
    public:
        constexpr const_iterator() noexcept = default;
        constexpr const_iterator(const_iterator const& other) noexcept = default;
        constexpr const_iterator(const_iterator&& other) noexcept = default;
        constexpr const_iterator& operator=(const_iterator const& other) noexcept = default;
        constexpr const_iterator& operator=(const_iterator&& other) noexcept = default;

    private:
        friend basic_short_string;
        constexpr const_iterator(pointer data) noexcept : contiguous_iterator_base(data) {}
        constexpr const_iterator(contiguous_iterator_base<iterator, value_type> other) noexcept
            : contiguous_iterator_base(other) {}
    };

    using reverse_iterator = ::std::reverse_iterator<iterator>;
    using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

    constexpr basic_short_string(basic_short_string const& other) noexcept(!utl::with_exceptions)
        : storage_(other.storage_.first(),
              alloc_traits::select_on_container_copy_construction(other.storage_.second()))
        , size_(other.size_)
        , is_heap_(other.is_heap_) {

        if (other.is_heap_) {
            auto& allocator = storage_.second();
            storage_.get_heap() = clone_heap(allocator, other);
        }
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& operator=(basic_short_string const& other) noexcept(
        !utl::with_exceptions) {
        if (this != addressof(other)) {
            destroy();
            storage_.first() = other.storage_.first();
            alloc_traits::assign(storage_.second(), other.storage_.second());
            size_ = other.size_;
            is_heap_ = other.is_heap_;

            if (other.is_heap_) {
                auto& allocator = storage_.second();
                storage_.get_heap() = clone_heap(allocator, other);
            }
        }

        return *this;
    }

    UTL_CONSTEXPR_CONSTRUCTS_AT basic_short_string(basic_short_string&& other) noexcept
        : storage_(other.storage_)
        , size_(other.size_)
        , is_heap_(other.is_heap_) {
        other.size_ = 0;
        other.is_heap_ = false;
        construct_at(addressof(other.get_short()), default_construct);
    }

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

    UTL_ATTRIBUTE(NODISCARD) UTL_CONSTEXPR_CXX14 pointer data() noexcept {
        return !is_heap_ ? get_short().data_ : get_heap().data_;
    }

    UTL_ATTRIBUTE(NODISCARD) constexpr const_pointer data() const noexcept {
        return !is_heap_ ? get_short().data_ : get_heap().data_;
    }

    UTL_ATTRIBUTE(NODISCARD) constexpr const_pointer c_str() const noexcept { return data(); }

    UTL_ATTRIBUTE(NODISCARD) constexpr bool empty() const noexcept { return !size_; }
    UTL_ATTRIBUTE(NODISCARD) constexpr size_type size() const noexcept { return size_; }
    UTL_ATTRIBUTE(NODISCARD) constexpr size_type length() const noexcept { return size_; }

    UTL_ATTRIBUTE(NODISCARD) static constexpr size_type max_size() const noexcept {
        static constexpr size_type max_bytes = ~(1 << (sizeof(size_type) * CHAR_BIT - 1));
        static constexpr size_type max_characters = max_bytes / sizeof(value_type);
        return max_characters - 1;
    }

    UTL_ATTRIBUTE(NODISCARD) constexpr size_type capacity() const noexcept {
        static constexpr size_type short_capacity = sizeof(short_type::data_) / sizeof(value_type);
        size_type const buffer_capacity = !is_heap_ ? short_capacity : get_heap().capacity_;
        size_type const string_capacity = buffer_capacity - 1;
        return string_capacity;
    }

    UTL_ATTRIBUTE(NODISCARD)
    constexpr allocator_type get_allocator() const noexcept { return storage_.second(); }

    UTL_CONSTEXPR_CXX14 void reserve(size_type new_capacity) noexcept(!utl::with_exceptions) {
        if (new_capacity <= this->capacity()) {
            return;
        }

        UTL_THROW_IF(new_capacity > max_size(),
            program_exception<void>("[UTL] basic_short_string::reserve operation failed, "
                                    "Reason=[Requested capacity exceeds maximum size]"));
        reserve_impl(new_capacity);
    }

    UTL_CONSTEXPR_CXX14 void resize(size_type new_size) noexcept(!utl::with_exceptions) {
        resize(new_size, value_type());
    }

    UTL_CONSTEXPR_CXX14 void resize(size_type new_size, value_type ch) noexcept(
        !utl::with_exceptions) {
        reserve(new_size);

        auto end_ptr = data() + new_size;
        *end_ptr = value_type();
        for (auto dst = data() + size(); dst < end_ptr; ++dst) {
            *dst = ch;
        }
        size_ = new_size;
    }

    template <typename Op UTL_REQUIRES_CXX11(
        is_integral<decltype(declval<Op>()(pointer(), size_type()))>::value)>
    UTL_REQUIRES_CXX20(requires(Op op, pointer p, size_type s) {
        { op(p, s) } -> integral;
    })
    UTL_NOTRY_CONSTEXPR_CXX14 void resize_and_overwrite(size_type new_size, Op operation) noexcept(
        !utl::with_exceptions) {
        reserve(new_size);
        UTL_TRY {
            size_ = move(operation)(data(), new_size);
            *end() = value_type();
            // Thrown exception is UB so assert
            UTL_ASSERT(size_ < new_size);
        }
        UTL_CATCH(...) {
            UTL_ASSERT(false);
        }
    }

    UTL_CONSTEXPR_CXX14 void shrink_to_fit() noexcept(!utl::with_exceptions) {
        if (is_heap_ && size() < capacity()) {
            get_heap() = alloc_traits::reallocate(allocator_, get_heap(), size() + 1);
        }
    }

    UTL_ATTRIBUTE(REINITIALIZE)
    UTL_CONSTEXPR_CXX14 void clear() noexcept {
        *data() = 0;
        size_ = 0;
    }

    constexpr iterator begin() noexcept { return iterator(data()); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(data()); }
    constexpr const_iterator begin() const noexcept { return const_iterator(data()); }
    constexpr iterator end() noexcept { return iterator(data() + size()); }
    constexpr const_iterator cend() const noexcept { return const_iterator(data() + size()); }
    constexpr const_iterator end() const noexcept { return const_iterator(data() + size()); }
    constexpr iterator rbegin() noexcept { return reverse_iterator(end()); }
    constexpr const_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    constexpr const_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    constexpr iterator rend() noexcept { return reverse_iterator(begin()); }
    constexpr const_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
    constexpr const_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(size_type count, value_type ch) noexcept(
        !utl::with_exceptions) {
        UTL_ATTRIBUTE(MAYBE_UNUSED) size_type const old_size = exchange(size_, 0);
        UTL_TRY {
            resize(count, ch);
            return *this;
        }
        UTL_CATCH(...) {
            size_ = old_size;
            UTL_RETRHOW;
        }
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(basic_short_string const& other) noexcept(
        !utl::with_exceptions) {
        return *this = other;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(basic_short_string const& other, size_type pos,
        size_type count = npos) noexcept(!utl::with_exceptions) {
        UTL_THROW_IF(pos > other.size(),
            program_exception<void>("[UTL] basic_short_string::assign operation failed, "
                                    "Reason=[substring position is out of range]"));
        count = count > other.size() ? other.size() : count;
        resize(count);
        traits_type::copy(data(), other.data() + pos, count);
        return *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(basic_short_string&& other) noexcept(
        !utl::with_exceptions) {
        return *this = move(other);
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(pointer str, size_type count) noexcept(
        !utl::with_exceptions) {
        reserve(count);
        traits_type::copy(data(), str, count);
        size_ = count;
        return *this;
    }

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(pointer str) noexcept(!utl::with_exceptions) {
        assign(str, traits_type::length(str));
    }

    template <UTL_CONCEPT_CXX20(details::iterator::InputIterator)
            It UTL_REQUIRES_CXX11(details::iterator::InputIterator<It>::value &&
                !details::iterator::ForwardIterator<It>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& assign(It begin, It end) noexcept(
        !utl::with_exceptions) {
        size_ = 0;
        while (begin != end) {
            push_back(*begin);
            ++begin;
        }
    }

    template <UTL_CONCEPT_CXX20(details::iterator::ForwardIterator)
            It UTL_REQUIRES_CXX11(details::iterator::ForwardIterator<It>::value)>
    UTL_CONSTEXPR_CXX17 basic_short_string& assign(It begin, It end) noexcept(
        !utl::with_exceptions) {
        size_type diff = static_cast<size_type>(::std::distance(begin, end));
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

    UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        ::std::initializer_list<value_type> list) noexcept(!utl::with_exceptions) {
        assign(list.begin(), list.end());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>)
            View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& assign(View view) noexcept(!utl::with_exceptions) {
        auto v = basic_string_view<value_type>(view);
        return assign(v.data(), v.size());
    }

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>)
            View UTL_REQUIRES_CXX11(is_convertible<View, view_type>::value)>
    UTL_CONSTEXPR_CXX14 basic_short_string& assign(
        View view, size_type pos, size_type count = npos) noexcept(!utl::with_exceptions) {
        UTL_THROW_IF(pos > view.size(),
            program_exception<void>("[UTL] basic_short_string::assign operation failed, "
                                    "Reason=[substring position is out of range]"));

        auto v = basic_string_view<value_type>(view).substr(pos, count);
        return assign(v.data(), v.size());
    }

protected:
    UTL_CONSTEXPR_CXX20 ~basic_short_string() noexcept { destroy(); }

private:
    constexpr size_t used_bytes() const noexcept { return (size() + 1) * sizeof(value_type); }

    UTL_CONSTEXPR_CONSTRUCTS_AT void transfer_to_heap(size_t new_capacity) noexcept(
        !utl::with_exceptions) {
        UTL_ASSERT(!is_heap_);
        auto const result = alloc_traits::allocate_at_least(allocator_, new_capacity + 1);
        heap_type new_heap{result.ptr, result.count};
        traits_type::copy(new_heap.data_, data(), size());
        construct_at(addressof(get_heap()), new_heap);
    }

    UTL_CONSTEXPR_CXX20 void grow_heap(size_t new_capacity) noexcept(!utl::with_exceptions) {
        UTL_ASSERT(is_heap_);
        get_heap() = alloc_traits::reallocate_at_least(allocator_, get_heap(), new_capacity + 1);
    }

    UTL_CONSTEXPR_CXX20 void reserve_impl(size_type new_capacity) noexcept(!utl::with_exceptions) {
        UTL_ASSERT(new_capacity > this->capacity());
        UTL_TRY {
            if (!is_heap_) {
                transfer_to_heap(new_capacity);
            } else {
                grow_heap(new_capacity);
            }
        }
        UTL_CATCH(...) {
            UTL_THROW(program_exception<void>("[UTL] basic_short_string::reserve operation failed, "
                                              "Reason=[Allocation failed]"));
        }
    }

    UTL_CONSTEXPR_CXX14 void destroy() noexcept {
        if (is_heap_) {
            alloc_traits::deallocate(storage_.second(), get_heap().data_, get_heap().capacity_);
        }
    }

    static UTL_CONSTEXPR_CXX20 heap_type clone_heap(
        allocator_type& alloc, basic_short_string const& src) noexcept(!utl::with_exceptions) {
        UTL_ASSERT(src.is_heap_);
        UTL_TRY {
            size_type const buffer_capacity = src.capacity() + 1;
            heap_type heap{alloc_traits::allocate(alloc, buffer_capacity), buffer_capacity};
            traits_type::copy(heap.data_, src.data(), src.size());
            return heap;
        }
        UTL_CATCH(...) {
            UTL_THROW(program_exception<void>("[UTL] basic_short_string copy operation failed, "
                                              "Reason=[Allocation failed]"));
        }
    }

    UTL_CONSTEXPR_CXX14 void move_assign(basic_short_string& other, false_type) noexcept(
        !utl::with_exceptions) {
        *this = other;
    }

    UTL_CONSTEXPR_CONSTRUCTS_AT void move_assign(basic_short_string& other, true_type) noexcept {
        destroy();
        storage_.first() = other.storage_.first();
        alloc_traits::assign(storage_.second(), move(other.storage_.second()));
        size_ = other.size_;
        is_heap_ = other.is_heap_;

        other.size_ = 0;
        other.is_heap_ = false;
        static constexpr default_construct_t default_construct = {};
        construct_at(addressof(other.get_short()), default_construct);
    }

    UTL_ATTRIBUTES(NODISCARD, CONST)
    constexpr short_type const& get_short() const noexcept { return storage_.first().short_; }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    constexpr short_type& get_short() noexcept { return storage_.first().short_; }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    constexpr heap_type const& get_heap() const noexcept { return storage_.first().heap_; }
    UTL_ATTRIBUTES(NODISCARD, CONST)
    constexpr heap_type& get_heap() noexcept { return storage_.first().heap_; }

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

UTL_NAMESPACE_END
