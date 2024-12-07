// Copyright 2023-2024 Bryan Wong

#pragma once

#ifndef UTL_SYSTEM_ERROR_PRIVATE_HEADER_GUARD
#  error "Private header accessed"
#endif

#include "utl/compare/utl_compare_fwd.h"

#include "utl/span/utl_span.h"
#include "utl/string/utl_basic_short_string.h"

UTL_NAMESPACE_BEGIN

inline size_t error_code::message(span<char> buffer) const noexcept {
    return this->message(buffer.data(), buffer.size());
}

inline string error_code::message() const {
    string str;
    str.resize(this->message(nullptr, 0));
    this->message(str.data(), str.size() + 1);
    return str;
}

inline error_condition error_code::default_error_condition() const noexcept {
    return category_->default_error_condition(value_);
}

inline size_t error_condition::message(span<char> buffer) const noexcept {
    return this->message(buffer.data(), buffer.size());
}

inline string error_condition::message() const {
    string str;
    str.resize(this->message(nullptr, 0));
    this->message(str.data(), str.size() + 1);
    return str;
}

inline size_t error_category::message(int condition, span<char> buffer) const noexcept {
    return message(condition, buffer.data(), buffer.size());
}

inline string error_category::message(int condition) const {
    string str;
    str.resize(message(condition, nullptr, 0));
    message(condition, str.data(), str.size() + 1);
    return str;
}

inline error_condition error_category::default_error_condition(int val) const noexcept {
    return error_condition{val, *this};
}

__UTL_HIDE_FROM_ABI inline bool operator==(
    error_code const& left, error_code const& right) noexcept {
    return left.category() == right.category() && left.value() == right.value();
}

__UTL_HIDE_FROM_ABI inline bool operator==(
    error_code const& left, error_condition const& right) noexcept {
    return left.category()->equivalent(left.value(), right) ||
        right.category()->equivalent(left, right.value());
}

__UTL_HIDE_FROM_ABI inline bool operator==(
    error_condition const& left, error_code const& right) noexcept {
    return right == left;
}

__UTL_HIDE_FROM_ABI inline bool operator==(
    error_condition const& left, error_condition const& right) noexcept {
    return left.category() == right.category() && left.value() == right.value();
}

inline bool error_category::equivalent(int code, error_condition const& condition) const noexcept {
    return default_error_condition(code) == condition;
}

inline bool error_category::equivalent(error_code code, int condition) const noexcept {
    return this == code.category() && code.value() == condition;
}

__UTL_HIDE_FROM_ABI inline bool operator!=(
    error_code const& left, error_code const& right) noexcept {
    return !(left == right);
}

__UTL_HIDE_FROM_ABI inline bool operator!=(
    error_code const& left, error_condition const& right) noexcept {
    return !(left == right);
}

__UTL_HIDE_FROM_ABI inline bool operator!=(
    error_condition const& left, error_code const& right) noexcept {
    return !(left == right);
}

__UTL_HIDE_FROM_ABI inline bool operator!=(
    error_condition const& left, error_condition const& right) noexcept {
    return !(left == right);
}

__UTL_HIDE_FROM_ABI inline bool operator<(
    error_code const& left, error_code const& right) noexcept {
    return left.category() < right.category() ||
        (left.category() == right.category() && left.value() < right.value());
}

__UTL_HIDE_FROM_ABI inline bool operator<(
    error_condition const& left, error_condition const& right) noexcept {
    return left.category() < right.category() ||
        (left.category() == right.category() && left.value() < right.value());
}

UTL_NAMESPACE_END
