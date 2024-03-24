// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"
#include "utl/exception/utl_exception_base.h"

#include <string.h>

UTL_NAMESPACE_BEGIN

template <typename T>
class program_exception;
template <>
class program_exception<void> : public exception {

public:
    program_exception(char const* msg) : msg_ptr_(msg_buffer_) {
        size_t length = strlen(msg);
        if (length >= sizeof(msg_buffer_)) {
            msg_ptr_ = new char[length];
        }
        msg_ptr_[length] = 0;
        memcpy(msg_ptr_, msg, length);
    }

    ~program_exception() noexcept {
        if (msg_ptr_ != msg_buffer_) {
            delete[] msg_ptr_;
        }
    }

    UTL_ATTRIBUTE(NODISCARD) char const* what() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) final {
        return msg_ptr_;
    }

    // TODO add non-const what

private:
    // TODO add throw location
    // TODO add stack trace [maybe]
    // TODO use strings
    char msg_buffer_[1024];
    char* msg_ptr_;
};

template <typename T>
class program_exception : public program_exception<void> {
    using base_type = program_exception<void>;

public:
    program_exception(char const* msg, T data) : base_type(msg), data_(move(data)) {}

    UTL_ATTRIBUTES(NODISCARD, CONST) T const& data() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return data_;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST) T& data() noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return data_;
    }

private:
    T data_;
};

UTL_NAMESPACE_END
