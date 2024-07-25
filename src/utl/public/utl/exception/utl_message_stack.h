// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/exception/utl_exception_base.h"
#include "utl/exception/utl_message_header.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/memory/utl_addressof.h"
#include "utl/utility/utl_exchange.h"
#include "utl/utility/utl_move.h"

#include <cstdarg>

UTL_NAMESPACE_BEGIN

namespace exceptions {

class UTL_ABI_PUBLIC message_stack {
public:
    using size_type = size_t;
    using value_type = message_header;
    using reference = value_type&;
    using const_reference = value_type const&;
    class const_iterator;
    using iterator = const_iterator;

private:
    static constexpr auto header_size = sizeof(message_header);

public:
    UTL_HIDE_FROM_ABI constexpr message_stack() noexcept = default;
    UTL_HIDE_FROM_ABI constexpr message_stack(message_stack&& other) noexcept
        : head_(UTL_SCOPE exchange(other.head_, nullptr))
        , tail_(UTL_SCOPE exchange(other.tail_, nullptr))
        , size_(UTL_SCOPE exchange(other.size_, 0)) {}
    UTL_HIDE_FROM_ABI constexpr message_stack(message_stack const& other) noexcept
        : head_(other.tail_ ? (increment(*other.tail_), other.head_) : other.head_)
        , tail_(other.tail_)
        , size_(other.size_) {}
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 message_stack& operator=(message_stack const& other) noexcept {
        if (UTL_SCOPE addressof(other) == this) {
            return *this;
        }

        // internally a doubly-linked-list, with the tail being the first entry
        // The first entry holds the reference count and owns all other subsequent entries
        if (tail_ != nullptr) {
            decrement(*tail_);
        }

        increment(*other.tail_);
        head_ = other.head_;
        tail_ = other.tail_;
        size_ = other.size_;
        return *this;
    }

    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 message_stack& operator=(message_stack&& other) noexcept {
        if (UTL_SCOPE addressof(other) == this) {
            return *this;
        }

        if (tail_ != nullptr) {
            decrement(*tail_);
        }

        head_ = UTL_SCOPE exchange(other.head_, nullptr);
        tail_ = UTL_SCOPE exchange(other.tail_, nullptr);
        size_ = UTL_SCOPE exchange(other.size_, 0);
        return *this;
    }

    class const_iterator {
    public:
        using value_type = message_header const;
        using pointer = message_header const*;
        using reference = message_header const&;
        using iterator_concept = UTL_SCOPE forward_iterator_tag;

        UTL_HIDE_FROM_ABI const_iterator operator++(int) noexcept {
            auto prev = *this;
            current_ = next(*current_);
            return prev;
        }
        UTL_HIDE_FROM_ABI const_iterator& operator++() noexcept {
            current_ = next(*current_);
            return *this;
        }

        UTL_HIDE_FROM_ABI constexpr const_iterator() noexcept = default;
        UTL_HIDE_FROM_ABI constexpr message_header const& operator*() const noexcept {
            return *current_;
        }
        UTL_HIDE_FROM_ABI constexpr message_header const* operator->() const noexcept {
            return current_;
        }

        UTL_HIDE_FROM_ABI constexpr bool operator==(const_iterator other) const noexcept {
            return current_ == other.current_;
        }

        UTL_HIDE_FROM_ABI constexpr bool operator!=(const_iterator other) const noexcept {
            return current_ != other.current_;
        }

    private:
        friend message_stack;
        UTL_HIDE_FROM_ABI constexpr const_iterator(message_header const* p) : current_(p) {}
        message_header const* current_ = nullptr;
    };

    UTL_ATTRIBUTES(NODISCARD, PURE) UTL_HIDE_FROM_ABI constexpr message_header const& top() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return *head_;
    }

    UTL_ATTRIBUTES(NODISCARD, PURE) UTL_HIDE_FROM_ABI constexpr bool empty() const noexcept {
        return size() == 0;
    }

    UTL_ATTRIBUTES(NODISCARD, PURE) UTL_HIDE_FROM_ABI constexpr size_type size() const noexcept {
        return size_;
    }

    UTL_ATTRIBUTES(NODISCARD, PURE) UTL_HIDE_FROM_ABI constexpr const_iterator begin() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return const_iterator(head_);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE) UTL_HIDE_FROM_ABI constexpr const_iterator end() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return const_iterator();
    }

    UTL_ATTRIBUTES(NODISCARD, PURE) UTL_HIDE_FROM_ABI constexpr const_iterator cbegin() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND){
        return const_iterator(head_);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE) UTL_HIDE_FROM_ABI constexpr const_iterator cend() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND){
        return const_iterator();
    }

    UTL_HIDE_FROM_ABI void emplace(message_format fmt, ...) UTL_THROWS {
        va_list args;
        va_start(args, fmt);
        auto header = message_header::create(UTL_SCOPE move(fmt), args);
        va_end(args);
        set_next(*header, head_);

        if (tail_ == nullptr) {
            UTL_ASSERT(head_ == nullptr && empty());
            tail_ = header;
        }

        head_ = header;
        ++size_;
    }

    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 ~message_stack() noexcept {
        if (tail_ != nullptr) {
            decrement(*tail_);
        }

        size_ = 0;
        head_ = nullptr;
        tail_ = nullptr;
    }

private:
    message_header* head_ = nullptr;
    message_header* tail_ = nullptr;
    size_type size_ = 0;
};

} // namespace exceptions

UTL_NAMESPACE_END
