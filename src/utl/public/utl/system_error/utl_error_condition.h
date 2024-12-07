// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/span/utl_span_fwd.h"
#include "utl/string/utl_string_fwd.h"

#include "utl/concepts/utl_enum_type.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/system_error/utl_errc.h"
#include "utl/system_error/utl_error_category.h"
#include "utl/system_error/utl_error_common.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_is_enum.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/utility/utl_customization_point.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_error_condition_enum : false_type {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_error_condition_enum<errc> : true_type {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_error_condition_enum_v = is_error_condition_enum<T>::value;
#endif

namespace details {
namespace error_condition {

void make_error_condition() = delete;

struct make_error_condition_t {
    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(
        is_error_condition_enum<T>::value && UTL_TRAIT_is_enum(T) && UTL_TRAIT_is_same(decltype(make_error_condition(T{})), error_condition))>
    UTL_CONSTRAINT_CXX20(is_error_condition_enum<T>::value && requires(T val) {
        { make_error_condition(val) } -> __UTL same_as<__UTL error_condition>; 
    })
    __UTL error_condition operator()(T val) const noexcept;
};

UTL_DEFINE_CUSTOMIZATION_POINT(__UTL details::error_condition::make_error_condition_t, make);

} // namespace error_condition
} // namespace details

class __UTL_ABI_PUBLIC error_condition : details::error_code::error_compare<error_condition> {
public:
    __UTL_HIDE_FROM_ABI inline error_condition() noexcept
        : value_{0}
        , category_{__UTL addressof(__UTL generic_category())} {}

    __UTL_HIDE_FROM_ABI inline constexpr error_condition(
        int value, error_category const& category) noexcept
        : value_{value}
        , category_{__UTL addressof(category)} {}

    template <typename E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(decltype(__UTL details::error_condition::make(E{}))))>
    UTL_CONSTRAINT_CXX20(requires { __UTL details::error_condition::make(E{}); })
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 error_condition(E code) noexcept
        : error_condition{__UTL details::error_condition::make(code)} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void assign(
        int value, error_category const& category) noexcept {
        value_ = value;
        category_ = __UTL addressof(category);
    }

    template <typename E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(decltype(__UTL details::error_condition::make(E{}))))>
    UTL_CONSTRAINT_CXX20(requires { __UTL details::error_condition::make(E{}); })
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 error_condition& operator=(E code) noexcept {
        return *this = __UTL details::error_condition::make(code);
    }

    __UTL_HIDE_FROM_ABI void clear() noexcept {
        value_ = 0;
        category_ = __UTL addressof(__UTL generic_category());
    }

    __UTL_HIDE_FROM_ABI inline constexpr int value() const noexcept { return value_; }
    __UTL_HIDE_FROM_ABI inline error_category const* category() const noexcept { return category_; }

    __UTL_HIDE_FROM_ABI explicit inline operator bool() const noexcept { return value_ != 0; }

    __UTL_HIDE_FROM_ABI inline size_t message(span<char>) const noexcept;

    __UTL_HIDE_FROM_ABI inline string message() const;

    __UTL_HIDE_FROM_ABI inline size_t message(char* buffer, size_t size) const noexcept {
        return category_->message(value_, buffer, size);
    }

private:
    int value_;
    error_category const* category_;
};

namespace details {
namespace error_condition {

template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(
        is_error_condition_enum<T>::value && UTL_TRAIT_is_enum(T) && UTL_TRAIT_is_same(decltype(make_error_condition(T{})), error_condition))>
    UTL_CONSTRAINT_CXX20(is_error_condition_enum<T>::value && requires(T val) {
        { make_error_condition(val) } -> __UTL same_as<__UTL error_condition>; 
    })
__UTL error_condition make_error_condition_t::operator()(T val) const noexcept {
    return make_error_condition(val);
}

} // namespace error_condition
} // namespace details

__UTL_HIDE_FROM_ABI inline error_condition make_error_condition(errc code) noexcept {
    return error_condition(static_cast<int>(code), generic_category());
}

template <typename>
struct hash;

template <>
struct __UTL_PUBLIC_TEMPLATE hash<error_condition> {
    __UTL_HIDE_FROM_ABI inline constexpr size_t operator()(
        error_condition const& code) const noexcept {
        return static_cast<size_t>(code.value());
    }
};

UTL_NAMESPACE_END
