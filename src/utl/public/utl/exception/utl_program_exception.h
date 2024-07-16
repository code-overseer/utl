// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/exception/utl_exception_base.h"

#include <cstddef>
#include <cstring>

UTL_NAMESPACE_BEGIN

namespace details {
namespace exceptions {

struct format_source {
    template <size_t N>
    format_source(char const (&fmt)[N], int src = 0) noexcept : format(fmt)
                                                              , source(src) {}

    char const (&format)[N];
    UTL_SCOPE source_location source;
};

class message_header {
public:
    UTL_ATTRIBUTES(NODISCARD, CONST) UTL_SCOPE source_location const& source() const noexcept {
        return source_;
    }
    UTL_ATTRIBUTES(NODISCARD, PURE) char const* message() const noexcept {
        return reinterpret_cast<char*>(this) + sizeof(h);
    }

private:
    UTL_SCOPE source_location source_;
    message_header* next_;

    friend void set_next(message_header& h, message_header const* value) noexcept {
        h.next_ = value;
    }

    friend void next(message_header const& h) noexcept { return h.next_; }
};

class message_stack {
public:
    constexpr message_stack() noexcept = default;
    message_stack(message_stack const&) = delete;
    message_stack& operator=(message_stack const&) = delete;
    constexpr message_stack(message_stack&& other) noexcept
        : head_(UTL_SCOPE exchange(other.head_, nullptr)) {}
    message_stack& operator=(message_stack&& other) {
        clear();
        head_ = UTL_SCOPE exchange(other.head_, nullptr);
        return *this;
    }
    class const_iterator {
    public:
    };

    message_header const& top() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) { return *head_; }

    bool empty() const noexcept { return size() == 0; }

    size_t size() const noexcept { return size_; }

    void pop() {
        auto prev_head = UTL_SCOPE exchange(head_, next(*head_));
        destroy(prev_head);
    }

    ~message_stack() { clear(); }

    void emplace(format_source fmt, ...) {
        va_list args1;
        va_list args2;
        va_start(args1, fmt);
        va_copy(args2, args1);
        auto const buffer_size = vsnprintf(nullptr, 0, fmt.format, args1) + 1;
        va_end(args1);
        static constexpr size_t header_size = sizeof(message_header);
        auto ptr = ::operator new(header_size + buffer_size);
        auto message = ::new (ptr) message_header(line, filename, function);
        auto str = new (const_cast<char*>(message->message())) char[buffer_size];
        vsnprintf(str, buffer_size, fmt.format, args2);
        va_end(args2);

        set_next(*message, head_);
        head_ = message;
        ++size_;
    }

private:
    static void destroy(message_header* msg) noexcept {
        msg->~message_header();
        ::operator delete(msg);
    }
    void clear() noexcept {
        auto current = head_;
        while (current != nullptr) {
            auto to_delete = current;
            current = next(*current);
            destroy(to_delete);
        }
    }

    message_header* head_ = nullptr;
    size_t size_ = 0;
};
} // namespace exceptions
} // namespace details

#define UTL_PROGRAM_EXCEPTION_PUSH_MESSAGE(P, FORMAT, ...)

template <typename T>
class program_exception;

template <>
class program_exception<void> : public exception {

public:
    program_exception(details::exception::format_source fmt, ...)
        : source_(UTL_SCOPE move(fmt.src)) {
        va_list args;
        va_start(args, fmt);
        messages_.emplace(
            source_.line(), source_.file_name(), source_.function_name(), fmt.format, args);
        va_end(args);
    }

    ~program_exception() noexcept {
        if (msg_ptr_ != msg_buffer_) {
            delete[] msg_ptr_;
        }
    }

    UTL_ATTRIBUTE(NODISCARD) char const* what() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) final {
        return messages_.front().message();
    }

    message_stack release_messages() noexcept { return UTL_SCOPE move(messages_); }

private:
    // TODO add throw location
    // TODO add stack trace [maybe]
    UTL_SCOPE source_location source_;
    message_stack messages_;
};

template <typename T>
class program_exception : public program_exception<void> {
    using base_type = program_exception<void>;

public:
    template <size_t N, typename... Args>
    program_exception(T data, details::exception::format_source<N> fmt, Args... args)
        : base_type(fmt, args...)
        , data_(UTL_SCOPE move(data)) {}

    UTL_ATTRIBUTES(NODISCARD, CONST) T const& data() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return data_;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST) T& data() noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) { return data_; }

private:
    T data_;
};

UTL_NAMESPACE_END
