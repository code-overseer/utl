// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/concepts/utl_constructible_as.h"
#include "utl/exception/utl_exception_base.h"
#include "utl/exception/utl_message_format.h"
#include "utl/exception/utl_message_header.h"
#include "utl/exception/utl_message_stack.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"

UTL_NAMESPACE_BEGIN

template <typename T>
class basic_exception;

template <>
class basic_exception<void> : public exception {

public:
    explicit basic_exception(exceptions::message_format fmt, ...) : location_(fmt.location) {
        va_list args;
        va_start(args, fmt);
        messages_.emplace(UTL_SCOPE move(fmt), args);
        va_end(args);
    }

    basic_exception(exceptions::message_stack stack, exceptions::message_format fmt, ...)
        : location_(fmt.location)
        , messages_(UTL_SCOPE move(stack)) {
        va_list args;
        va_start(args, fmt);
        messages_.emplace(UTL_SCOPE move(fmt), args);
        va_end(args);
    }

    UTL_ATTRIBUTE(NODISCARD) char const* what() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) final {
        return messages_.top().message();
    }

    void emplace_message(exceptions::message_format fmt, ...) {
        va_list args;
        va_start(args, fmt);
        messages_.emplace(UTL_SCOPE move(fmt), args);
        va_end(args);
    }

    constexpr exceptions::message_stack const& messages() const { return messages_; }

    exceptions::message_stack release_messages() noexcept { return UTL_SCOPE move(messages_); }

private:
    // TODO add stack trace
    UTL_SCOPE source_location location_;
    exceptions::message_stack messages_;
};

template <typename T>
class basic_exception : public basic_exception<void> {
    using base_type = basic_exception<void>;

public:
    template <UTL_CONCEPT_CXX20(constructible_as<T>) U, typename... Args UTL_REQUIRES_CXX11(
        is_constructible<T, U>::value)>
    basic_exception(U&& u, exceptions::message_format fmt, Args... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, U))
        : base_type(UTL_SCOPE move(fmt), args...)
        , data_(UTL_SCOPE forward<U>(u)) {}

    template <UTL_CONCEPT_CXX20(constructible_as<T>) U, typename... Args UTL_REQUIRES_CXX11(
        is_constructible<T, U>::value)>
    basic_exception(U&& u, exceptions::message_stack stack, exceptions::message_format fmt,
        Args... args) noexcept(UTL_TRAIT_is_nothrow_constructible(T, U))
        : base_type(UTL_SCOPE move(stack), UTL_SCOPE move(fmt), args...)
        , data_(UTL_SCOPE forward<U>(u)) {}

    UTL_ATTRIBUTES(NODISCARD, CONST) T const& data() const& noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return data_;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST) T const&& data() const&& noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return data_;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST) T& data() & noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) { return data_; }

    UTL_ATTRIBUTES(NODISCARD, CONST) T&& data() && noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) { return data_; }

private:
    T data_;
};

using program_exception = basic_exception<void>;

UTL_NAMESPACE_END
