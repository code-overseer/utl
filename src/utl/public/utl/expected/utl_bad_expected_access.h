// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/exception/utl_program_exception.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename E>
class __UTL_PUBLIC_TEMPLATE expected;

template <typename E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public basic_exception<void> {
    using base_type = basic_exception<void>;

protected:
    __UTL_HIDE_FROM_ABI inline bad_expected_access(__UTL source_location const& src)
        : base_type(__UTL exceptions::message_vformat::forward(
              {"[UTL] bad access to utl::expected", src})) {}
    __UTL_HIDE_FROM_ABI inline bad_expected_access(bad_expected_access const&) = default;
    __UTL_HIDE_FROM_ABI inline bad_expected_access(bad_expected_access&&) = default;
    __UTL_HIDE_FROM_ABI inline bad_expected_access& operator=(bad_expected_access const&) = default;
    __UTL_HIDE_FROM_ABI inline bad_expected_access& operator=(bad_expected_access&&) = default;
    __UTL_HIDE_FROM_ABI inline ~bad_expected_access() = default;
};

template <typename E>
class bad_expected_access : public bad_expected_access<void> {
    using base_type = bad_expected_access<void>;

public:
    __UTL_HIDE_FROM_ABI bad_expected_access(E e, __UTL source_location const& src)
        : base_type(src)
        , error_(__UTL move(e)) {}
    __UTL_HIDE_FROM_ABI bad_expected_access(bad_expected_access const&) = default;
    __UTL_HIDE_FROM_ABI bad_expected_access(bad_expected_access&&) = default;
    __UTL_HIDE_FROM_ABI bad_expected_access& operator=(bad_expected_access const&) = default;
    __UTL_HIDE_FROM_ABI bad_expected_access& operator=(bad_expected_access&&) = default;

    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE, _HIDE_FROM_ABI) inline E const& error() const& noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) {
        return error_;
    }

    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE, _HIDE_FROM_ABI) inline E const&& error() const&& noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) {
        return error_;
    }

    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE, _HIDE_FROM_ABI) inline E& error() & noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) {
        return error_;
    }

    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE, _HIDE_FROM_ABI) inline E&& error() && noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) {
        return error_;
    }

private:
    E error_;
};

UTL_NAMESPACE_END
