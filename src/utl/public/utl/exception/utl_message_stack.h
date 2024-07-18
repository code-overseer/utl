// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/exception/utl_exception_base.h"
#include "utl/exception/utl_message_format.h"
#include "utl/exception/utl_message_header.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/memory/utl_allocator_decl.h"
#include "utl/memory/utl_allocator_traits.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/utility/utl_exchange.h"
#include "utl/utility/utl_move.h"

#include <cstdarg>
#include <cstddef>
#include <cstdio>

UTL_NAMESPACE_BEGIN

namespace exceptions {

template <typename Alloc>
class basic_message_stack {
public:
    using allocator_type = Alloc;
    using size_type = size_t;
    using element_type = message_header;

private:
    static constexpr auto header_size = sizeof(message_header);
    using alloc_traits = allocator_traits<Alloc>;
    static_assert(UTL_TRAIT_is_same(typename alloc_traits::value_type, element_type));

public:
    constexpr basic_message_stack() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(allocator_type)) = default;
    constexpr basic_message_stack(allocator_type const& alloc) noexcept : allocator_(alloc) {}
    constexpr basic_message_stack(basic_message_stack const& other) UTL_THROWS
        : basic_message_stack(
              other, alloc_traits::select_on_container_copy_construction(other.allocator_)) {}
    constexpr basic_message_stack(basic_message_stack&& other) noexcept
        : head_(UTL_SCOPE exchange(other.head_, nullptr))
        , size_(UTL_SCOPE exchange(other.size_, 0))
        , allocator_(UTL_SCOPE move(other.allocator_)) {}
    UTL_CONSTEXPR_CXX14 basic_message_stack(
        basic_message_stack const& other, allocator_type const& alloc) UTL_THROWS
        : size_(other.size_)
        , allocator_(alloc) {
        if (size_ > 0) {
            head_ = clone(other.head_, allocator_);
        }
    }
    UTL_CONSTEXPR_CXX14 basic_message_stack(basic_message_stack&& other,
        allocator_type const& alloc) UTL_NOEXCEPT(alloc_traits::is_always_equal::value)
        : basic_message_stack(
              UTL_SCOPE move(other), alloc, typename alloc_traits::is_always_equal{}) {}

    UTL_CONSTEXPR_CXX14 basic_message_stack& operator=(basic_message_stack const& other)
        UTL_THROWS {
        if (UTL_SCOPE addressof(other) == this) {
            return *this;
        }
        clear();
        size_ = other.size_;
        alloc_traits::assign(allocator_, other.allocator_);
        if (size_ > 0) {
            head_ = clone(other.head_, allocator_);
        }

        return *this;
    }

    UTL_CONSTEXPR_CXX14 basic_message_stack& operator=(basic_message_stack&& other) noexcept(
        alloc_traits::is_always_equal::value ||
        alloc_traits::propagate_on_container_move_assignment::value) {
        if (UTL_SCOPE addressof(other) == this) {
            return *this;
        }

        using always_equal = typename alloc_traits::is_always_equal;
        using propogate = typename alloc_traits::propagate_on_container_move_assignment;
        move_assign(other, always_equal{}, propogate{});

        return *this;
    }

    class const_iterator {
    public:
        using value_type = message_header const;
        using pointer = message_header const*;
        using reference = message_header const&;
        using iterator_concept = UTL_SCOPE forward_iterator_tag;

        constexpr const_iterator() noexcept = default;
        constexpr message_header const& operator*() const noexcept { return *current_; }
        constexpr message_header const* operator->() const noexcept { return current_; }
        const_iterator operator++(int) noexcept {
            auto prev = *this;
            current_ = next(*current_);
            return prev;
        }
        const_iterator& operator++() noexcept {
            current_ = next(*current_);
            return *this;
        }

        constexpr bool operator==(const_iterator other) const noexcept {
            return current_ == other.current_;
        }

        constexpr bool operator!=(const_iterator other) const noexcept {
            return current_ != other.current_;
        }

