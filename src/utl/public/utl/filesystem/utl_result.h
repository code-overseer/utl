// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/expected/utl_expected.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/filesystem/utl_result.h"
#include "utl/system_error/utl_error_category.h"
#include "utl/system_error/utl_error_code.h"
#include "utl/system_error/utl_error_condition.h"

__UFS_NAMESPACE_BEGIN

enum class error_value : int {
    file_type_mismatch
};

__UTL_ABI_PUBLIC error_category const& filesystem_category() noexcept;

__UTL_HIDE_FROM_ABI inline error_code make_error_code(error_value code) noexcept {
    return error_code(static_cast<int>(code), filesystem_category());
}

enum class failure_source : int {
    filesystem_api,
    utl_api,
    system,
};

__UTL_HIDE_FROM_ABI inline error_condition make_error_condition(failure_source code) noexcept {
    return error_condition(static_cast<int>(code), filesystem_category());
}

__UFS_NAMESPACE_END

UTL_NAMESPACE_BEGIN

template <>
struct is_error_code_enum<__UFS error_value> : true_type {};

template <>
struct is_error_condition_enum<__UFS failure_source> : true_type {};

template <typename T>
using result = __UTL expected<T, __UTL error_code>;

UTL_NAMESPACE_END
