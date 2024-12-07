// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/compare/utl_compare_fwd.h"
#include "utl/span/utl_span_fwd.h"
#include "utl/string/utl_string_fwd.h"
#include "utl/system_error/utl_system_error_fwd.h"

#include "utl/concepts/utl_convertible_to.h"
#include "utl/memory/utl_addressof.h"
#include "utl/system_error/utl_error_common.h"

UTL_NAMESPACE_BEGIN

class __UTL_ABI_PUBLIC error_category : details::error_code::category_compare<error_category> {
public:
    __UTL_HIDE_FROM_ABI inline constexpr error_category() noexcept = default;
    error_category(error_category const&) = delete;
    error_category& operator=(error_category const&) = delete;

    __UTL_HIDE_FROM_ABI_VIRTUAL virtual char const* name() const noexcept = 0;

    __UTL_HIDE_FROM_ABI_VIRTUAL inline virtual size_t message(
        int condition, char* str, size_t size) const noexcept = 0;

    __UTL_HIDE_FROM_ABI_VIRTUAL inline virtual error_condition default_error_condition(
        int val) const noexcept;

    __UTL_HIDE_FROM_ABI_VIRTUAL inline virtual bool equivalent(
        int code, error_condition const& condition) const noexcept;

    __UTL_HIDE_FROM_ABI_VIRTUAL inline virtual bool equivalent(
        error_code code, int condition) const noexcept;

    __UTL_HIDE_FROM_ABI inline size_t message(int condition, span<char> buffer) const noexcept;

    __UTL_HIDE_FROM_ABI inline string message(int condition) const;

    __UTL_HIDE_FROM_ABI inline bool operator==(error_category const& rhs) const noexcept {
        return this == __UTL addressof(rhs);
    }

    __UTL_HIDE_FROM_ABI inline bool operator!=(error_category const& rhs) const noexcept {
        return this != __UTL addressof(rhs);
    }

    __UTL_HIDE_FROM_ABI inline bool operator<(error_category const& rhs) const noexcept {
        return this < __UTL addressof(rhs);
    }

#if UTL_CXX20
    using category_compare<error_category>::operator<=>;
#endif

protected:
    UTL_CONSTEXPR_CXX20 ~error_category() = default;
};

UTL_ATTRIBUTES(_ABI_PUBLIC, CONST) error_category const& generic_category() noexcept;

UTL_ATTRIBUTES(_ABI_PUBLIC, CONST) error_category const& system_category() noexcept;

#if UTL_TARGET_MICROSOFT
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST) error_category const& winapi_category() noexcept;
#endif
UTL_NAMESPACE_END