    private:
        friend basic_message_stack;
        constexpr const_iterator(message_header const* h) : current_(h) {}
        message_header const* current_ = nullptr;
    };

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr message_header const& top() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) { return *head_; }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr bool empty() const noexcept { return size() == 0; }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr size_type size() const noexcept { return size_; }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr const_iterator begin() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return const_iterator(head_);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr const_iterator end() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) { return const_iterator(); }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr const_iterator cbegin() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND){
        return const_iterator(head_);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr const_iterator cend() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND){ return const_iterator(); }

    UTL_CONSTEXPR_CXX20 void pop() noexcept {
        UTL_ASSERT(head_ != nullptr);
        auto prev_head = UTL_SCOPE exchange(head_, next(*head_));
        destroy(prev_head);
        --size_;
    }

    void emplace(message_format fmt, ...) UTL_THROWS {
        va_list args1;
        va_list args2;

        va_start(args1, fmt);
        va_copy(args2, args1);
        auto const str_size = vsnprintf(nullptr, 0, fmt.format, args1);
        va_end(args1);
        auto const buffer_size = str_size + 1;
        auto const count = (buffer_size + sizeof(message_header) - 1) / sizeof(message_header) + 1;

        UTL_TRY {
            // restart lifetime (null operation)
            auto raw_bytes = ::new (allocator_.allocate(count)) unsigned char[count * header_size];
            // start lifetime of header
            auto header = ::new (raw_bytes) message_header(UTL_SCOPE move(fmt.location), str_size);
            // start lifetime of string (null operation)
            auto str = ::new (raw_bytes + header_size) char[buffer_size];
            UTL_ASSERT(str == header->message());
            vsnprintf(str, buffer_size, fmt.format, args2);
            va_end(args2);
            set_next(*header, head_);
            head_ = header;
            ++size_;
        } UTL_CATCH(...) {
            va_end(args2);
            UTL_RETHROW();
        }
    }

    UTL_CONSTEXPR_CXX20 void clear() noexcept {
        destroy_all();
        size_ = 0;
        head_ = nullptr;
    }

    UTL_CONSTEXPR_CXX20 ~basic_message_stack() noexcept { destroy_all(); }

private:
    UTL_ATTRIBUTE(NODISCARD) static UTL_CONSTEXPR_CXX20 message_header* clone(
        message_header* head, allocator_type const& alloc) UTL_THROWS {
        UTL_ASSERT(head != nullptr);

        auto const copied_head = message_header::clone(*head, alloc);
        auto to_copy = next(*head);
        auto copied_prev = copied_head;
        while (to_copy != nullptr) {
            auto copied = copy(*to_copy, alloc);
            set_next(*copied_prev, copied);
            copied_prev = copied;
            to_copy = next(*to_copy);
        }

        return copied_head;
    }

    constexpr void destroy_all() noexcept {
        auto current = head_;
        while (current != nullptr) {
            auto to_delete = current;
            current = next(*current);
            destroy(to_delete);
        }
    }

    UTL_CONSTEXPR_CXX20 void destroy(message_header* msg) noexcept {
        auto const buffer_size = msg->size() + 1;
        auto const count = (buffer_size + header_size - 1) / header_size + 1;
        msg->~message_header();
        allocator_.deallocate(msg, count);
    }

    UTL_CONSTEXPR_CXX14 void move_assign(basic_message_stack& other, true_type always_equal, bool) noexcept {
        head_ = UTL_SCOPE exchange(other.head_, head_);
        size_ = UTL_SCOPE exchange(other.size_, size_);
    }

    UTL_CONSTEXPR_CXX14 void move_assign(
        basic_message_stack& other, false_type, true_type propogate) noexcept {
        if (alloc_traits::equals(allocator_, other.allocator_)) {
            move_assign(other, true_type{}, propogate);
        } else {
            destroy_all();
            head_ = UTL_SCOPE exchange(other.head_, nullptr);
            size_ = UTL_SCOPE exchange(other.size_, nullptr);
            allocator_ = UTL_SCOPE move(other.allocator_);
        }
    }

    UTL_CONSTEXPR_CXX14 void move_assign(basic_message_stack& other, false_type, false_type) noexcept {
        destroy_all();
        head_ = clone(other.head_, allocator_);
        size_ = other.size_;
    }

    constexpr basic_message_stack(basic_message_stack&& other, allocator_type const& alloc,
        bool_constant<alloc_traits::is_always_equal::value == true>) noexcept
        : head_(UTL_SCOPE exchange(other.head_, nullptr))
        , size_(UTL_SCOPE exchange(other.size_, 0)) {}

    UTL_CONSTEXPR_CXX14 basic_message_stack(basic_message_stack&& other, allocator_type const& alloc,
        bool_constant<alloc_traits::is_always_equal::value == false>) UTL_THROWS
        : allocator_(alloc) {
        if (!alloc_traits::equals(allocator_, other.allocator_)) {
            head_ = clone(other.head_, allocator_);
            size_ = other.size_;
        } else {
            head_ = UTL_SCOPE exchange(other.head_, nullptr);
            size_ = UTL_SCOPE exchange(other.size_, 0);
        }
    }

    message_header* head_ = nullptr;
    size_type size_ = 0;
    allocator_type allocator_;
};

using message_stack = basic_message_stack<UTL_SCOPE allocator<message_header>>;

} // namespace exceptions

UTL_NAMESPACE_END
