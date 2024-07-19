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

/**
 * @brief Specialization of basic_exception for void type, inheriting from exception.
 *
 * This class represents the default version of basic_exception where no additional
 * context data is required, providing functionality to handle exceptions with
 * formatted messages and source location tracking.
 */
template <>
class basic_exception<void> : public exception {

public:
    /**
     * @brief Constructs a basic_exception with a formatted message.
     *
     * This constructor initializes the exception with a formatted message and its
     * source location. It supports variadic arguments for the message format.
     *
     * @param fmt The message format object containing the format string and source location.
     * @param ... Variadic arguments for the message format.
     */
    explicit basic_exception(exceptions::message_format fmt, ...) : location_(fmt.location) {
        va_list args;
        va_start(args, fmt);
        messages_.emplace(UTL_SCOPE move(fmt), args);
        va_end(args);
    }

    /**
     * @brief Retrieves the message associated with the exception.
     *
     * This function returns the top message from the message stack associated with
     * the exception.
     *
     * @return The message string.
     */
    UTL_ATTRIBUTE(NODISCARD) char const* what() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) final {
        return messages_.top().message();
    }

    /**
     * @brief Adds a formatted message to the message stack.
     *
     * This function emplaces a new formatted message into the message stack. It supports
     * variadic arguments for the message format.
     *
     * @param fmt The message format object containing the format string.
     * @param ... Variadic arguments for the message format.
     */
    void emplace_message(exceptions::message_format fmt, ...) {
        va_list args;
        va_start(args, fmt);
        messages_.emplace(UTL_SCOPE move(fmt), args);
        va_end(args);
    }

    /**
     * @brief Retrieves the message stack associated with the exception.
     *
     * This function returns a constant reference to the message stack.
     *
     * @return The message stack.
     */
    constexpr exceptions::message_stack const& messages() const { return messages_; }

private:
    // TODO add stack trace
    UTL_SCOPE source_location location_;
    exceptions::message_stack messages_;
};

/**
 * @brief Template specialization of basic_exception for a specific type T.
 *
 * This class represents a template specialization of basic_exception that inherits
 * from basic_exception<void>. It provides functionality to handle exceptions with
 * formatted messages, source location tracking, and additional context data storage
 * of type T.
 *
 * @tparam T The type of data to be associated with the exception.
 */
template <typename T>
class basic_exception : public basic_exception<void> {
    using base_type = basic_exception<void>;

public:
    /**
     * @brief Constructs a basic_exception with data of type T and a formatted message.
     *
     * This constructor initializes the exception with data of type T, a formatted message,
     * and its source location. It supports variadic arguments for the message format.
     *
     * @tparam U The type of the data being forwarded to construct T.
     * @tparam Args The types of the additional arguments for the message format.
     * @param u The data to be stored in the exception.
     * @param fmt The message format object containing the format string and source location.
     * @param args The additional arguments for the message format.
     */
    template <UTL_CONCEPT_CXX20(constructible_as<T>) U, typename... Args UTL_REQUIRES_CXX11(
        is_constructible<T, U>::value)>
    basic_exception(U&& u, exceptions::message_format fmt, Args... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, U))
        : base_type(UTL_SCOPE move(fmt), args...)
        , data_(UTL_SCOPE forward<U>(u)) {}

    /**
     * @brief Retrieves the data associated with the exception.
     *
     * This function returns a constant reference to the data stored in the exception.
     *
     * @return The constant reference to the data.
     */
    UTL_ATTRIBUTES(NODISCARD, CONST) T const& data() const& noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return data_;
    }

    /**
     * @brief Retrieves the data associated with the exception.
     *
     * This function returns a constant rvalue reference to the data stored in the exception.
     *
     * @return The constant rvalue reference to the data.
     */
    UTL_ATTRIBUTES(NODISCARD, CONST) T const&& data() const&& noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return data_;
    }

    /**
     * @brief Retrieves the data associated with the exception.
     *
     * This function returns a reference to the data stored in the exception.
     *
     * @return The reference to the data.
     */
    UTL_ATTRIBUTES(NODISCARD, CONST) T& data() & noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) { return data_; }

    /**
     * @brief Retrieves the data associated with the exception.
     *
     * This function returns an rvalue reference to the data stored in the exception.
     *
     * @return The rvalue reference to the data.
     */
    UTL_ATTRIBUTES(NODISCARD, CONST) T&& data() && noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) { return data_; }

private:
    T data_;
};

using program_exception = basic_exception<void>;

UTL_NAMESPACE_END
